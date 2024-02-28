#ifndef __PERLIN_H__
#define __PERLIN_H__

#include <raymath.h>
#include <time.h>
#include <stdlib.h>

void perlin_make_permutation();
Vector2 perlin_constant_vector(int v);
float perlin_fade(float t);
float perlin_lerp(float t, float a0, float a1);
float perlin_noise(float x, float y);

#endif