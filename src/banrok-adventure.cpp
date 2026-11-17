#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <cstring>

#include <vector>
#include <array>

const int screenWidth = 800;
const int screenHeight = 480;

const uint16_t MIN_SECTION_SIZE_TILES   = 12;
const uint16_t MIN_ROOM_DIMENSION_TILES = 7;
const uint16_t MIN_ROOM_BORDER_TILES    = 1;

struct Player
{
    uint16_t x      = 0;
    uint16_t y      = 0;
    uint16_t health = 100;
    bool     alive  = true;
    bool     hasKey = false;

    void     reset(void)
    {
        x      = 0;
        y      = 0;
        health = 100;
        alive  = true;
        hasKey = false;
    }
};

struct Chest
{
    uint16_t x           = 0;
    uint16_t y           = 0;
    bool     containsKey = false;
    bool     opened      = false;

    void     reset(void)
    {
        x           = 0;
        y           = 0;
        containsKey = false;
        opened      = false;
    }
};

struct Enemy
{
    uint16_t x       = 0;
    uint16_t y       = 0;
    bool     isAlive = true;

    void     reset(void)
    {
        x       = 0;
        y       = 0;
        isAlive = true;
    }
};

struct Exit
{
    uint16_t x  = 0;
    uint16_t y  = 0;
    bool locked = true;
};

struct Level
{
    uint16_t width       = 0;
    uint16_t height      = 0;
    uint8_t  tiles[4096] = {0};
    uint16_t walls[4096] = {0};
    char     raw[4096]   = {0};

    void clear(void)
    {
        for (int idx = 0; idx < 4096; idx++)
        {
            tiles[idx] = 78;  // All blank
            raw[idx]   = '#';
            walls[idx] = 0;
        }

        width  = 0;
        height = 0;
    }

