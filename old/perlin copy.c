#include <raymath.h>
#include <time.h>
#include <stdlib.h>

static int perlin_perm[256];

void
perlin_make_permutation()
{
    for(int i = 0; i < 256; ++i) perlin_perm[i] = i;

    srand(time(NULL));
    for(int i = 255; i; --i) {
        int j = (int)(rand()*(i-1));
        int tmp = perlin_perm[i];
        perlin_perm[i] = perlin_perm[j];
        perlin_perm[j] = tmp;
    }
}

Vector2
perlin_constant_vector(int v)
{
    v &= 3;
    return (Vector2){
        -1 + 2*(v < 1 || v > 2),
        -1 + 2*(v > 0 || v < 3)
    };
}

float
perlin_fade(float t)
{
    return ((6*t - 15)*t + 10)*t*t*t;
}

float 
perlin_lerp(float t,
            float a0,
            float a1)
{
    return a0 + t*(a1-a0);
}

float 
perlin_noise(float x,
             float y)
{
    int X = (int)x & 255;
    int Y = (int)y & 255;

    float xdif = x - (int)x;
    float ydif = y - (int)y;
    
    Vector2 corner[] = {
        {xdif-1, ydif-1}, {xdif, ydif-1},
        {xdif-1, ydif}, {xdif, ydif}
    };

    Vector2 value[] = {
        perlin_perm[perlin_perm[X+1]+Y+1],
        perlin_perm[perlin_perm[X]+Y+1],
        perlin_perm[perlin_perm[X+1]+Y+1],
        perlin_perm[perlin_perm[X]+Y]
    };

    int dot[4];
    for(size_t i = 0; i < 4; ++i) {
        dot[i] = Vector2DotProduct(corner[i], value[i]);
    }

    float u = perlin_fade(xdif);
    float v = perlin_fade(ydif);

    return perlin_lerp(u,
        perlin_lerp(v, dot[3], dot[1]),
        perlin_lerp(v, dot[2], dot[0])
    );
}