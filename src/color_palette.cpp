#include <common.hpp>

#ifdef SPECTRAL_PALETTE
// https://www.shadertoy.com/view/ls2Bz1

vec3 bump3y(vec3 x, vec3 yoffset)
{
	vec3 y = vec3(1.0f) - (x * x);
	y = glm::clamp(y - yoffset, 0.0f, 1.0f);
	return y;
}

// spectral_zucconi6
vec3 color_palette(float t)
{
	// w: [0  , 1  ]
	// x: [400, 700]

	//t = 1.0f - t;
	t = glm::pow(t, 0.5f);

	t = glm::clamp(t, 0.0f, 1.0f);

	const vec3 c1 = vec3(3.54585104f, 2.93225262f, 2.41593945f);
	const vec3 x1 = vec3(0.69549072f, 0.49228336f, 0.27699880f);
	const vec3 y1 = vec3(0.02312639f, 0.15225084f, 0.52607955f);

	const vec3 c2 = vec3(3.90307140f, 3.21182957f, 3.96587128f);
	const vec3 x2 = vec3(0.11748627f, 0.86755042f, 0.66077860f);
	const vec3 y2 = vec3(0.84897130f, 0.88445281f, 0.73949448f);

	return bump3y(c1 * (t - x1), y1) + bump3y(c2 * (t - x2), y2);
}
#else
vec3 color_palette(float t)
{
	//t = 1.0f - t;
	t = glm::pow(t, 0.5f);

	return 0.5f * (
	vec3(
	glm::cos( 2.0f * M_PI * ( t - (0.0f / 3.0f) ) ),
	glm::cos( 2.0f * M_PI * ( t - (1.0f / 3.0f) ) ),
	glm::cos( 2.0f * M_PI * ( t - (2.0f / 3.0f) ) )
	)
	+ 1.0f);
}
#endif