    void load(char * data, uint16_t lvl_width, uint16_t lvl_height)
    {
        if (data != NULL)
        {
            if (sizeof(tiles) > (lvl_width * lvl_height))
            {
                width  = lvl_width;
                height = lvl_height;

                memset(tiles, 78, sizeof(tiles));
                memset(raw,   0,  sizeof(raw));
                
                for (uint16_t row = 0; row < lvl_height; row++)
                {
                    for (uint16_t col = 0; col < lvl_width; col++)
                    {
                        uint16_t idx = row * lvl_width + col;
                        
                        raw[idx] = data[idx];
                        
                        walls[idx] = 0u;

                        if (0 == row) // Top row always blank - rule that level generator will need to obey
                        {
                            tiles[idx] = 78;
                        }
                        else
                        {
                            if ('#' == data[idx]) // Current tile
                            {
                                walls[idx] |= 0x0020;
                            }

                            if (0 == col) // first column
                            {
                                walls[idx] |= 0x444;
                                if ('#' == data[idx - lvl_width]) // N
                                {
                                    walls[idx] |= 0x0200;
                                }
                                if ('#' == data[idx - lvl_width + 1]) // NE
                                {
                                    walls[idx] |= 0x0100;
                                }
                                if ('#' == data[idx + 1]) // E
                                {
                                    walls[idx] |= 0x0010;
                                }
                                if (((lvl_height - 1) > row))
                                {
                                    if ('#' == data[idx + lvl_width]) // S
                                    {
                                        walls[idx] |= 0x0002;
                                    }
                                    if ('#' == data[idx + lvl_width + 1]) // SE
                                    {
                                        walls[idx] |= 0x0001;
                                    }
                                }
                            }
                            else if ((lvl_width - 1) == col) // last column
                            {
                                walls[idx] |= 0x111;
                                if ('#' == data[idx - lvl_width]) // N
                                {
                                    walls[idx] |= 0x0200;
                                }
                                if ('#' == data[idx - lvl_width - 1]) // NW
                                {
                                    walls[idx] |= 0x0400;
                                }
                                if ('#' == data[idx - 1]) // W
                                {
                                    walls[idx] |= 0x0040;
                                }
                                if (((lvl_height - 1) > row))
                                {
                                    if ('#' == data[idx + lvl_width]) // S
                                    {
                                        walls[idx] |= 0x0002;
                                    }
                                    if ('#' == data[idx + lvl_width - 1]) // SW
                                    {
                                        walls[idx] |= 0x0004;
                                    }
                                }
                            }
                            else // everywhere else
                            {
                                if ('#' == data[idx - lvl_width - 1]) // NW
                                {
                                    walls[idx] |= 0x0400;
                                }
                                if ('#' == data[idx - lvl_width]) // N
                                {
                                    walls[idx] |= 0x0200;
                                }
                                if ('#' == data[idx - lvl_width + 1]) // NE
                                {
                                    walls[idx] |= 0x0100;
                                }
                                if ('#' == data[idx - 1]) // W
                                {
                                    walls[idx] |= 0x0040;
                                }
                                if ('#' == data[idx + 1]) // E
                                {
                                    walls[idx] |= 0x0010;
                                }
                                if (((lvl_height - 1) > row))
                                {
                                    if ('#' == data[idx + lvl_width - 1]) // SW
                                    {
                                        walls[idx] |= 0x0004;
                                    }
                                    if ('#' == data[idx + lvl_width]) // S
                                    {
                                        walls[idx] |= 0x0002;
                                    }
                                    if ('#' == data[idx + lvl_width + 1]) // SE
                                    {
                                        walls[idx] |= 0x0001;
                                    }
                                }
                        }

                            if (0x0777 == walls[idx])
                            {
                                tiles[idx] = 78;
                            }
                            else if (walls[idx] & 0x0020)
                            {
                                switch (walls[idx])
                                {
                                case 0x747: // Top Left
                                case 0x744:
                                    tiles[idx] = 0;
                                    break;
                                case 0x666: // Left
                                case 0x667:
                                case 0x760:
                                    tiles[idx] = 10 + 10 * (idx % 2);
                                    break;
                                case 0x670: // Bottom Left
                                case 0x671:
                                case 0x672:
                                case 0x673:
                                case 0x674:
                                case 0x675:
                                case 0x676:
                                case 0x677:
                                    tiles[idx] = 40;
                                    break;
                                case 0x470: // Bottom
                                case 0x471:
                                case 0x472:
                                case 0x473:
                                case 0x474:
                                case 0x475:
                                case 0x476:
                                case 0x477:
                                case 0x170:
                                case 0x171:
                                case 0x172:
                                case 0x173:
                                case 0x174:
                                case 0x175:
                                case 0x176:
                                case 0x177:
                                case 0x070:
                                case 0x071:
                                case 0x072:
                                case 0x073:
                                case 0x074:
                                case 0x075:
                                case 0x076:
                                case 0x077:
                                    tiles[idx] = 41;
                                    break;
                                case 0x570: // Bottom Right
                                case 0x571:
                                case 0x572:
                                case 0x573:
                                case 0x574:
                                case 0x575:
                                case 0x576:
                                case 0x577:
                                case 0x370:
                                    tiles[idx] = 45;
                                    break;
                                case 0x555: // Right
                                case 0x337:
                                case 0x517:
                                    tiles[idx] = 15 + 10 * (idx % 2);
                                    break;
                                case 0x757: // Top Right
                                case 0x756:
                                case 0x754:
                                case 0x751:
                                case 0x750:
                                case 0x763:
                                    tiles[idx] = 0;
                                    break;
                                case 0x770:
                                case 0x771:
                                case 0x774:
                                    tiles[idx] = 78;
                                    break;
                                
                                default:
                                    tiles[idx] = 69;
                                    break;
                                }
                            }
                            else
                            {
                                tiles[idx] = 79;
                            }
                        }
                    }
                }
            }
        }
    }

};

const Color colours[] =
{
    { .r = 0x04, .g = 0x0c, .b = 0x06, .a = 0xff }, // #040c06 0
    { .r = 0x11, .g = 0x23, .b = 0x18, .a = 0xff }, // #112318 1
    { .r = 0x1e, .g = 0x1e, .b = 0x29, .a = 0xff }, // #1e1e29 2
    { .r = 0x30, .g = 0x30, .b = 0x42, .a = 0xff }, // #303042 3
    { .r = 0x4d, .g = 0x4d, .b = 0x61, .a = 0xff }, // #4d4d61 4
    { .r = 0x89, .g = 0x89, .b = 0x57, .a = 0xff }, // #898957 5
    { .r = 0xbe, .g = 0xbe, .b = 0x7f, .a = 0xff }, // #bebe7f 6
    { .r = 0xee, .g = 0xee, .b = 0xcc, .a = 0xff }, // #eeffcc 7
    { .r = 0xe3, .g = 0xe3, .b = 0x4c, .a = 0xff }, // #e3e34c 8
    { .r = 0x45, .g = 0x45, .b = 0xa9, .a = 0xff }, // #4545a9 9
};

