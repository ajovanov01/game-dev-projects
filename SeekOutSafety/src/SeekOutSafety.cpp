/*Andrija Jovanov
  A Nintendo DS game meant to introduce me to embedded systems development.
  It is a very small, basic dungeon crawler.*/

#include <nds.h>
#include <stdio.h>
#include <time.h>

static const int DMA_CHANNEL = 3;
static const int UP = 0;
static const int DOWN = 1;
static const int LEFT = 2;
static const int RIGHT = 3;

//The sprite struct
typedef struct{

    int x;
    int y;
    int dx;
    int dy;
    bool alive;
    u16 * image;
    SpriteColorFormat colFormat;
    SpriteSize size;
    int hp;
    int facing;
    int score;
    int idNum;

}gameSprite;

gameSprite player;
gameSprite mobs[50];
gameSprite items[50];

OamState *oam = &oamMain;

const int width = 256;                            //Width of the world
const int height = 204;                           //Height of the world
const int relativeWidth = 32;                     //Width of the map
const int relativeHeight = 24;                    //Height of the map
const int tileWidth = 8;                          //Width of each tile (as well as height, since they are square)
int grid[relativeWidth][relativeHeight];          //Grid for unit detection

/*A 32x24 map*/
u16 map[] = {

    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,

};

//Coloured 8x8 tiles
u8 tiles[] = {

    //01 = black
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,

    //00 = blue
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

};

void videoSetup();
int BGSetup();
void makePath(int x, int y);
int * findUnvisited(int unvisited[], int x, int y);
void newSprite(gameSprite * sprite, int x, int y, int dx, int dy, SpriteSize size, SpriteColorFormat format, int hp, int idNum, u8 colIndex);
int initMobs(int tileWidth, SpriteSize size, SpriteColorFormat colFormat);
int initItems(int tileWidth, SpriteSize size, SpriteColorFormat colFormat);
void findItem(int itemX, int itemY, int numItems);
void moveMobs(int numMobs);
void fightMob(int index, int mobX, int mobY, int numMobs);
void update(int numMobs, int numItems);

