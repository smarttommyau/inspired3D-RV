// use this if you want shortcut to common colors
#define INSPIRE3D_DISPLAY_COMMON_COLOR 
#define CH32V003_I2C_IMPLEMENTATION // using i2c

#include "ch32v003fun.h"
#include <stdio.h>
#include "driver.h"
#include "ch32v003_i2c.h"
#include "inspire3d_display.h"

//maze edges
uint8_t edges[300][3];

// 0: path, 1: wall, 2: start, 3: end
// 3D maze
uint8_t maze[125]; // 125 bytes
uint8_t ufParent[125]; // 125 bytes
uint8_t ufRankUF[125]; // 125 bytes

void show_maze(Inspire3D_Display * display);

static uint8_t findSet3D(uint8_t v) {
    uint8_t root = v;
    printf("Checkpoint set3d %d\n", v); 
    while (root != ufParent[root]) {
        root = ufParent[root];
        printf("Checkpoint set3d 2 %d\n", root); 
    }
    printf("Checkpoint set3d3\n"); 
    // Path compression
    while (v != root) {
        uint8_t p = ufParent[v];
        ufParent[v] = root;
        v = p;
    }
    printf("Checkpoint set3d4\n"); 
    return root;
}

static void unionSet3D(uint8_t a, uint8_t b) {
    a = findSet3D(a);
    b = findSet3D(b);
    if (a == b) return;

    if (ufRankUF[a] < ufRankUF[b]) {
        uint8_t tmp = a; a = b; b = tmp;
    }
    ufParent[b] = a;
    if (ufRankUF[a] == ufRankUF[b]) {
        ufRankUF[a]++;
    }
}

void generateMaze(Inspire3D_Display * display){
    //set all cells to wall
    for(uint8_t i=0; i<125; i++){
        maze[i] = 1;
    }
    // Initialize union-find
    for(uint8_t i=0; i<125; i++){
        ufParent[i] = i;
        ufRankUF[i] = 0;
    }

    // Build a list of edges for (x+1,y,z), (x,y+1,z), (x,y,z+1)
    // Each edge: {x1,y1,z1, x2,y2,z2}
    // Lack of memory, use edges[125*3][3] to store edges
    // instead of [125*3][6]
    // 4 bytes for each edge
    uint16_t edgeCount=0;
    for(uint8_t x=0; x<5; x++){
        for(uint8_t y=0; y<5; y++){
            for(uint8_t z=0; z<5; z++){
                // Next x
                if(x+1 < 5){
                    // (x,y,z), (x+1,y,z)
                    uint8_t x2 = x+1, y2 = y, z2 = z;
                    edges[edgeCount][0] = ((x & 0x0F) << 4) | (y & 0x0F);
                    edges[edgeCount][1] = ((z & 0x0F) << 4) | (x2 & 0x0F);
                    edges[edgeCount][2] = ((y2 & 0x0F) << 4) | (z2 & 0x0F);
                    edgeCount++;
                }
                // Next y
                if(y+1 < 5){
                    // (x,y,z), (x,y+1,z)
                    uint8_t x2 = x, y2 = y+1, z2 = z;
                    edges[edgeCount][0] = ((x & 0x0F) << 4) | (y & 0x0F);
                    edges[edgeCount][1] = ((z & 0x0F) << 4) | (x2 & 0x0F);
                    edges[edgeCount][2] = ((y2 & 0x0F) << 4) | (z2 & 0x0F);
                    edgeCount++;
                }
                // Next z
                if(z+1 < 5){
                    // (x,y,z), (x,y,z+1)
                    uint8_t x2 = x, y2 = y, z2 = z+1;
                    edges[edgeCount][0] = ((x & 0x0F) << 4) | (y & 0x0F);
                    edges[edgeCount][1] = ((z & 0x0F) << 4) | (x2 & 0x0F);
                    edges[edgeCount][2] = ((y2 & 0x0F) << 4) | (z2 & 0x0F);
                    edgeCount++;
                }
            }
        }
    }
    printf("Edge count: %d\n", edgeCount);
    // Shuffle edges
    for(uint16_t i=0; i<edgeCount; i++){
        uint16_t r = JOY_random() % edgeCount;
        printf("r: %d\n", r);
        for(uint8_t j=0; j<3; j++){
            uint8_t tmp = edges[i][j];
            edges[i][j] = edges[r][j];
            edges[r][j] = tmp;
        }
    }
    printf("Shuffle done\n");
    printf("Calculate maze\n");
    // Kruskal's algorithm
    for(uint16_t i=0; i<edgeCount; i++){
        printf("i: %d/%d\n", i,edgeCount);
        uint8_t x1 = (edges[i][0] >> 4) & 0x0F;
        uint8_t y1 = edges[i][0] & 0x0F;
        uint8_t z1 = (edges[i][1] >> 4) & 0x0F;
        uint8_t x2 = edges[i][1] & 0x0F;
        uint8_t y2 = (edges[i][2] >> 4) & 0x0F;
        uint8_t z2 = edges[i][2] & 0x0F;
        printf("x1: %d, y1: %d, z1: %d, x2: %d, y2: %d, z2: %d\n", x1, y1, z1, x2, y2, z2);
        printf("checkpoint 1\n");
        uint8_t id1 = Inspire3D_Display_Coords2Index(x1,y1,z1);
        uint8_t id2 = Inspire3D_Display_Coords2Index(x2,y2,z2);
        printf("checkpoint 2 %d %d\n", id1, id2);
        uint8_t a = findSet3D(id1);
        uint8_t b = findSet3D(id2);
        printf("checkpoint 3\n");
        if(a != b){
            unionSet3D(id2, id1);
            printf("checkpoint 3\n");
            // Remove wall => set these cells to 0 (path)
            maze[id1] = 0;
            // maze[id2] = 0;
            show_maze(display);
        }
    }
    // random select path cells as start and end
    uint8_t start = JOY_random() % 125;
    uint8_t end = JOY_random() % 125;
    while(maze[start] != 0){
        start = JOY_random() % 125;
    }
    maze[start] = 2;
    while(maze[end] != 0){
        end = JOY_random() % 125;
    }
    maze[end] = 3;
}

