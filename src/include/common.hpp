#include <glm/glm.hpp>
using namespace glm;

// ##### Parameters #####

// number of threads to use
#define THREADS 8u

// maximum orbits
#define ORBITS 12500000u

// maximum orbit points or "iterations"
#define ORBIT_SIZE 1024u

// fractal bailout
#define BAILOUT 2.0f

// amount each point contributes to the final image (TODO: replace with a better way of controlling brightness)
#define ACCUMULATION 0.04f

// if defined: replace HSV color palette with spectral
#define SPECTRAL_PALETTE

// Tonemap Exposure
#define EXPOSURE 1.0f

// ##### Maths #####

#define dot_p(vector) dot(vector, vector)
//inline float dot_p(vec2 vector) { return dot(vector, vector); }
//inline float dot_p(vec3 vector) { return dot(vector, vector); }
//inline float dot_p(vec4 vector) { return dot(vector, vector); }

// ##### Random Number Generator #####

uint32_t triple32(uint32_t x); // https://nullprogram.com/blog/2018/07/31/

float uniform_random(uint32_t *ns); // Random Value Between 0.0 and 1.0

vec2 rand2(uint32_t *ns); // 2-Component Uniform Random Vector
vec3 rand3(uint32_t *ns); // 3-Component Uniform Random Vector
vec4 rand4(uint32_t *ns); // 4-Component Uniform Random Vector

vec2 udir2(uint32_t *ns); // Uniformly distributed random point on a unit circle
vec3 udir3(uint32_t *ns); // Uniformly distributed random point on the surface of a unit sphere

vec2 nrand2(vec2 mean, float sigma, uint32_t *ns); // 2-Dimensional Gaussian Distribution
vec3 nrand3(vec3 mean, float sigma, uint32_t *ns); // 3-Dimensional Gaussian Distribution
vec4 nrand4(vec4 mean, float sigma, uint32_t *ns); // 4-Dimensional Gaussian Distribution

// ##### Color Palette #####

vec3 color_palette(float t);