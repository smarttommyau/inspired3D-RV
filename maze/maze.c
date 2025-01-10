// use this if you want shortcut to common colors
#define INSPIRE3D_DISPLAY_COMMON_COLOR 
#define CH32V003_I2C_IMPLEMENTATION // using i2c

#include "ch32v003fun.h"
#include <stdio.h>
#include "driver.h"
#include "ch32v003_i2c.h"
#include "inspire3d_display.h"

//maze edges
static uint8_t edges[125*3][3];
// share memory to below variables

// 0: path, 1: wall, 2: start, 3: end
// 3D maze
// 3x3 for 1,3,5 so actually 3x3x3
// 2,4 layers are filled with walls
uint8_t maze[125]; // 125 bytes
uint8_t ufParent[125]; // 125 bytes
uint8_t ufRankUF[125]; // 125 bytes

static uint8_t findSet3D(uint8_t v) {
    if (ufParent[v] == v) return v;
    ufParent[v] = findSet3D(ufParent[v]);
    return ufParent[v];
}

static void unionSet3D(uint8_t a, uint8_t b) {
    a = findSet3D(a);
    b = findSet3D(b);
    if (a != b) {
        if (ufRankUF[a] < ufRankUF[b]) {
            uint8_t tmp = a; a = b; b = tmp;
        }
        ufParent[b] = a;
        if (ufRankUF[a] == ufRankUF[b]) {
            ufRankUF[a]++;
        }
    }
}

// Convert (x,y,z) in [0..4,0..4,0..4] to index 0..124 in maze[]
static uint8_t posToId3D(uint8_t x, uint8_t y, uint8_t z) {
    return x + 5*(y + 5*z);
}
void generateMaze(){
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
    uint8_t edgeCount=0;
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

    // Shuffle edges
    for(uint8_t i=0; i<edgeCount; i++){
        uint8_t r = JOY_random() % edgeCount;
        for(uint8_t j=0; j<3; j++){
            uint8_t tmp = edges[i][j];
            edges[i][j] = edges[r][j];
            edges[r][j] = tmp;
        }
    }

    // Kruskal's algorithm
    for(uint8_t i=0; i<edgeCount; i++){
      uint8_t x1 = (edges[i][0] >> 4) & 0x0F;
        uint8_t y1 = edges[i][0] & 0x0F;
        uint8_t z1 = (edges[i][1] >> 4) & 0x0F;
        uint8_t x2 = edges[i][1] & 0x0F;
        uint8_t y2 = (edges[i][2] >> 4) & 0x0F;
        uint8_t z2 = edges[i][2] & 0x0F;

        uint8_t id1 = posToId3D(x1,y1,z1);
        uint8_t id2 = posToId3D(x2,y2,z2);

        if(findSet3D(id1) != findSet3D(id2)){
            unionSet3D(id1, id2);
            // Remove wall => set these cells to 0 (path)
            maze[id1] = 0;
            maze[id2] = 0;
        }
    }
    // random select path cells as start and end
    uint8_t start = JOY_random() % 125;
    uint8_t end = JOY_random() % 125;
    while(maze[start] != 0){
        start = JOY_random() % 125;
    }
    while(maze[end] != 0){
        end = JOY_random() % 125;
    }
    maze[start] = 2;
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
    Inspire3D_Display_Update(display);
    
}

uint8_t seed = 0;

// use edges memory to store display buffer, as lack of memory
char * display_buffer = (char *)&edges; //[sizeof(Inspire3D_Display)]


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
    Inspire3D_Display_Clear(display);// reset data and update
    Inspire3D_Display_SetBrightness(display, 0.05);
    uint8_t mode = 0; //0: select; 1: selected
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
    generateMaze();
    show_maze(display);

    while(1){
        // read ABCD key
        uint16_t abcd_reading   = abcd_key_read_ADC();
        uint16_t arrow_reading  = arrow_key_read_ADC();
        ARROW_KEY arrow         = arrow_key_down(arrow_reading);
        ABCD_KEY abcd           = abcd_key_down(abcd_reading);
        if(mode == 0){
            if(arrow == ARROW_UP){
                y = (y + 1) % 5;
            } else if(arrow == ARROW_DOWN){
                y = (y - 1) % 5;
                y = y < 0 ? 4 : y;
            } else if(arrow == ARROW_RIGHT){
                x = (x + 1) % 5;
            } else if(arrow == ARROW_LEFT){
                x = (x - 1) % 5;
                x = x < 0 ? 4 : x;
            }
            // read arrow key
            if(abcd == ABCD_A){
                z = (z + 1) % 5;
            } else if(abcd == ABCD_B){
                z = (z - 1) % 5;
                z = z < 0 ? 4 : z;
            }
        }
        // select with CD keys
        if(abcd == ABCD_C||abcd == ABCD_D){
            if(mode == 0){
                mode = 1;
                Inspire3D_Display_SetBGColor(display, Inspire3D_Color_setRGB(y*50,x*50,z*50));
                Inspire3D_Display_Update(display);
                Delay_Ms(200);
            } else {
                mode = 0;
            }
        }
        // blink color
        uint8_t index = Inspire3D_Display_Coords2Index(x,y,z);
        // Inspire3D_Display_SetColor(display, index, Inspire3D_Color_setRGB(y*50,x*50,z*50));
        printf("x: %d, y: %d, z: %d adc: %d\n", x, y, z, abcd_reading);
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Black);
        Inspire3D_Display_Update(display);
        Delay_Ms(100);
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_setRGB(y*50,x*50,z*50));
        Inspire3D_Display_Update(display);
        Delay_Ms(100);
    }
}