void show_maze(Inspire3D_Display * display){
    for(uint8_t i=0; i<125; i++){
        printf("%d ", maze[i]);
        if(maze[i] == 1){
            Inspire3D_Display_SetColor(display, i, Inspire3D_Color_Red);
        } else if(maze[i] == 0){
            Inspire3D_Display_SetColor(display, i, Inspire3D_Color_Black);
        } else if(maze[i] == 2){
            Inspire3D_Display_SetColor(display, i, Inspire3D_Color_Green);
        } else if(maze[i] == 3){
            Inspire3D_Display_SetColor(display, i, Inspire3D_Color_Yellow);
        }
    }
    printf("\nShow maze\n");
    Inspire3D_Display_Update(display);
    
}

void blink_point(Inspire3D_Display * display,int index){
    // blink color with the maze
    Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Purple);
    Inspire3D_Display_Update(display);
    Delay_Ms(100);
    if(maze[index] == 0){
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Black);
    } else if(maze[index] == 1){
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Red);
    } else if(maze[index] == 2){
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Green);
    } else if(maze[index] == 3){
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Yellow);
    }
    Inspire3D_Display_Update(display);

}


uint8_t seed = 0;

char display_buffer[sizeof(Inspire3D_Display)];


int main(void) {
    SystemInit();
    GPIO_ADCinit();
    i2c_init();
    i2c_scan();

    printf("I2C initialized\n");
    // int seed = 0;
    int x,y,z = 0;// selector coords
    Inspire3D_Display * display = (Inspire3D_Display *)&display_buffer;
    Inspire3D_Display_Init(display,GPIOA, PA2);
while(1){ //program loop
    Inspire3D_Display_SetBGColor(display, Inspire3D_Color_Green);
    Inspire3D_Display_SetBrightness(display, 0.05);
    Inspire3D_Display_Update(display);
    //wait and increment seed until any button is pressed
    while(1){
        uint16_t abcd_reading = abcd_key_read_ADC();
        ABCD_KEY abcd = abcd_key_down(abcd_reading);
        uint16_t arrow_reading = arrow_key_read_ADC();
        ARROW_KEY arrow = arrow_key_down(arrow_reading);
        if(abcd != ABCD_NOT_FOUND ||  arrow != ARROW_NOT_FOUND){
            break;
        }
        seed++;
        if(seed > 65535){
            seed = 0;
        }
        Delay_Ms(10);
    }
    JOY_setseed(seed);
    generateMaze(display);
    show_maze(display);
    // set x,y,z to start
    // find start
    for(uint8_t i=0; i<125; i++){
        if(maze[i] == 2){
            uint8_t tx,ty,tz;
            Inspire3D_Display_Index2Coords(i, &tx, &ty, &tz);
            x = tx;
            y = ty;
            z = tz;
            break;
        }
    }
    
    while(1){
        // read ABCD key
        uint16_t abcd_reading   = abcd_key_read_ADC();
        uint16_t arrow_reading  = arrow_key_read_ADC();
        ARROW_KEY arrow         = arrow_key_down(arrow_reading);
        ABCD_KEY abcd           = abcd_key_down(abcd_reading);
        printf("%d %d %d %d\n",arrow_reading, arrow_reading, arrow, abcd);
        if(arrow == ARROW_UP){
            int8_t ty = y + 1; 
            if(ty < 5 && maze[Inspire3D_Display_Coords2Index(x,ty,z)] != 1){
            y = ty;
            }
        } else if(arrow == ARROW_DOWN){
            int8_t ty = y - 1;
            if(ty >= 0 && maze[Inspire3D_Display_Coords2Index(x,ty,z)] != 1){
            y = ty;
            }
        } else if(arrow == ARROW_RIGHT){
            int8_t tx = x + 1;
            if(tx < 5 && maze[Inspire3D_Display_Coords2Index(tx,y,z)] != 1){
            x = tx;
            }
        } else if(arrow == ARROW_LEFT){
            int8_t tx = x - 1;
            if(tx >= 0 && maze[Inspire3D_Display_Coords2Index(tx,y,z)] != 1){
            x = tx;
            }
        } else if(abcd == ABCD_A){
            int8_t tz = z + 1;
            if(tz < 5 && maze[Inspire3D_Display_Coords2Index(x,y,tz)] != 1){
            z = tz;
            }
        } else if(abcd == ABCD_B){
            int8_t tz = z - 1;
            if(tz >= 0 && maze[Inspire3D_Display_Coords2Index(x,y,tz)] != 1){
            z = tz;
            }
        }
        if(maze[Inspire3D_Display_Coords2Index(x,y,z)] == 3){
            // end
            break;
        }
        blink_point(display, Inspire3D_Display_Coords2Index(x,y,z));
    }
    // show end animaton
    delay(1000);
    while(1){
        // get key values
        uint16_t abcd_reading   = abcd_key_read_ADC();
        uint16_t arrow_reading  = arrow_key_read_ADC();
        ARROW_KEY arrow         = arrow_key_down(arrow_reading);
        ABCD_KEY abcd           = abcd_key_down(abcd_reading);
        Inspire3D_Display_SetBGColor(display, Inspire3D_Color_Yellow);
        Inspire3D_Display_Update(display);
        Delay_Ms(100);
        Inspire3D_Display_SetBGColor(display, Inspire3D_Color_Black);
        Inspire3D_Display_Update(display);
        Delay_Ms(100);
        // check if any key is pressed
        if(arrow != ARROW_NOT_FOUND || abcd != ABCD_NOT_FOUND){
            break;
        }

    }
}
}
