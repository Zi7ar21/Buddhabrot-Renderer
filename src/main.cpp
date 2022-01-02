#include <iostream>
#include <fstream>
#include <thread>

#include <common.hpp>

void compute_orbit(vec2 *orbit_buffer, unsigned int orbit_size, unsigned int *orbit_length, bool *draw_orbit, uint32_t *ns)
{
	vec2 c = 2.0f * BAILOUT * (rand2(ns) - 0.5f);

	/*
	float r = uniform_random(ns);
	vec2 c = BAILOUT * r * udir2(ns);
	*/

	/*
	vec2 c = nrand2(vec2(0), BAILOUT, ns);
	*/
	
	vec2 z = c;

	for(unsigned int i = 0u; i < orbit_size; i++)
	{
		if(dot_p(z) > BAILOUT * BAILOUT)
		{
			*draw_orbit = true;
			*orbit_length = i;
			break;
		}

		z = vec2(z.x * z.x - z.y * z.y, 2.0f * z.x * z.y) + c;

		orbit_buffer[i] = z;
	}
}

void thread_worker(vec3 *image, uvec2 resolution, vec2 *orbit_buffer, uint32_t tid)
{
	uint32_t ns = tid + 1u;

	for(unsigned int orbit = 0u; orbit < ORBITS; orbit++)
	{
		bool draw_orbit = false;
		unsigned int orbit_length = 0u;

		compute_orbit(orbit_buffer, ORBIT_SIZE, &orbit_length, &draw_orbit, &ns);

		if(draw_orbit)
		{
			for(unsigned int i = 0u; i < orbit_length; i++)
			{
				orbit_buffer[i] *= 0.5f;

				// uv = 2.0 * (fragCoord.xy - 0.5 * resolution) / max(resolution.x * resolution.y)

				orbit_buffer[i].y *= ( float(resolution.x) / float(resolution.y) );
				orbit_buffer[i] = ( (0.5f * orbit_buffer[i]) + 0.5f ) * vec2(resolution);

				ivec2 pixel_coord;
				pixel_coord = ivec2(orbit_buffer[i]);

				if( pixel_coord.x >= 0 && pixel_coord.x < int(resolution.x) && pixel_coord.y >= 0 && pixel_coord.y < int(resolution.y) )
				{
					unsigned int index = pixel_coord.x + pixel_coord.y * resolution.x;
					image[index] += color_palette( float(orbit_length) / float(ORBIT_SIZE) ) * ACCUMULATION;

					index = pixel_coord.x + (int(resolution.y) - pixel_coord.y - 1) * resolution.x;
					image[index] += color_palette( float(orbit_length) / float(ORBIT_SIZE) ) * ACCUMULATION;
				}
			}
		}
	}
}

void render_image(vec3 *image, uvec2 resolution)
{
	unsigned int threads = std::thread::hardware_concurrency();

	#if THREADS != 0u
	threads = std::min(threads, THREADS);
	#endif

	std::cout << "Using " << threads << " threads..." << std::endl;

	vec3 *thread_image[threads];
	vec2 *orbit_buffer[threads];

	for(unsigned int tid = 0u; tid < threads; tid++)
	{
		thread_image[tid] = (vec3*)malloc(sizeof(vec3) * resolution.x * resolution.y);

		for(unsigned int i = 0u; i < resolution.x * resolution.y; i++)
		{
			thread_image[tid][i] = vec3(0);
		}

		orbit_buffer[tid] = (vec2*)malloc(sizeof(vec2) * ORBIT_SIZE);

		for(unsigned int i = 0u; i < ORBIT_SIZE; i++)
		{
			orbit_buffer[tid][i] = vec2(0);
		}
	}

	std::cout << "Calculating and drawing " << ORBITS * threads << " orbits (" << ORBITS << " orbits per thread)..." << std::endl;

	std::thread t[threads];

	for(unsigned int tid = 0u; tid < threads; tid++)
	{
		t[tid] = std::thread(thread_worker, thread_image[tid], resolution, orbit_buffer[tid], tid);
	}

	for(unsigned int tid = 0u; tid < threads; tid++)
	{
		t[tid].join();
	}

	for(unsigned int tid = 0u; tid < threads; tid++)
	{
		free(orbit_buffer[tid]);
	}

	for(unsigned int tid = 0u; tid < threads; tid++)
	{
		for(unsigned int i = 0u; i < resolution.x * resolution.y; i++)
		{
			image[i] += thread_image[tid][i] / float(threads);
		}
	}

	for(unsigned int tid = 0u; tid < threads; tid++)
	{
		free(thread_image[tid]);
	}
}

void write_render_HDR(vec3 *image, uvec2 resolution)
{
	std::ofstream image_file;

	image_file.open("render.pfm");

	image_file << "PF\n" << resolution.x << " " << resolution.y << "\n-1.0\n";

	for(unsigned int i = 0u; i < resolution.x * resolution.y; i++)
	{
		// https://stackoverflow.com/questions/30923685/writing-floats-to-a-binary-file-in-c-equivalent-of-javas-dataoutputstream-w
		image_file.write( reinterpret_cast<const char*>(&image[i].r), sizeof(float) );
		image_file.write( reinterpret_cast<const char*>(&image[i].g), sizeof(float) );
		image_file.write( reinterpret_cast<const char*>(&image[i].b), sizeof(float) );
	}

	image_file.close();
}

int main()
{
	unsigned int threads = std::thread::hardware_concurrency();

	if(threads < 1u)
	{
		std::cout << "Error! This machine reports having " << threads << " threads." << std::endl;

		return EXIT_FAILURE;
	}

	std::cout << "This machine reports having " << threads << " threads." << std::endl;

	const uvec2 resolution = uvec2(2048, 1080);

	std::cout << "Initializing image buffer (" << resolution.x << "x" << resolution.y << ")..." << std::endl;

	vec3 *image;

	image = (vec3*)malloc(sizeof(vec3) * resolution.x * resolution.y);

	for(unsigned int i = 0u; i < resolution.x * resolution.y; i++)
	{
		image[i] = vec3(0);
	}

	std::cout << "Starting render..." << std::endl;

	render_image(image, resolution);

	std::cout << "Applying tonemap..." << std::endl;

	for(unsigned int i = 0u; i < resolution.x * resolution.y; i++)
	{
		image[i] = glm::clamp(1.0f - glm::exp(-glm::max(image[i], 0.0f) * EXPOSURE), 0.0f, 1.0f);
	}

	std::cout << "Saving render..." << std::endl;

	write_render_HDR(image, resolution);

	std::cout << "Cleaning up..." << std::endl;

	free(image);

	std::cout << "Done!" << std::endl;

	return EXIT_SUCCESS;
}