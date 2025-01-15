// use this if you want shortcut to common colors
#define INSPIRE3D_DISPLAY_COMMON_COLOR 
#define CH32V003_I2C_IMPLEMENTATION // using i2c

#include "ch32v003fun.h"
#include <stdio.h>
#include "driver.h"
#include "inspire3d_display.h"


//maze edges
// static uint8_t edges[125*3][3];
// share memory to below variables

// 0: path, 1: wall, 2: start, 3: end
// 3D maze
// 3x3 for 1,3,5 so actually 3x3x3
// 2,4 layers are filled with walls
uint8_t maze[125]; 

#define MAX_ITERATION 20
void show_maze(Inspire3D_Display * display);

bool checkIfConnect(uint8_t depth, uint8_t index){
    if (depth>MAX_ITERATION) return false;
    if(maze[index] == 1||maze[index]==4){return false;}
    if(maze[index] == 3){return true;}
    uint8_t x,y,z;
    maze[index] = 4;
    Inspire3D_Display_Index2Coords(index, &x, &y, &z);
    return (x+1<5)?checkIfConnect(depth+1, Inspire3D_Display_Coords2Index(x+1,y,z)):false ||
           (x-1>0)?checkIfConnect(depth+1, Inspire3D_Display_Coords2Index(x-1,y,z)):false ||
           (y+1<5)?checkIfConnect(depth+1, Inspire3D_Display_Coords2Index(x,y+1,z)):false ||
           (y-1>0)?checkIfConnect(depth+1, Inspire3D_Display_Coords2Index(x,y-1,z)):false ||
           (z+1<5)?checkIfConnect(depth+1, Inspire3D_Display_Coords2Index(x,y,z+1)):false ||
           (z-1>0)?checkIfConnect(depth+1, Inspire3D_Display_Coords2Index(x,y,z-1)):false;

}


void generateMaze(Inspire3D_Display * display){
    for(uint8_t i=0; i<125; i++){
        maze[i] = abs(JOY_random() % 3);// add chance for wall
        maze[i] = maze[i] == 2 ? 1 : maze[i];
    }
    // set start and end
    uint8_t start = JOY_random()%125;
    maze[start] = 2;
    uint8_t end = JOY_random()%125;
    while(end == start){
        end = JOY_random()%125;
    }
    maze[end] = 3;
    printf("Start: %d End: %d\n", start,end);
    show_maze(display);

    while(!checkIfConnect(0, start)){
        // restore 4 to 0
        for(uint8_t i=0; i<125; i++){
            if(maze[i] == 4){
                maze[i] = 0;
            }
        }
        // randomly break a wall
        uint8_t index = JOY_random()%125;
        while(maze[index] != 1){
            index = JOY_random()%125;
        }
        maze[index] = 0;
        maze[start] = 2;
        show_maze(display);
        Delay_Ms(500);
    }
    maze[start] = 2;
    for(uint8_t i=0; i<125; i++){
        if(maze[i] == 4){
            maze[i] = 0;
        }
    }
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
    Delay_Ms(500);
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
    MY_I2C_init();

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
        if(seed >= 65530){
            seed = 0;
        }
        Delay_Ms(10);
    }
    JOY_setseed(seed);
    generateMaze(display);
    Inspire3D_Display_SetBGColor(display,Inspire3D_Color_Black);
    Delay_Ms(500);
    show_maze(display);
    printf("Game_start");
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
    uint8_t refresh_count = 6;// prevent purple dots not disappearing due to random issue
    while(1){
        // read ABCD key
        uint16_t abcd_reading   = abcd_key_read_ADC();
        uint16_t arrow_reading  = arrow_key_read_ADC();
        ARROW_KEY arrow         = arrow_key_down(arrow_reading);
        ABCD_KEY abcd           = abcd_key_down(abcd_reading);
        // printf("%d %d %d %d\n",arrow_reading, arrow_reading, arrow, abcd);
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
        } else if(arrow == ARROW_LEFT){
            int8_t tx = x + 1;
            if(tx < 5 && maze[Inspire3D_Display_Coords2Index(tx,y,z)] != 1){
            x = tx;
            }
        } else if(arrow == ARROW_RIGHT){
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
        refresh_count--;
        if(refresh_count == 0){
            show_maze(display);
            refresh_count = 6;
        }
    }
    // show end animaton
    Delay_Ms(1000);
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