int main(){

    int bg = 0;
    int x, y;
    int i, j;
    int keys = 0;
    int playerX;
    int playerY;
    int playeHP = 20;
    bool finished = false;                      //General purpose boolean
    bool gameover = false;

    SpriteSize size = SpriteSize_8x8;                           //Every sprite will be the same size as the tiles
    SpriteColorFormat colFormat = SpriteColorFormat_256Color;
    int spriteX = 0;
    int spriteY = 0;

    int numMobs = 0;
    int numItems = 0;

	powerOn(POWER_ALL_2D);                      //Turn on all 2D hardware
	videoSetup();                               //Set vram banks and video modes
	bg = BGSetup();                             //Set up background
    oamInit(oam, SpriteMapping_1D_128, false);  //Initialize OAM for drawing sprites
    consoleDemoInit();                          //Allow printing to the lower screen

	//Set background colour values
    BG_PALETTE[0] = RGB15(0, 0, 0);       //Black
    BG_PALETTE[1] = RGB15(199, 97, 20);   //Blue

    //Set sprite colour values
    SPRITE_PALETTE[1] = RGB15(150, 0, 0);       //Red
    SPRITE_PALETTE_SUB[1] = RGB15(150, 0, 0);

    SPRITE_PALETTE[2] = RGB15(75, 75, 15);       //Grey
    SPRITE_PALETTE_SUB[2] = RGB15(75, 75, 15);

    SPRITE_PALETTE[3] = RGB15(0, 50, 0);       //Green
    SPRITE_PALETTE_SUB[3] = RGB15(0, 50, 0);

    //Maze generator
    srand(time(NULL));
    x = rand() % relativeWidth;
    y = rand() % relativeHeight;
    map[x + y * relativeWidth] = 01;
    makePath(x, y);
    
    //Copy tiles into memory
    dmaCopy(tiles, bgGetGfxPtr(bg), sizeof(tiles));
    
    //Points to map in hardware
    u16* m = bgGetMapPtr(bg);

    //Copy the map into hardware and initialize grid
    for(i = 0; i < relativeWidth; i++)
        for(j = 0; j < relativeHeight; j++){

            m[i + j * relativeWidth] = map[i + j * relativeWidth];
            grid[i][j] = 0;

        }

    for(i = 0; i < relativeWidth; i++)
        for(j = 0; j < relativeHeight; j++)
            grid[i][j] = 0;

    //Create the player sprite
    while(!finished){

        spriteX = rand() % relativeWidth;
        spriteY = rand() % relativeHeight;
        if(map[spriteX + spriteY * relativeWidth] == 01){

            finished = true;
            grid[spriteX][spriteY] = 1;

        }

    }
    newSprite(&player, spriteX * tileWidth, spriteY * tileWidth, 8, 8, size, colFormat, playeHP, 1, 1);

    //Create the mobs and items
    numMobs = initMobs(tileWidth, size, colFormat);
    numItems = initItems(tileWidth, size, colFormat);

    while(1){

		//This is similar in functionality to a sleep call
        for(i = 0; i < 10; i++)
            swiWaitForVBlank();

        if(!gameover){

    		scanKeys();
            keys = keysHeld();
            playerX = player.x / tileWidth;
            playerY = player.y / tileWidth;

            //Move the player character
            if(keys & KEY_UP && map[playerX + (playerY - 1) * relativeWidth] != 00){

                if(grid[playerX][playerY - 1] != 2){

                    if(grid[playerX][playerY - 1] == 3)
                        findItem(playerX, playerY - 1, numItems);

                    player.y = player.y - player.dy;
                    grid[playerX][playerY] = 0;
                    grid[playerX][playerY - 1] = player.idNum;

                }else{

                    fightMob(-1, playerX, playerY - 1, numMobs);

                }

            }

            if(keys & KEY_DOWN && map[playerX + (playerY + 1) * relativeWidth] != 00){

                if(grid[playerX][playerY + 1] != 2){

                    if(grid[playerX][playerY + 1] == 3)
                        findItem(playerX, playerY + 1, numItems);

                    player.y = player.y + player.dy;
                    grid[playerX][playerY] = 0;
                    grid[playerX][playerY + 1] = player.idNum;

                }else{

                    fightMob(-1, playerX, playerY + 1, numMobs);

                }

            }

            if(keys & KEY_LEFT && map[(playerX - 1) + playerY * relativeWidth] != 00){

                if(grid[playerX - 1][playerY] != 2){

                    if(grid[playerX - 1][playerY] == 3)
                        findItem(playerX - 1, playerY, numItems);

                    player.x = player.x - player.dx;
                    grid[playerX][playerY] = 0;
                    grid[playerX - 1][playerY] = player.idNum;

                }else{

                    fightMob(-1, playerX - 1, playerY, numMobs);

                }

            }

            if(keys & KEY_RIGHT && map[(playerX + 1) + playerY * relativeWidth] != 00){

                if(grid[playerX + 1][playerY] != 2){

                    if(grid[playerX + 1][playerY] == 3)
                        findItem(playerX + 1, playerY, numItems);

                    player.x = player.x + player.dx;
                    grid[playerX][playerY] = 0;
                    grid[playerX + 1][playerY] = player.idNum;

                }else{

                    fightMob(-1, playerX + 1, playerY, numMobs);

                }

            }

            moveMobs(numMobs);

            //Update the background and all of the sprites
            bgUpdate();
            update(numMobs, numItems);
            oamUpdate(oam);

        }

        consoleClear();
        printf("Hit Points: %d\n", player.hp);
        printf("Score: %d\n", player.score);

        if(player.hp <= 0){

            printf("Game Over");
            gameover = true;

        }

        if(player.score == numItems * 50){

            printf("You Win!");
            gameover = true;

        }

	}

	return 0;

}

//Set vram banks and video modes
void videoSetup(){

	/*Set the VRAM banks:
	  A - Main screen background memory
	  B - Main screen sprite memory
	  C - Sub screen background memory
	  D - Sub screen sprite memory
	*/
	vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000,
                        VRAM_B_MAIN_SPRITE_0x06400000,
                        VRAM_C_SUB_BG_0x06200000,
                        VRAM_D_SUB_SPRITE);

	//Set main screen and sub screen video modes.
	videoSetMode(MODE_5_2D | DISPLAY_BG0_ACTIVE);
	videoSetModeSub(MODE_5_2D);

}

//Set up background
int BGSetup(){

	//initialize a 256x256 background.
    return bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);

}

//Generate the maze using depth-first search
void makePath(int x, int y){

    int unvisitedA[5];
    int * unvisited;
    int randNum;

    while(1){

        unvisited = findUnvisited(unvisitedA, x, y);

        if(unvisited[4] != 0){

            do{

                randNum = rand() % 4;

            }while(unvisited[randNum] == 0);

            if(randNum == 0){

                x = x + 1;

            }else if(randNum == 1){

                x = x - 1;

            }else if(randNum == 2){

                y = y + 1;

            }else{

                y = y - 1;

            }

            map[x + y * relativeWidth] = 01;
            makePath(x, y);

        }else{

            break;

        }

    }

}

