#define CH32V003_I2C_IMPLEMENTATION // using i2c
#include "ch32v003fun.h"
#include "ch32v003_i2c.h"
#include "driver.h"
#define INSPIRE3D_DISPLAY_COMMON_COLOR // using common colors
#include "inspire3d_display.h"

#include <stdio.h>

#define PAD_SIZE 3
#define PAD_REGION_SIZE 6 - PAD_SIZE

#define TICK 1000
#define CHECKPERTICK 3


char display_buffer[sizeof(Inspire3D_Display)]; // memory for display

void draw_pad(Inspire3D_Display *display, int x, int y, int z, Inspire3D_Color color) {
    // z should be 0 or 4
    // 3x3 pad
    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            Inspire3D_Display_SetColor(
                display, 
                Inspire3D_Display_Coords2Index(x + i, y + j, z), 
                color
            );
        }
    }
}
void draw_ball(Inspire3D_Display *display, int x, int y, int z, Inspire3D_Color color) {
            Inspire3D_Display_SetColor(
                display, 
                Inspire3D_Display_Coords2Index(x, y, z), 
                color
            );
}


//coordinates of red pad (3x3 space) (z=0)
uint8_t Red[2] = {1,1}; 
//coordinates of blue pad (3x3 space) (z=4)
uint8_t Blue[2] = {1,1}; 
uint8_t Ball[3] = {2,2,2}; //coordinates of ball (5x5x5 space)
uint8_t VBall[3] = {0,0,0}; //velocity of ball (5x5x5 space)

bool interate_ball(){
    // true for win, false for continue
    for(int i = 0; i < 3; i++){
        Ball[i] += VBall[i];
    }
    for(int i = 0; i < 2; i++){
        if(Ball[i] < 0){
            Ball[i] = 1;
            VBall[i] *= -1;
        }else if(Ball[i] > 4){
            Ball[i] = 3;
            VBall[i] *= -1;
        }
    }
    if(Ball[2] == 0){
        // check touch red pad (z=0)
        if(
            Ball[0] >= Red[0] && Ball[0] < Red[0] + PAD_SIZE 
        &&  Ball[1] >= Red[1] && Ball[1] < Red[1] + PAD_SIZE
        ){
            Ball[2] = 1;
            VBall[2] = 1;
        }else{
            Ball[2] = 1;
            VBall[2] = 1;
            return true;
        }
    }else if(Ball[2] == 4){
        // check touch blue pad (z=4)
        if(
            Ball[0] >= Blue[0] && Ball[0] < Blue[0] + PAD_SIZE 
        &&  Ball[1] >= Blue[1] && Ball[1] < Blue[1] + PAD_SIZE
        ){
            Ball[2] = 3;
            VBall[2] = -1;
        }else{
            Ball[2] = 3;
            VBall[2] = -1;
            return true;
        }
    }
    return false;
}

void moveRedPad(ARROW_KEY key){
    if(key == ARROW_UP && Red[1] < PAD_REGION_SIZE - 1){
        Red[1]++;
    }else if(key == ARROW_DOWN && Red[1] > 0){
        Red[1]--;
    }else if(key == ARROW_LEFT && Red[0] > 0){
        Red[0]--;
    }else if(key == ARROW_RIGHT && Red[0] < PAD_REGION_SIZE - 1){
        Red[0]++;
    }
}
void moveBluePad(ABCD_KEY key){
    if(key == ABCD_A && Blue[1] < PAD_REGION_SIZE - 1){
        Blue[1]++;
    }else if(key == ABCD_B && Blue[1] > 0){
        Blue[1]--;
    }else if(key == ABCD_C && Blue[0] > 0){
        Blue[0]--;
    }else if(key == ABCD_D && Blue[0] < PAD_REGION_SIZE - 1){
        Blue[0]++;
    }
}


int seed = 0;

int main(void) {
    // initialize everthing
    SystemInit();
    GPIO_ADCinit();
    i2c_init();
    i2c_scan();
    printf("I2C scan done\n");
    // initialize display
    Inspire3D_Display *display = (Inspire3D_Display*)display_buffer;
    Inspire3D_Display_Init(display,GPIOA,PA2);
    // coords of pad 3x3 plane
while(1){
    draw_pad(display, 1, 1, 0, Inspire3D_Color_Red);
    draw_pad(display, 1, 1, 4, Inspire3D_Color_Blue);
    Inspire3D_Display_Update(display);
    uint16_t arrow_adc   = arrow_key_read_ADC();
    //set brightness before start
    while(!arrow_key_pressed(arrow_adc, ARROW_DOWN)){
        if(arrow_key_pressed(arrow_adc, ARROW_LEFT) && display->brightness > 0.1){
            Inspire3D_Display_SetBrightness(display, display->brightness - 0.1);
        }else if(arrow_key_pressed(arrow_adc, ARROW_RIGHT) && display->brightness < 1.0){
            Inspire3D_Display_SetBrightness(display, display->brightness + 0.1);
        }
        Inspire3D_Display_Update(display);
        arrow_adc   = arrow_key_read_ADC();
        seed++;
        Delay_Ms(200);
        printf("Brightness: %f\n", display->brightness);
    }
    printf("Brightness set\n");
    // start game
    Ball[0] = 2;Ball[1] = 2;Ball[2] = 2;
    Red[0]  = 1;Red[1]  = 1;
    Blue[0] = 1;Blue[1] = 1;
    draw_ball(display, Ball[0], Ball[1], Ball[2], Inspire3D_Color_White);
    Inspire3D_Display_Update(display);
    Delay_Ms(3000);
    //set random seed
    JOY_setseed(seed);
    // random set VBall
    // 1 or 0 or -1 for each values
    // for z value, only 1 or -1
    VBall[0] = JOY_random() % 3 - 1;
    VBall[1] = JOY_random() % 3 - 1;
    // VBall[2] = JOY_random() % 2 * 2 - 1; 
    VBall[2] = 0; 
    

    printf("Start game\n");
    while(1){
        if(interate_ball()){
            // win
            draw_ball(display, Ball[0], Ball[1], Ball[2], Inspire3D_Color_Green);
            Inspire3D_Display_Update(display);
            Delay_Ms(1000);
            break;
        }
        // check 3 input in one tick
        for(int i=0;i<CHECKPERTICK;i++){
            Inspire3D_Display_Reset(display);
            arrow_adc   = arrow_key_read_ADC();
            uint8_t abcd_adc   = abcd_key_read_ADC();
            ARROW_KEY arrow_key = arrow_key_down(arrow_adc);
            ABCD_KEY abcd_key = abcd_key_down(abcd_adc);
            moveRedPad(arrow_key);
            moveBluePad(abcd_key);
            draw_ball(display, Ball[0], Ball[1], Ball[2], Inspire3D_Color_Green);
            draw_pad(display, Red[0], Red[1], 0, Inspire3D_Color_Red);
            draw_pad(display, Blue[0], Blue[1], 4, Inspire3D_Color_Blue);
            Inspire3D_Display_Update(display);
            printf("Red: %d %d\n", Red[0], Red[1]);
            printf("Blue: %d %d\n", Blue[0], Blue[1]);
            Delay_Ms(TICK/CHECKPERTICK);
        }
    }
    printf("Game over\n");
    Delay_Ms(3000);
    Inspire3D_Display_Clear(display);
}
    
}
