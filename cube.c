#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define CUBE_SIZE 30

const float cube_light[] = {
    0.3f, 0.25f, 0.25f, 
    0.0f, 0.0f, 0.3f
};


typedef enum {
    VOID,
    SOLID,
    FEATURE
} BlockType;

typedef struct {
    BlockType type;
    Vector2 hitbox;
    Vector2 size;
    union {
        Color color;
        Texture2D texture;
    };
} CubeInfo;

typedef struct {
    unsigned length;
    unsigned height;
    CubeInfo ***blocks;
} Chunk;

void
draw_cube(CubeInfo cube,
          int x, 
          int y, 
          int z)
{
    Vector2 center = {
        CUBE_SIZE*(x*sin(PI/3) - y*sin(PI/3)),
        CUBE_SIZE*(x*cos(PI/3) + y*cos(PI/3) - z)
    };


    switch (cube.type) {
    case FEATURE:
        float side = 2*cos(PI/6)*CUBE_SIZE*cube.size.x;
        Rectangle rec = {
            .x = center.x - cos(PI/6)*CUBE_SIZE,
            .y = center.y - (side - CUBE_SIZE/2),
            .height = side,
            .width = side
        };
        DrawTexturePro(
            cube.texture, 
            (Rectangle){ 0.0f, 0.0f, cube.texture.width, cube.texture.height },
            rec, (Vector2){0,0}, 0.0, WHITE);
        break; 
    case SOLID:
        for(float angle = PI/3;
            angle <= 2*PI;
            angle += PI/3)
        {
            int iangle = (int)((angle-1)/(PI/3));
            DrawTriangle(center,
                Vector2Add(center,Vector2Rotate((Vector2){0,-CUBE_SIZE}, angle)),
                Vector2Add(center,Vector2Rotate((Vector2){0,-CUBE_SIZE}, angle-PI/3)),
                ColorBrightness(
                    cube.color, 
                    cube_light[iangle] + (iangle == 0 || iangle == 5 ? 0.1*z : 0)
                ));
        }
    }
}

Chunk*
chunk_create(unsigned length, 
             unsigned height)
{
    Chunk *chunk = malloc(sizeof(Chunk));
    chunk->length = length;
    chunk->height = height;
    
    chunk->blocks = malloc(length*sizeof(CubeInfo**));
    for(size_t x = 0; x < length; x++) {
        chunk->blocks[x] = malloc(length*sizeof(CubeInfo*));
        for(size_t y = 0; y < length; y++) {
            chunk->blocks[x][y] = malloc(height*sizeof(CubeInfo));
            for(size_t z = 0; z < height; z++) {
                chunk->blocks[x][y][z] = (CubeInfo){
                    .type = VOID
                };
            } 
        }
    }

    return chunk;
}

void
chunk_destroy(Chunk *chunk)
{
    assert(chunk);
    
    for(size_t i = 0; i < chunk->length; i++) {
        for(size_t j = 0; j < chunk->length; j++) {
            free(chunk->blocks[i][j]);
        }
        free(chunk->blocks[i]);
    }
    free(chunk->blocks);
    free(chunk);
}

void
draw_chunk(Chunk *chunk)
{
    for(long x = 0; x < chunk->length; x++) {
        for(long y = 0; y < chunk->length; y++) {
            for(long z = 0; z < chunk->height; z++) {
                draw_cube(
                    chunk->blocks[x][y][z],
                    x - chunk->length,
                    y - chunk->length,
                    z - chunk->height
                );
            }
        }
    }
}

int32_t main()
{
    Chunk *chunk = chunk_create(3,3);

    Camera2D camera = {0};
    camera.target = (Vector2){0,0};
    camera.offset = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    camera.zoom = 1.0f;


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Poorject");
    SetTargetFPS(60);

    chunk->blocks[1][1][1] = (CubeInfo){
        .type = FEATURE,
        .size = {1,1},
        .hitbox = {1,1},
        .texture = LoadTexture("grass.png")
    };
    chunk->blocks[1][1][0] = (CubeInfo){
        .type = SOLID,
        .size = {1,1},
        .hitbox = {1,1},
        .color = DARKGREEN
    };

    //UnloadTexture(chunk->blocks[1][1][0].texture);


    while(!WindowShouldClose()) {

        BeginDrawing();
        BeginMode2D(camera);
            
            ClearBackground(GRAY);
            draw_chunk(chunk);
            
        EndMode2D();
        EndDrawing();
    }

    chunk_destroy(chunk);

    return 0;
}

