/*Andrija Jovanov
  A Nintendo DS game meant to introduce me to embedded systems development.
  It is presently very early in development. More work should be done on it over the course of the next
  couple of months. As it is now, much of it is based off of a DrunkenCoders tutorial.*/

#include <nds.h>
#include <stdio.h>
#include <time.h>

static const int DMA_CHANNEL = 3;

/*A 32x32 map*/
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
00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
};

//Coloured tiles
u8 tiles[] = {
    //00 = black
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    //01 = blue
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
void makePath(int x, int y, int relativeWidth, int relativeHeight);
int * findUnvisited(int unvisited[], int x, int y, int relativeWidth, int relativeHeight);

int main(){

    int bg = 0;
    int x, y;
    int keys = 0;
    int sx = 0;
    int sy = 0;
    int width = 640;            //Width of the world
    int height = 640;           //Height of the world
    int relativeWidth = 32;     //Width of the map
    int relativeHeight = 32;    //Height of the map

	powerOn(POWER_ALL_2D); //Turn on all 2D hardware
	videoSetup();          //Set vram banks and video modes
	bg = BGSetup();        //Set up background

	//Set background colour values
    BG_PALETTE[0] = RGB15(0,0,0); //black
    BG_PALETTE[1] = RGB15(199,97,20); //tan

    //Maze generator
    srand(time(NULL));
    x = rand() % relativeWidth;
    y = rand() % relativeHeight;
    map[x + y * relativeWidth] = 01;
    makePath(x, y, relativeWidth, relativeHeight);
    
    //Copy tiles into memory
    dmaCopy(tiles, bgGetGfxPtr(bg), sizeof(tiles));
    
    //Points to map in hardware
    u16* m = bgGetMapPtr(bg);

    //Copy the map into hardware
    for(int x = 0; x < relativeWidth; x++)
        for(int y = 0; y < relativeHeight; y++)
            m[x + y * relativeWidth] = map[x + y * relativeWidth];

    while(1){

		swiWaitForVBlank();

		scanKeys();
        keys = keysHeld();

        //Scroll if the directional keys are held
        if(keys & KEY_UP)
            sy--;
        if(keys & KEY_DOWN)
            sy++;
        if(keys & KEY_LEFT)
            sx--;
        if(keys & KEY_RIGHT)
            sx++;

        if(sx < 0)
            sx = 0;
        if(sx >= width - 256)
            sx = width - 1 - 256;
        if(sy < 0)
            sy = 0;
        if(sy >= height - 192)
            sy = height - 1 - 192;

        bgSetScroll(bg, sx, sy);
        bgUpdate();

        if(keysDown() & KEY_B)
            break;

	}

	return 0;

}

//Set vram banks and video modes
void videoSetup(){

	/*Set the VRAM banks:
	  A - Main screen background memory
	  B - Main screen background memory
	  C - Sub screen background memory
	  D - LCD memory
	*/
	vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000,
                        VRAM_B_MAIN_BG_0x06020000,
                        VRAM_C_SUB_BG_0x06200000,
                        VRAM_D_LCD);

	//Set main screen and sub screen video modes.
	videoSetMode(MODE_5_2D | DISPLAY_BG0_ACTIVE);
	videoSetModeSub(MODE_5_2D);

}

//Set up background
int BGSetup(){

	//initialize a tiled background.  A "text" background thats 256x256 
    return bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);

}

//Generate the maze using depth-first search
void makePath(int x, int y, int relativeWidth, int relativeHeight){

    int unvisitedA[5];
    int * unvisited;
    int randNum;

    while(1){

        unvisited = findUnvisited(unvisitedA, x, y, relativeWidth, relativeHeight);

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
            makePath(x, y, relativeWidth, relativeHeight);

        }else{

            break;

        }

    }

}

//Find the directions the maze can currently branch out to without colliding with itself
int * findUnvisited(int unvisited[], int x, int y, int relativeWidth, int relativeHeight){

    int i;

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