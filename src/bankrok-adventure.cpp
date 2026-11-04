#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <vector>

const int screenWidth = 800;
const int screenHeight = 480;

struct Player
{
    uint16_t x      = 0;
    uint16_t y      = 0;
    uint16_t health = 100;
    bool     alive  = true;
    bool     hasKey = false;
} player;

struct Chest
{
    uint16_t x           = 0;
    uint16_t y           = 0;
    bool     containsKey = true;
    bool     opened      = false;
};

struct Enemy
{
    uint16_t x       = 0;
    uint16_t y       = 0;
    bool     isAlive = true;
};

std::vector<Chest> chests;
std::vector<Enemy> enemies;

uint32_t score = 0;

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
                    "##X.....E......#########......................####"
                    "##.............#########......................####"
                    "##.............#########..E...................####"
                    "##################################################";

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Banroks Adventure");

    SetTargetFPS(30);               // Set our game to run at 30 frames-per-second for that retro lifestyle

    uint16_t world_width  = screenWidth / tile_size;
    uint16_t world_height = screenHeight / tile_size;

    bool start_processed = false;
    bool running         = true;

    bool key_assigned = false;
    
    uint64_t last_attack_attempt = (uint64_t)GetTime();

    while (running && !WindowShouldClose())    // Detect window close button or ESC key
    {
        if('X' == world[player.x + world_width * player.y])
        {
            if (player.hasKey)
            {
                running = false;
                continue;
            }
        }

        // Update
        if (start_processed)
        {
            int enemy_within_range = enemies.size();

            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
            {
                uint16_t check_idx = (player.y - 1) * world_width + player.x;

                if ('#' != world[check_idx]) // if it's not a wall you might be able to move
                {
                    bool enemy_blocking = false;

                    for (Enemy enemy : enemies) // double check for enemies in this location
                    {
                        if (((player.y - 1) == enemy.y) && (enemy.isAlive) && (player.x == enemy.x))
                        {
                            enemy_blocking = true;

                            break;
                        }
                    }

                    if (!enemy_blocking)
                    {
                        player.y--;
                    }
                }
            }
            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
            {
                if ('#' != world[(player.y + 1) * world_width + player.x])
                {
                    bool enemy_blocking = false;

                    for (Enemy enemy : enemies) // double check for enemies in this location
                    {
                        if (((player.y + 1) == enemy.y) && (enemy.isAlive) && (player.x == enemy.x))
                        {
                            enemy_blocking = true;

                            break;
                        }
                    }

                    if (!enemy_blocking)
                    {
                        player.y++;
                    }
                }
            }
            
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
            {
                if ('#' != world[player.y * world_width + (player.x - 1)])
                {
                    bool enemy_blocking = false;
                    uint16_t enemy_idx = 0;

                    for (Enemy enemy : enemies) // double check for enemies in this location
                    {
                        enemy_idx++;
                        if (((player.x - 1) == enemy.x) && (enemy.isAlive) && (player.y == enemy.y))
                        {
                            enemy_blocking = true;
                            break;
                        }
                    }

                    if (!enemy_blocking)
                    {
                        player.x--;
                    }
                }
            }
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
            {
                if ('#' != world[player.y * world_width + (player.x + 1)])
                {
                    bool enemy_blocking = false;

                    for (Enemy enemy : enemies) // double check for enemies in this location
                    {
                        if (((player.x + 1) == enemy.x) && (enemy.isAlive) && (player.y == enemy.y))
                        {
                            enemy_blocking = true;
                            break;
                        }
                    }

                    if (!enemy_blocking)
                    {
                        player.x++;
                    }
                }
            }
            
            uint16_t enemy_idx = 0;

            for (Enemy &enemy : enemies) // double check for enemies in this location
            {
                if (((((player.x + 1) == enemy.x) || ((player.x - 1) == enemy.x)) && (enemy.isAlive) && (player.y == enemy.y)) ||
                    ((((player.y + 1) == enemy.y) || ((player.y - 1) == enemy.y)) && (enemy.isAlive) && (player.x == enemy.x)))
                {
                    enemy_within_range = enemy_idx;
                    break;
                }
                enemy_idx++;
            }
        
            if ((enemy_within_range != enemies.size()) && (0 != enemies.size()))
            {
                if (0 == rand() % 15)
                {
                    if (last_attack_attempt != (uint64_t)GetTime())
                    {
                        player.health -= 10;
                    }
                }

                if (IsKeyPressed(KEY_SPACE))
                {
                    enemies[enemy_within_range].isAlive = false;
                    score += 100;
                }
            }

            for (Chest &chest : chests)
            {
                if ((chest.x == player.x) && (chest.y == player.y))
                {
                    if (IsKeyPressed(KEY_SPACE) && !chest.opened)
                    {
                        chest.opened = true;

                        player.hasKey |= chest.containsKey;
                        score += chest.containsKey ? 60 : 50;
                    }
                }
            }
        }

        // Draw
        BeginDrawing();

            ClearBackground(colours[0]);

            for (size_t y = 0; y < world_height; y++)
            {
                for (size_t x = 0; x < world_width; x++)
                {
                    char tile = world[x + y * world_width];
                    if ('#' == tile) // wall
                    {
                        DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, colours[0]);
                    }
                    else if ('X' == tile) // exit
                    {
                        DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, colours[5]);
                    }
                    else // floor
                    {
                        DrawRectangle(x * tile_size, y * tile_size, tile_size, tile_size, colours[3]);
                    }
                    
                    if (!start_processed)
                    {
                        if ('@' == tile) // player start pos
                        {
                            player.x = x;
                            player.y = y;

                            printf("player at {%u, %u}\r\n", x, y);
                        }
                        else if ('E' == tile) // enemy
                        {
                            Enemy fresh_enemy;
                            fresh_enemy.x = x;
                            fresh_enemy.y = y;
                            
                            printf("enemy at {%u, %u} : ", x, y);

                            enemies.push_back(fresh_enemy);

                            printf("total enemies now %u\r\n", enemies.size());
                        }
                        else if ('C' == tile) // chest
                        {
                            Chest fresh_chest;
                            fresh_chest.x = x;
                            fresh_chest.y = y;

                            if (!key_assigned)
                            {
                                int rng = std::rand();
                                if (rng % 2)
                                {
                                    fresh_chest.containsKey = true;
                                    key_assigned = true;
                                }
                            }

                            printf("chest at {%u, %u} : ", x, y);

                            chests.push_back(fresh_chest);

                            printf("total chests now %u\r\n", chests.size());
                        }
                    }
                }
            }
            
            start_processed = true;

            if (start_processed)
            {
                for (Chest &chest : chests)
                {
                    if (!key_assigned)
                    {
                        chest.containsKey = true;
                    }

                    if(!chest.opened)
                    {
                        DrawRectangle(chest.x * tile_size, chest.y * tile_size, tile_size, tile_size, colours[9]);
                    }
                }

                uint8_t enemy_idx = 0;
                for(Enemy &enemy : enemies)
                {
                    if (enemy.isAlive)
                    {
                        DrawRectangle(enemy.x * tile_size, enemy.y * tile_size, tile_size, tile_size, colours[8]);
                    }
                    
                    enemy_idx++;
                }

                DrawRectangle(player.x * tile_size, player.y * tile_size, tile_size, tile_size, player.hasKey ? colours[7] : RED);
            }

            char score_str[21] = "";
            sprintf(score_str, "SCORE : %u", score);
            DrawText(score_str, 2, 2, 16, WHITE);
            
            char health_str[21] = "";
            sprintf(health_str, "HEALTH : %u", player.health);
            DrawText(health_str, 200, 2, 16, WHITE);
            

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context
    return 0;
}
