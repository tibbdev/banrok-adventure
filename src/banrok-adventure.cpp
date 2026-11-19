#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <cstring>

#include <vector>
#include <array>

constexpr uint16_t WORLD_BUFFER_SIZE = 16 * 1024;

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
    uint16_t coin   = 0;
    uint16_t potion = 0;
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
    uint8_t  tiles[WORLD_BUFFER_SIZE] = {0};
    uint16_t walls[WORLD_BUFFER_SIZE] = {0};
    char     raw[WORLD_BUFFER_SIZE]   = {0};

    void clear(void)
    {
        for (int idx = 0; idx < WORLD_BUFFER_SIZE; idx++)
        {
            tiles[idx] = 78;  // All blank
            raw[idx]   = '#';
            walls[idx] = 0;
        }

        width  = 0;
        height = 0;
    }

    void process_raw_data(void)
    {
        memset(tiles, 78, sizeof(tiles));
        
        for (uint16_t row = 0; row < height; row++)
        {
            for (uint16_t col = 0; col < width; col++)
            {
                uint16_t idx = row * width + col;                        
                
                walls[idx] = 0u;

                if (0 == row) // Top row always blank - rule that level generator will need to obey
                {
                    tiles[idx] = 78;
                }
                else
                {
                    if ('#' == raw[idx]) // Current tile
                    {
                        walls[idx] |= 0x0020;
                    }

                    if (0 == col) // first column
                    {
                        walls[idx] |= 0x444;
                        if ('#' == raw[idx - width]) // N
                        {
                            walls[idx] |= 0x0200;
                        }
                        if ('#' == raw[idx - width + 1]) // NE
                        {
                            walls[idx] |= 0x0100;
                        }
                        if ('#' == raw[idx + 1]) // E
                        {
                            walls[idx] |= 0x0010;
                        }
                        if (((height - 1) > row))
                        {
                            if ('#' == raw[idx + width]) // S
                            {
                                walls[idx] |= 0x0002;
                            }
                            if ('#' == raw[idx + width + 1]) // SE
                            {
                                walls[idx] |= 0x0001;
                            }
                        }
                    }
                    else if ((width - 1) == col) // last column
                    {
                        walls[idx] |= 0x111;
                        if ('#' == raw[idx - width]) // N
                        {
                            walls[idx] |= 0x0200;
                        }
                        if ('#' == raw[idx - width - 1]) // NW
                        {
                            walls[idx] |= 0x0400;
                        }
                        if ('#' == raw[idx - 1]) // W
                        {
                            walls[idx] |= 0x0040;
                        }
                        if (((height - 1) > row))
                        {
                            if ('#' == raw[idx + width]) // S
                            {
                                walls[idx] |= 0x0002;
                            }
                            if ('#' == raw[idx + width - 1]) // SW
                            {
                                walls[idx] |= 0x0004;
                            }
                        }
                    }
                    else // everywhere else
                    {
                        if ('#' == raw[idx - width - 1]) // NW
                        {
                            walls[idx] |= 0x0400;
                        }
                        if ('#' == raw[idx - width]) // N
                        {
                            walls[idx] |= 0x0200;
                        }
                        if ('#' == raw[idx - width + 1]) // NE
                        {
                            walls[idx] |= 0x0100;
                        }
                        if ('#' == raw[idx - 1]) // W
                        {
                            walls[idx] |= 0x0040;
                        }
                        if ('#' == raw[idx + 1]) // E
                        {
                            walls[idx] |= 0x0010;
                        }
                        if (((height - 1) > row))
                        {
                            if ('#' == raw[idx + width - 1]) // SW
                            {
                                walls[idx] |= 0x0004;
                            }
                            if ('#' == raw[idx + width]) // S
                            {
                                walls[idx] |= 0x0002;
                            }
                            if ('#' == raw[idx + width + 1]) // SE
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
                        case 0x766:
                        case 0x767:
                        case 0x740:
                        case 0x760:
                            tiles[idx] = 0;
                            break; 
                        case 0x660: // Left
                        case 0x661:
                        case 0x662:
                        case 0x663:
                        case 0x664:
                        case 0x665:
                        case 0x666:
                        case 0x667:
                        case 0x764:
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
                        case 0x371:
                        case 0x372:
                        case 0x373:
                        case 0x374:
                        case 0x375:
                        case 0x376:
                        case 0x377:
                            tiles[idx] = 45;
                            break;
                        case 0x555: // Right
                        case 0x330:
                        case 0x331:
                        case 0x332:
                        case 0x333:
                        case 0x334:
                        case 0x335:
                        case 0x336:
                        case 0x337:
                        case 0x737:
                        case 0x517:
                            tiles[idx] = 15 + 10 * (idx % 2);
                            break;
                        case 0x757: // Top Right
                        case 0x756:
                        case 0x754:
                        case 0x751:
                        case 0x750:
                        case 0x763:
                        case 0x730:
                        case 0x733:
                        case 0x731:
                            tiles[idx] = 5;
                            break;
                        case 0x033:
                        case 0x030:
                        case 0x133:
                        case 0x130:
                        case 0x131:
                        case 0x136:
                        case 0x137:
                            tiles[idx] = 54;
                            break;
                        case 0x037:
                            tiles[idx] = 50;
                            break;
                        case 0x067:
                        case 0x467:
                        case 0x466:
                            tiles[idx] = 53;
                            break;
                        case 0x460:
                        case 0x464:
                        case 0x060:
                        case 0x064:
                        case 0x066:
                            tiles[idx] = 55;
                            break;
                        case 0x770:
                        case 0x771:
                        case 0x772:
                        case 0x773:
                        case 0x774:
                        case 0x775:
                        case 0x776:
                        case 0x777:
                            tiles[idx] = 78;
                            break;
                        
                        default:
                            tiles[idx] = 69;
                            break;
                        }
                    }
                    else
                    {
                        switch (walls[idx])
                        {
                            case 0x740:
                            case 0x741:
                            case 0x742:
                            case 0x743:
                            case 0x744:
                            case 0x745:
                            case 0x746:
                            case 0x747:
                            case 0x300:
                            case 0x313:
                                tiles[idx] = 1;
                                break;
                            case 0x710:
                            case 0x711:
                            case 0x712:
                            case 0x713:
                            case 0x714:
                            case 0x715:
                            case 0x716:
                            case 0x717:
                            case 0x701:
                            case 0x310:
                            case 0x755:
                                tiles[idx] = 4;
                                break;
                            case 0x702:
                            case 0x703:
                            case 0x704:
                            case 0x705:
                            case 0x706:
                            case 0x311:
                            case 0x317:
                            case 0x301:
                            case 0x302:
                            case 0x303:
                            case 0x304:
                            case 0x305:
                            case 0x306:
                            case 0x307:
                            case 0x201:
                            case 0x202:
                            case 0x203:
                            case 0x204:
                            case 0x205:
                            case 0x206:
                            case 0x207:
                            case 0x600:
                                tiles[idx] = 3;
                                break;
                            case 0x707:
                            case 0x601:
                            case 0x602:
                            case 0x603:
                            case 0x604:
                            case 0x605:
                            case 0x606:
                            case 0x607:
                            case 0x644:
                            case 0x645:
                            case 0x647:
                            case 0x320:
                            case 0x700:
                            case 0x750:
                                tiles[idx] = 2;
                                break;
                            case 0x200:
                            case 0x500:
                                tiles[idx] = 22;
                                break;
                            case 0x501:
                            case 0x504:
                                tiles[idx] = 7;
                                break;
                            case 0x502:
                            case 0x503:
                                tiles[idx] = 8;
                                break;
                            case 0x505:
                            case 0x506:
                            case 0x507:
                                tiles[idx] = 27;
                                break;

                            default:
                                tiles[idx] = 79;
                                break;
                        }
                    }
                }
            }
        }
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
                    }
                }
                process_raw_data();
            }
        }
    }

    void generate(uint16_t lvl_width, uint16_t lvl_height)
    {
        memset(raw, '#', sizeof(raw[0] * WORLD_BUFFER_SIZE));

        if (sizeof(tiles) > (lvl_width * lvl_height))
        {
            width  = lvl_width;
            height = lvl_height;

            printf("generating level!\r\n");
            
            for (uint8_t round = 0; round < 3; round++)
            {
                printf("Round %d!\r\n", round + 1);

                if (0 == round)
                {
                    uint16_t chance = rand() % 5;

                    for (uint16_t row = 0; lvl_height > row ; row++)
                    {
                        for (uint16_t col = 0; lvl_width > col ; col++)
                        {
                            uint16_t idx = row * lvl_width + col;
                            
                            raw[idx] = '#';
                            
                            chance = rand() % 1000;
                            if (500 <= chance)
                            {
                                raw[idx] = '.';
                            }
                            printf("%c", raw[idx]);
                        }
                        printf("\r\n");
                    }
                }
                else
                {
                    for (uint16_t row = 0; lvl_height > row ; row++)
                    {
                        for (uint16_t col = 0; lvl_width > col ; col++)
                        {
                            uint16_t idx = row * lvl_width + col;
                            if ( (row == 0) || (row == (lvl_height - 1)) ||
                                 (col == 0) || (col == (lvl_width - 1)) )
                            {
                                raw[idx] = '#';
                            }
                            else
                            {
                                uint8_t neighbouring_wall_cnt = 0;

                                // printf("idx %d [%c]", idx, raw[idx]);

                                for (int16_t y = -1; 1 >= y; y++ )
                                {
                                    for (int16_t x = -1; 1 >= x; x++ )
                                    {
                                        uint16_t test_idx = idx + (y * lvl_width + x);
                                        
                                        // printf(" test-idx:=%d [%c]", test_idx, raw[test_idx]);
                                        if (idx != test_idx)
                                        {
                                            if (raw[test_idx] == '#')
                                            {
                                                neighbouring_wall_cnt++;
                                            }
                                        }                                        
                                    }
                                }

                                // printf(" %d\n", neighbouring_wall_cnt);

                                if (raw[idx] == '#')
                                {
                                    if (2 > neighbouring_wall_cnt)
                                    {
                                        raw[idx] = '.';
                                    }
                                }
                                else
                                {
                                    if (5 > neighbouring_wall_cnt)
                                    {
                                        raw[idx] = '#';
                                    }
                                }
                            }
                        }
                    }
                }
            }

            printf("generation complete... finding starting point!\r\n");

            uint32_t start_x = 0, start_y = 0, exit_x = 100, exit_y = 100;

            do
            {
                start_x = (rand() % lvl_width) / 2;
                start_y = (rand() % lvl_height) / 2;
            } while (raw[start_y * lvl_width + start_x] == '#');

            raw[start_y * lvl_width + start_x] = '@';
            raw[start_y * lvl_width + start_x + 1] = '.';
            raw[start_y * lvl_width + start_x + 2] = '.';
            raw[start_y * lvl_width + start_x + 3] = '.';
            raw[start_y * lvl_width + start_x + 4] = '.';
            raw[(start_y + 1) * lvl_width + start_x + 4] = '.';
            raw[(start_y + 2) * lvl_width + start_x + 4] = '.';
            raw[(start_y + 3) * lvl_width + start_x + 4] = '.';
            raw[(start_y + 4) * lvl_width + start_x + 4] = '.';

            printf("generation complete... finding exit point!\r\n");

            do
            {
                exit_x = lvl_width / 3;
                exit_y = lvl_height / 3;
                exit_x += (rand() % lvl_width) / 2;
                exit_y += (rand() % lvl_height) / 2;
            } while (raw[exit_y * lvl_width + exit_x] == '#');

            raw[exit_y * lvl_width + exit_x] = 'X';
            
            for (uint16_t row = 0; lvl_height > row ; row++)
            {
                for (uint16_t col = 0; lvl_width > col ; col++)
                {
                    uint16_t idx = row * lvl_width + col;

                    printf("%c", raw[idx]);                        
                }
                
                printf("\r\n");
            }

            process_raw_data();
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
                "##C########################..........####E.....#############"
                "#...#####........................................###########"
                "#....####..#################.....................###########"
                "#..........##################..................#############"
                "#.@........################.....................C###########"
                "#..........################..E...................###########"
                "#####......##################..................####........#"
                "#####..........############....................####.######.#"
                "##############...............................######.######.#"
                "##############.#############.................###....#####..#"
                "#######............###########...................########.##"
                "#####E.............#######.......................#######..##"
                "######.............#####......##................########.###"
                "#####..............#####.##.C.#####...........####.C.##...##"
                "#####C.............#####.############..###########...##.####"
                "#####....................############..########.........##C#"
                "########...........##################..######...........##.#"
                "######...............####..........##..######..............#"
                "#####.C....#########.........####......######E.............#"
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
                "####.##.#####.............####..................E###########"
                "####.##.#####.............####................##############"
                "####.##.#####..............................#################"
                "####....#######...E....##########..C..######################"
                "############################################################";

char hiddn[] =  "                                                            "
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

void handle_player_movement(Player &player, std::vector<Enemy> &enemies, Level &level, uint16_t world_width)
{
    uint16_t wall = level.walls[player.x + player.y * level.width];

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
    {
        if (!(0x200 & wall)) // if it's not a wall you might be able to move
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

    wall = level.walls[player.x + player.y * level.width];

    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
    {
        if (!(0x002 & wall))
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

    wall = level.walls[player.x + player.y * level.width];
    
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
    {
        if (!(0x040 & wall))
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

    wall = level.walls[player.x + player.y * level.width];

    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
    {
        if (!(0x010 & wall))
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
        return;
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
    uint32_t score   = 0;
    bool dbg_overlay = true;
    bool dbg_nbor    = false;

    Player             player;
    std::vector<Chest> chests;
    std::vector<Enemy> enemies;
    Level              level;

    // level.load(world, 60, 37);

    InitWindow(screenWidth, screenHeight, "Banroks Adventure");

    Texture2D tex_tileset;
    Texture2D tex_characters;

    {
        Image img = LoadImage(ASSETS_PATH "2DPixelDungeon/character and tileset/Dungeon_Tileset.png");
        tex_tileset = LoadTextureFromImage(img);
        UnloadImage(img);
        img = LoadImage(ASSETS_PATH "2DPixelDungeon/character and tileset/Dungeon_Character_2.png");
        tex_characters = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    ToggleFullscreen();

    float window_scale = (float)2.0;
    
    SetTargetFPS(30);               // Set our game to run at 30 frames-per-second for that retro lifestyle

    uint16_t world_width  = (GetScreenWidth() / window_scale) / (tile_size);
    uint16_t world_height = (GetScreenHeight() / window_scale) / (tile_size);

    level.generate(world_width, world_height);

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
        if('X' == level.raw[player.x + level.width * player.y]) // Is player at exit?
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
        if (IsKeyPressed(KEY_N)) // Toggle debug info on/off
        {
            dbg_overlay = !dbg_overlay;
        }
        if (IsKeyPressed(KEY_M)) // Toggle neighbour info on/off
        {
            dbg_nbor = !dbg_nbor;
        }

        // Update
        if (start_processed)
        {
            handle_player_movement(player, enemies, level, level.width);

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

            if ((0 >= y) | (level.height < y))
            {
                continue;
            }
            
            for (int16_t x_off = -6; x_off <= 6; x_off++)
            {
                int32_t x = player.x + x_off;

                if ((0 > x) | (level.width < x))
                {
                    continue;
                }

                int16_t distance = (x_off * x_off) + (y_off * y_off);

                if (32 > distance)
                {
                    hiddn[player.x + x_off + (player.y + y_off)*level.width] = ' ';
                }
            }
        }
        
        // Draw
        BeginDrawing();
        {
            ClearBackground(colours[0]);

            for (size_t y = 0; y < level.height; y++)
            {
                // printf("row #%03d\n", y);
                for (size_t x = 0; x < level.width; x++)
                {
                    char raw = level.raw[x + y * level.width];
                    
                    // printf("tile-id = %d, WALL:=0x%03x\n", level.tiles[x + y * level.width], level.walls[x + y * level.width]);

                    draw_tile_from_set(tex_tileset, level.tiles[x + y * level.width], tile_size, x, y, 2.0f);
                    if ('X' == raw) // exit
                    {
                        draw_tile_from_set(tex_tileset, player.hasKey ? 39 : 38, tile_size, x, y, 2.0f);
                    }

                    if (dbg_nbor)
                    {
                        char dbg_nbor_str[8] = "";
                        sprintf(dbg_nbor_str, "%03x", level.walls[x + y * level.width]);
                        DrawText(dbg_nbor_str, x * tile_size * window_scale, y * tile_size * window_scale, 8, GRAY);
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
                        draw_tile_from_set(tex_tileset, chest.containsKey ? 84 : 85, tile_size, chest.x, chest.y, 2.0f);
                    }
                }

                uint8_t enemy_idx = 0;
                for(Enemy &enemy : enemies)
                {
                    if (enemy.isAlive)
                    {
                        draw_tile_from_set(tex_characters, 9, tile_size, enemy.x, enemy.y, 2.0f);
                    }
                    else
                    {
                        draw_tile_from_set(tex_tileset, 68, tile_size, enemy.x, enemy.y, 2.0f);
                    }
                    
                    enemy_idx++;
                }

                draw_tile_from_set(tex_characters, player.hasKey ? 2 : 1, tile_size, player.x, player.y, 2.0f);
                // DrawRectangle(player.x * tile_size * window_scale, player.y * tile_size * window_scale, tile_size * window_scale, tile_size * window_scale, player.hasKey ? colours[7] : RED);
                if (player.hasKey)
                { 
                    draw_tile_from_set(tex_tileset, 99, tile_size, (world_width / window_scale) - 1, 0, 2.0f);
                }
            }
            
            if (fog_of_war)
            {
                for (size_t y = 0; y < level.height; y++)
                {
                    for (size_t x = 0; x < level.width; x++)
                    {
                        char tile = hiddn[x + y * level.width];
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

            if (dbg_overlay)
            {
                char dbg_overlay_str[21] = "";
                sprintf(dbg_overlay_str, "player x.y := {%d,%d} walls:0x%03x - %u", player.x, player.y, level.walls[player.x + player.y * level.width], player.x + player.y * level.width);
                DrawText(dbg_overlay_str, 400 * window_scale, 2 * window_scale, 12 * window_scale, GREEN);
            }
        }
        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context
    return 0;
}