//Find the directions the maze can currently branch out to without colliding with itself
int * findUnvisited(int unvisited[], int x, int y){

    int i = 0;

    for(i = 0; i < 5; i++)
        unvisited[i] = 0;

    if(map[(x + 2) + y * relativeWidth] != 01 && map[(x + 3) + y * relativeWidth] != 01 && (x + 2) <= (relativeWidth - 1) &&
       map[(x + 1) + (y + 1) * relativeWidth] != 01 && map[(x + 1) + (y - 1) * relativeWidth] != 01 &&
       map[(x + 1) + (y + 2) * relativeWidth] != 01 && map[(x + 1) + (y - 2) * relativeWidth] != 01 &&
       map[(x + 2) + (y + 1) * relativeWidth] != 01 && map[(x + 2) + (y - 1) * relativeWidth] != 01){

        unvisited[0] = 1;
        unvisited[4] = 1;

    }

    if(map[(x - 2) + y * relativeWidth] != 01 && map[(x - 3) + y * relativeWidth] != 01 && (x - 2) >= 0 &&
       map[(x - 1) + (y + 1) * relativeWidth] != 01 && map[(x - 1) + (y - 1) * relativeWidth] != 01 &&
       map[(x - 1) + (y + 2) * relativeWidth] != 01 && map[(x - 1) + (y - 2) * relativeWidth] != 01 &&
       map[(x - 2) + (y + 1) * relativeWidth] != 01 && map[(x - 2) + (y - 1) * relativeWidth] != 01){

        unvisited[1] = 1;
        unvisited[4] = 1;

    }

    if(map[x + (y + 2) * relativeWidth] != 01 && map[x + (y + 3) * relativeWidth] != 01 && (y + 2) <= (relativeHeight - 1) &&
       map[(x + 1) + (y + 1) * relativeWidth] != 01 && map[(x - 1) + (y + 1) * relativeWidth] != 01 &&
       map[(x + 2) + (y + 1) * relativeWidth] != 01 && map[(x - 2) + (y + 1) * relativeWidth] != 01 &&
       map[(x + 1) + (y + 2) * relativeWidth] != 01 && map[(x - 1) + (y + 2) * relativeWidth] != 01){

        unvisited[2] = 1;
        unvisited[4] = 1;

    }

    if(map[x + (y - 2) * relativeWidth] != 01 && map[x + (y - 3) * relativeWidth] != 01 && (y - 2) >= 0 &&
       map[(x + 1) + (y - 1) * relativeWidth] != 01 && map[(x - 1) + (y - 1) * relativeWidth] != 01 &&
       map[(x + 2) + (y - 1) * relativeWidth] != 01 && map[(x - 2) + (y - 1) * relativeWidth] != 01 &&
       map[(x + 1) + (y - 2) * relativeWidth] != 01 && map[(x - 1) + (y - 2) * relativeWidth] != 01){

        unvisited[3] = 1;
        unvisited[4] = 1;

    }

    return unvisited;

}

//Create a new sprite
void newSprite(gameSprite * sprite, int x, int y, int dx, int dy, SpriteSize size, SpriteColorFormat colFormat, int hp, int idNum, u8 colIndex){

    //Set the sprite's colour
    u16 color = colIndex | (colIndex << 8);

    sprite->alive = true;
    sprite->x = x;
    sprite->y = y;
    sprite->dx = dx;
    sprite->dy = dy;
    sprite->size = size;
    sprite->colFormat = colFormat;
    
    //Allocate graphics memory for the sprite
    sprite->image = oamAllocateGfx(oam, size, colFormat);
    
    //Load the graphics if memory was successfully allocated
    if(sprite->image != NULL)
        swiCopy(&color, sprite->image, ((sprite->size & 0xFFF) << 4) | COPY_MODE_FILL);
    else
        sprite->alive = false;

    sprite->hp = hp;
    sprite->idNum = idNum;
    sprite->facing = rand() % 4;
    sprite->score = 0;

}

int initMobs(int tileWidth, SpriteSize size, SpriteColorFormat colFormat){

    int mobX = 0;
    int mobY = 0;
    int numMobs = (rand() % 7) + 6;
    int mobHP = 5;
    bool finished = false;
    int i = 0;

    for(i = 0; i < numMobs; i++){

        finished = false;
        while(!finished){

            mobX = rand() % relativeWidth;
            mobY = rand() % relativeHeight;
            //Place the mobs on spaces not occupied by another sprite
            if(map[mobX + mobY * relativeWidth] == 01 && grid[mobX][mobY] == 0){

                finished = true;
                grid[mobX][mobY] = 2;

            }

        }
        newSprite(&mobs[i], mobX * tileWidth, mobY * tileWidth, 8, 8, size, colFormat, mobHP, 2, 2);

    }

    return numMobs;

}

