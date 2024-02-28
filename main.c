#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

#include "perlin.h"

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 900
#define CUBE_SIZE 30
#define CHUNK_SIZE 9
#define CHUNK_VOLUME (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE/2)

#define CHUNK_LENGTH 13
#define CHUNK_HEIGHT 15

const Vector2 cube_sup = {0, -CUBE_SIZE};
const float cube_light[] = {
    0.3f, 0.25f, 0.25f, 
    0.0f, 0.0f, 0.3f
};

typedef struct {
    float x, y, z;
    Color color;
} CubeInfo;

typedef struct {
    int zindex[6];
    Color col[6];
    unsigned area[6];
} RenderInfo;

RenderInfo canvas[CHUNK_LENGTH][CHUNK_LENGTH*CHUNK_HEIGHT];
static CubeInfo chunk[CHUNK_LENGTH+2][CHUNK_LENGTH+2][CHUNK_HEIGHT+2];
int chunk_height[CHUNK_LENGTH+2][CHUNK_LENGTH+2];

void
draw_outline(CubeInfo cube)
{
    Vector2 center = {
        CUBE_SIZE*(cube.x*sin(PI/3) - cube.y*sin(PI/3)),
        CUBE_SIZE*(cube.x*cos(PI/3) + cube.y*cos(PI/3) - cube.z)
    };

    int ix = cube.x + CHUNK_LENGTH/2 + 1;
    int iy = cube.y + CHUNK_LENGTH/2 + 1;
    int iz = cube.z + 1;

    #define _t(x,y,z) (chunk[x][y][z].color.a == 0)

    const char mask[] = {
        _t(ix,iy-1,iz) && _t(ix,iy,iz+1),
        _t(ix,iy-1,iz),
        _t(ix,iy,iz-1) || _t(ix+1,iy,iz) && !_t(ix+1,iy,iz-1),
        _t(ix,iy,iz-1) && _t(ix,iy+1,iz) || _t(ix,iy+1,iz) && !_t(ix,iy+1,iz-1),
        chunk[ix-1][iy][iz].color.a == 0 && chunk[ix][iy+1][iz].color.a == 0,
        chunk[ix-1][iy][iz].color.a == 0 && chunk[ix][iy][iz+1].color.a == 0,
        _t(ix+1,iy,iz) && _t(ix,iy,iz+1),
        _t(ix+1,iy,iz) && _t(ix,iy+1,iz),
        _t(ix,iy,iz+1) && _t(ix,iy+1,iz),
    };
    const Vector2 sup = {0, -CUBE_SIZE};
    const Vector2 guide[][2] = {
        { sup, Vector2Rotate(sup, PI/3) },
        { Vector2Rotate(sup,PI/3), Vector2Rotate(sup, 2*PI/3) },
        { Vector2Rotate(sup,2*PI/3), Vector2Rotate(sup, PI) },
        { Vector2Rotate(sup,PI), Vector2Rotate(sup, 4*PI/3) },
        { Vector2Rotate(sup,4*PI/3), Vector2Rotate(sup, 5*PI/3) },
        { Vector2Rotate(sup,5*PI/3), Vector2Rotate(sup, 2*PI) },
        { Vector2Rotate(sup,PI/3), {0,0} },
        { Vector2Rotate(sup,PI), {0.0} },
        { Vector2Rotate(sup,5*PI/3), {0,0} },
    };

    for(int i = 0; i < 9; i++) {
        if(!mask[i]) continue;
        DrawLineV(
            Vector2Add(center,guide[i][0]), 
            Vector2Add(center,guide[i][1]), 
            BLACK);
    }

    /*Vector2 last = {0, -CUBE_SIZE};
    for(float angle = PI/3;
        angle <= 2*PI;
        angle += PI/3)
    {
        int iangle = (int)(3*angle/PI)-1;
        if(mask[iangle]) DrawLineV(
            Vector2Add(center,last), 
            Vector2Add(center,Vector2Rotate(last,PI/3)), 
            ColorBrightness(BLACK, cube.z/CHUNK_HEIGHT));
        last = Vector2Rotate(last,PI/3);
    }*/
}

