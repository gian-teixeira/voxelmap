#include <raymath.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

static int perlin_perm[256];

void
perlin_make_permutation()
{
    for(int i = 0; i < 256; ++i) perlin_perm[i] = i;
    
    srand(time(NULL));
    for(int i = 255; i; --i) {
        int j = (int)(rand() & 255);
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

float perlin_lerp(float t, float a0, float a1)
{
    return a0 + t*(a1-a0);
}

float perlin_interp(float t, float x, float y)
{
    //return perlin_lerp(t*t*(3-2*t), x, y);
    return perlin_lerp(t*t*t, x, y);
}

int perlin_random(int x, int y)
{
    int tmp = perlin_perm[y & 255];
    return perlin_perm[(x+tmp) & 255];
}

float perlin_noise(float x, float y)
{
    int ix = x;
    int iy = y;

    float fx = x - ix;
    float fy = y - iy;

    int s = perlin_random(ix, iy);
    int t = perlin_random(ix+1, iy);
    int u = perlin_random(ix, iy+1);
    int v = perlin_random(ix+1, iy+1);

    float low = perlin_interp(fx, s, t);
    float high = perlin_interp(fx, u, v);
    return perlin_interp(fy, low, high);
}

float perlin(float x, float y, float freq, float depth)
{
    float xa = x*freq;
    float ya = y*freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for(i = 0; i < depth; i++) {
        div += 256 * amp;
        fin += perlin_noise(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}