int initItems(int tileWidth, SpriteSize size, SpriteColorFormat colFormat){

    int itemX = 0;
    int itemY = 0;
    int numItems = (rand() % 5) + 2;
    bool finished = false;
    int i = 0;

    for(i = 0; i < numItems; i++){

        finished = false;
        while(!finished){

            itemX = rand() % relativeWidth;
            itemY = rand() % relativeHeight;
            if(map[itemX + itemY * relativeWidth] == 01 && grid[itemX][itemY] == 0){

                finished = true;
                grid[itemX][itemY] = 3;

            }

        }
        newSprite(&items[i], itemX * tileWidth, itemY * tileWidth, 8, 8, size, colFormat, 1, 3, 3);

    }

    return numItems;

}

void findItem(int itemX, int itemY, int numItems){

    int i = 0;

    for(i = 0; i < numItems; i++)
        if(items[i].x / tileWidth == itemX && items[i].y / tileWidth == itemY){

            items[i].alive = false;
            oamFreeGfx(oam, items[i].image);

            //Items boost the player's score and heal them slightly
            player.score = player.score + 50;
            player.hp = player.hp + 5;
            if(player.hp > 20)
                player.hp = 20;

            break;

        }

}

void moveMobs(int numMobs){

    int newX = 0;
    int newY = 0;
    int moveVal = 0;
    int i = 0;

    //Each of the mobs will move erratically
    for(i = 0; i < numMobs; i++){

        if(mobs[i].alive){

            newX = mobs[i].x / tileWidth;
            newY = mobs[i].y / tileWidth;

            moveVal = rand() % 10;
            if(moveVal == 0)
                mobs[i].facing = rand() % 4;

            if(mobs[i].facing == UP)
                newY--;
            else if(mobs[i].facing == DOWN)
                newY++;
            else if(mobs[i].facing == LEFT)
                newX--;
            else
                newX++;

            if(map[newX + newY * relativeWidth] != 00){

                if(grid[newX][newY] == 0){

                    grid[mobs[i].x / tileWidth][mobs[i].y / tileWidth] = 0;
                    grid[newX][newY] = mobs[i].idNum;
                    mobs[i].x = newX * tileWidth;
                    mobs[i].y = newY * tileWidth;

                }else if(grid[newX][newY] == player.idNum){

                    fightMob(i, 0, 0, numMobs);

                }

            }

        }

    }

}

void fightMob(int index, int mobX, int mobY, int numMobs){

    int playerDmg = 2;
    int mobDmg = 3;
    int i = 0;

    if(index == -1){

        //Find the mob being fought
        for(i = 0; i < numMobs; i++)
            if(mobs[i].x / tileWidth == mobX && mobs[i].y / tileWidth == mobY){

                index = i;
                break;

            }

    }

    //If the player is behind the mob, it deals no damage to the player
    if(player.facing == mobs[index].facing &&
       ((player.facing == UP && player.y > mobY) ||
        (player.facing == DOWN && player.y < mobY) ||
        (player.facing == LEFT && player.x > mobX) ||
        (player.facing == RIGHT && player.x < mobX)))
        mobDmg = 0;

    player.hp = player.hp - mobDmg;
    mobs[index].hp = mobs[index].hp - playerDmg;

    if(mobs[index].hp <= 0){

        mobs[index].alive = false;
        oamFreeGfx(oam, mobs[index].image);
        grid[mobs[index].x / tileWidth][mobs[index].y / tileWidth] = 0;

    }

}

//Draw the sprites
void update(int numMobs, int numItems){

    int i = 0;
    int id = 0;

    //Draw the player
    oamSet(oam, id, player.x, player.y, 0, 0, player.size, player.colFormat, player.image, 
           -1, false, !player.alive, false, false, false);
    id++;

    //Draw the mobs
    for(i = 0; i < numMobs; i++){

        oamSet(oam, id, mobs[i].x, mobs[i].y, 0, 0, mobs[i].size, mobs[i].colFormat, mobs[i].image, 
               -1, false, !mobs[i].alive, false, false, false);
        id++;

    }

    //Draw the items
    for(i = 0; i < numItems; i++){

        oamSet(oam, id, items[i].x, items[i].y, 0, 0, items[i].size, items[i].colFormat, items[i].image, 
               -1, false, !items[i].alive, false, false, false);
        id++;

    }

}