void
draw_cube(CubeInfo cube)
{    
    Vector2 center = {
        CUBE_SIZE*(cube.x*sin(PI/3) - cube.y*sin(PI/3)),
        CUBE_SIZE*(cube.x*cos(PI/3) + cube.y*cos(PI/3) - cube.z)
    };

    for(float angle = PI/3;
        angle <= 2*PI;
        angle += PI/3)
    {
        int iangle = (int)((angle-1)/(PI/3));
        DrawTriangle(center,
            Vector2Add(center,Vector2Rotate(cube_sup, angle)),
            Vector2Add(center,Vector2Rotate(cube_sup, angle-PI/3)),
            ColorBrightness(
                cube.color, 
                cube_light[iangle] + (iangle == 0 || iangle == 5 ? 0.1*cube.z : 0)
            ));
        if(cube.color.a) draw_outline(cube);
    }
}

void
draw()
{
    for(long x = 0; x < CHUNK_LENGTH; x++) {
        for(long y = 0; y < CHUNK_LENGTH; y++) {
            for(long z = 0; z < CHUNK_HEIGHT; z++) {
                draw_cube(chunk[x+1][y+1][z+1]);
            }
        }
    }
}

void 
gen_tree(int x, int y) {
    int z = chunk_height[x][y]+1;

    chunk[x][y][z] = (CubeInfo){x,y,z,BROWN};
}

void
chunk_gen()
{
    /*chunk[0][0][0] = (CubeInfo){0,0,0,LIGHTGRAY};
    chunk[0][0][1] = (CubeInfo){0,0,1,LIGHTGRAY};
    //chunk[1][0][0] = (CubeInfo){1,0,0,LIGHTGRAY};
    chunk[1][0][1] = (CubeInfo){1,0,1,LIGHTGRAY};
    printf("%d\n", chunk[0][1][0].color.a);*/
    

    //CubeInfo *chunk = malloc(sizeof(float)*CHUNK_VOLUME);
    perlin_make_permutation();
    for(long x = 0; x < CHUNK_LENGTH; x++) {
        for(long y = 0; y < CHUNK_LENGTH; y++) {
            float height = perlin(x, y, 0.1, CHUNK_HEIGHT)*5;
            for(long z = 0; z < CHUNK_HEIGHT; z++) {
                chunk[x+1][y+1][z+1] = (CubeInfo){
                    x - CHUNK_LENGTH/2,
                    y - CHUNK_LENGTH/2,
                    z, 
                    (z > height ? (Color){0,0,0,0} : 
                        (height < 1 ? BLUE : DARKGREEN))
                };
                chunk_height[x+1][y+1] = height;
            }
        }
    }
}


int
main()
{
    Camera2D camera = {0};
    camera.target = (Vector2){0,0};
    camera.offset = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    camera.zoom = 1.0f;

    CubeInfo tmp = {0,0,0};
    chunk_gen();
    //gen_tree(10,10);


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Poorject");
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        if(IsKeyDown(KEY_D)) ++tmp.x;
        if(IsKeyDown(KEY_A)) --tmp.x;
        if(IsKeyDown(KEY_S)) ++tmp.y;
        if(IsKeyDown(KEY_W)) --tmp.y;
        if(IsKeyDown(KEY_SPACE)) ++tmp.z;
        if(IsKeyDown(KEY_LEFT_SHIFT)) --tmp.z;

        BeginDrawing();
        BeginMode2D(camera);
            
            ClearBackground(GRAY);
            draw();
            /*for(size_t i = 0; i < CHUNK_VOLUME; i++)
                draw_cube(chunk[i]);*/
            //DrawRectangle(0,0,100,50,WHITE);
        EndMode2D();
        EndDrawing();
    }

    return 0;
}