const uint16_t tile_size = 16u;
char *world =   "############################################################"
                "#..........################..........####......#############"
                "#................................................###########"
                "#..........#################.....................###########"
                "#..........##################..................#############"
                "#.@........################.....................C###########"
                "#..........################..E...................###########"
                "#..........##################..................####........#"
                "#..............############....................####.######.#"
                "##############.................................####.######.#"
                "##############.#############...................#....#####..#"
                "#######............###########...................########.##"
                "#####..............##########....................#######..##"
                "######.............#############................########.###"
                "#####..............################...........####.C.##...##"
                "#####C.............##################..###########...##..###"
                "#####..............##################..########..........#C#"
                "########...........##################..######............#.#"
                "######...............####..........##..######..............#"
                "#####......####..............####......######E.............#"
                "#####################################..######..............#"
                "#####################################..#########...........#"
                "##.............#################..........#######..........#"
                "##.......E.....#########....................#####..........#"
                "##.............#########.................C....##########.###"
                "##............................................##########.###"
                "##X............#########....................############.###"
                "##.....E.......##########.................######....####.###"
                "##.............###########E..............#######.##.####.###"
                "####.##########################.################.##.####.###"
                "####.#########....C......#####.....#######.......##......###"
                "####.##...................####..................############"
                "####.##.#####.............####...................###########"
                "####.##.#####.............####................##############"
                "####.##.#####..............................#################"
                "####....#######...E....##########..C..######################"
                "############################################################";

char hiddn[] =  "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
                "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH";

void handle_player_movement(Player &player, std::vector<Enemy> &enemies, std::vector<Chest> &chests, uint16_t world_width)
{
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
}

void draw_tile_from_set(Texture2D tex, uint16_t tileid, uint16_t tilesize, uint16_t x_loc, uint16_t y_loc, float scale)
{
    if (IsTextureValid(tex))
    {
        const uint16_t tex_tiles_x = tex.width / tilesize;
        const uint16_t tex_tiles_y = tex.height / tilesize;

        if (tileid < (tex_tiles_x * tex_tiles_y)) // Make sure requested tileid is less that size of the texture
        {
            Rectangle src_rect;

            src_rect.x      = (tileid % tex_tiles_x) * tilesize;
            src_rect.y      = (tileid / tex_tiles_x) *tilesize;
            src_rect.width  = tilesize;
            src_rect.height = tilesize;

            Rectangle dest_rect;

            dest_rect.x      = scale * tile_size * x_loc;
            dest_rect.y      = scale * tile_size * y_loc;
            dest_rect.width  = scale * tilesize;
            dest_rect.height = scale * tilesize;

            DrawTexturePro(tex, src_rect, dest_rect, {0.0f, 0.0f}, 0.0f, WHITE);
        }
    }
}

