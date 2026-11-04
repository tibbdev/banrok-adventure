#include "raylib.h"
#include "stdint.h"
#include "stdbool.h"

const int screenWidth = 800;
const int screenHeight = 480;

const Color colours[] =
{
    { .r = 0x04, .g = 0x0c, .b = 0x06, .a = 0xff }, // #040c06
    { .r = 0x11, .g = 0x23, .b = 0x18, .a = 0xff }, // #112318
    { .r = 0x1e, .g = 0x1e, .b = 0x29, .a = 0xff }, // #1e1e29
    { .r = 0x30, .g = 0x30, .b = 0x42, .a = 0xff }, // #303042
    { .r = 0x4d, .g = 0x4d, .b = 0x61, .a = 0xff }, // #4d4d61
    { .r = 0x89, .g = 0x89, .b = 0x57, .a = 0xff }, // #898957
    { .r = 0xbe, .g = 0xbe, .b = 0x7f, .a = 0xff }, // #bebe7f
    { .r = 0xee, .g = 0xee, .b = 0xcc, .a = 0xff }, // #eeffcc
    { .r = 0xe3, .g = 0xe3, .b = 0x4c, .a = 0xff }, // #e3e34c
    { .r = 0x45, .g = 0x45, .b = 0xa9, .a = 0xff }, // #4545a9
};

const uint16_t tile_size = 16u;
const char *world = "##################################################"
                    "#..........################......................#"
                    "#................................................#"
                    "#..........################......................#"
                    "#.....@....################......................#"
                    "#..........################.....................C#"
                    "#..........################..E...................#"
                    "#..........################......................#"
                    "#..............############......................#"
                    "##############...................................#"
                    "##############.############......................#"
                    "#####..............########......................#"
                    "#####..............########......................#"
                    "#####..............########......................#"
                    "#####..............########......................#"
                    "#####C.............###################.###########"
                    "#####..............###################.###########"
                    "#####..............###################.###########"
                    "#####..................................###########"
                    "#####..............###################.###########"
                    "######################################.###########"
                    "######################################.###########"
                    "##.............#########......................####"
                    "##......E......#########......................####"
                    "##.............#########.................C....####"
                    "##............................................####"
                    "##C.....E......#########......................####"
                    "##.............#########......................####"
                    "##.............#########..E...................####"
                    "##################################################";

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Banroks Adventure");

    SetTargetFPS(30);               // Set our game to run at 30 frames-per-second for that retro lifestyle

    uint16_t world_width  = screenWidth / tile_size;
    uint16_t world_height = screenHeight / tile_size;

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        // Draw
        BeginDrawing();

            ClearBackground(colours[0]);

            for (size_t y = 0; y < world_height; y++)
            {
                for (size_t x = 0; x < world_width; x++)
                {
                    char tile = world[x + y * world_width];
                    if ('#' == tile)
                    {
                        DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, colours[0]);
                    }
                    else if ('.' == tile)
                    {
                        DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, colours[3]);
                    }
                    else if ('E' == tile)
                    {
                        DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, colours[8]);
                    }
                    else if ('C' == tile)
                    {
                        DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, colours[9]);
                    }
                    else if ('@' == tile)
                    {
                        DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, colours[6]);
                    }
                }
            }
            
            // DrawText("Congrats! You created your first window!", 190, 200, 20, colours[7]);

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context
    return 0;
}