int main(void)
{
    // Setup
    uint32_t score = 0;

    Player             player;
    std::vector<Chest> chests;
    std::vector<Enemy> enemies;
    Level              level;

    level.load(world, 60, 37);

    InitWindow(screenWidth, screenHeight, "Banroks Adventure");

    Texture2D tex_tileset;

    {
        Image img = LoadImage(ASSETS_PATH "2DPixelDungeon/character and tileset/Dungeon_Tileset.png");
        tex_tileset = LoadTextureFromImage(img);
    }

    ToggleFullscreen();

    float window_scale = (float)2.0;
    
    SetTargetFPS(30);               // Set our game to run at 30 frames-per-second for that retro lifestyle

    uint16_t world_width  = GetScreenWidth() / (tile_size);
    uint16_t world_height = GetScreenHeight() / (tile_size);

    printf("world_width: %d\n", world_width);
    printf("world_height: %d\n", world_height);

    printf("level_width: %d\n", level.width);
    printf("level_height: %d\n", level.height);

    bool start_processed = false;
    bool running         = true;
    bool fog_of_war      = false;
    bool key_assigned    = false;
    
    uint64_t last_attack_attempt = (uint64_t)GetTime();

    // Enter Game Loop
    while (running && !WindowShouldClose())    // Detect window close button or ESC key
    {
        if('X' == world[player.x + world_width * player.y]) // Is player at exit?
        {
            if (player.hasKey)
            {
                running = false;
                continue;
            }
        }

        if (IsKeyPressed(KEY_F)) // Toggle fog of war on/off
        {
            fog_of_war = !fog_of_war;
        }

        // Update
        if (start_processed)
        {
            handle_player_movement(player, enemies, chests, world_width);

            int enemy_within_range = enemies.size();
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

        // process fog of war
        for (int16_t y_off = -6; y_off <= 6; y_off++)
        {
            int32_t y = player.y + y_off;

            if ((0 >= y) | (world_height < y))
            {
                continue;
            }
            
            for (int16_t x_off = -6; x_off <= 6; x_off++)
            {
                int32_t x = player.x + x_off;

                if ((0 >= x) | (world_width < x))
                {
                    continue;
                }

                int16_t distance = (x_off * x_off) + (y_off * y_off);

                if (40 > distance)
                {
                    hiddn[player.x + x_off + (player.y + y_off)*world_width] = ' ';
                }
            }
        }
        
        // Draw
        BeginDrawing();

            ClearBackground(colours[0]);

            for (size_t y = 0; y < level.height; y++)
            {
                for (size_t x = 0; x < level.width; x++)
                {
                    char raw = level.raw[x + y * level.width];
                    
                    printf("tile-id = %d, WALL:=0x%03x\n", level.tiles[x + y * level.width], level.walls[x + y * level.width]);

                    draw_tile_from_set(tex_tileset, level.tiles[x + y * level.width], tile_size, x, y, 2.0f);
                    if ('X' == raw) // exit
                    {
                        draw_tile_from_set(tex_tileset, player.hasKey ? 39 : 38, tile_size, x, y, 2.0f);
                    }
                    
                    if (!start_processed)
                    {
                        if ('@' == raw) // player start pos
                        {
                            player.x = x;
                            player.y = y;

                            printf("player at {%u, %u}\r\n", x, y);
                        }
                        else if ('E' == raw) // enemy
                        {
                            Enemy fresh_enemy;
                            fresh_enemy.x = x;
                            fresh_enemy.y = y;
                            
                            printf("enemy at {%u, %u} : ", x, y);

                            enemies.push_back(fresh_enemy);

                            printf("total enemies now %u\r\n", enemies.size());
                        }
                        else if ('C' == raw) // chest
                        {
                            Chest fresh_chest;
                            fresh_chest.x = x;
                            fresh_chest.y = y;

                            printf("chest at {%u, %u} : ", x, y);

                            chests.push_back(fresh_chest);

                            printf("total chests now %u\r\n", chests.size());
                        }
                    }
                }
            }

            if (!key_assigned)
            {
                key_assigned = true;
                if (0 != chests.size())
                {
                    uint16_t key_idx = std::rand() % chests.size();
                    chests[key_idx].containsKey = true;
                }
            }
            
            start_processed = true;

            if (start_processed)
            {
                for (Chest &chest : chests)
                {
                    if(!chest.opened)
                    {
                        draw_tile_from_set(tex_tileset, 85, tile_size, chest.x, chest.y, 2.0f);
                    }
                }

                uint8_t enemy_idx = 0;
                for(Enemy &enemy : enemies)
                {
                    if (enemy.isAlive)
                    {
                        DrawRectangle(enemy.x * tile_size * window_scale, enemy.y * tile_size * window_scale, tile_size * window_scale, tile_size * window_scale, colours[8]);
                    }
                    
                    enemy_idx++;
                }

                DrawRectangle(player.x * tile_size * window_scale, player.y * tile_size * window_scale, tile_size * window_scale, tile_size * window_scale, player.hasKey ? colours[7] : RED);
            }
            
            if (fog_of_war)
            {
                for (size_t y = 0; y < world_height; y++)
                {
                    for (size_t x = 0; x < world_width; x++)
                    {
                        char tile = hiddn[x + y * world_width];
                        if ('H' == tile) // hidden
                        {
                            DrawRectangle(x * tile_size * window_scale, y * tile_size * window_scale, tile_size * window_scale, tile_size * window_scale, colours[1]);
                        }
                    }
                }
            }

            char score_str[21] = "";
            sprintf(score_str, "SCORE : %u", score);
            DrawText(score_str, 2 * window_scale, 2 * window_scale, 12 * window_scale, colours[7]);
            
            char health_str[21] = "";
            sprintf(health_str, "HEALTH : %u", player.health);
            DrawText(health_str, 200 * window_scale, 2 * window_scale, 12 * window_scale, colours[7]);

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context
    return 0;
}
