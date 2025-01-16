#define CH32V003_I2C_IMPLEMENTATION // using i2c
#include "ch32v003fun.h"
#include "driver.h"
#define INSPIRE3D_DISPLAY_COMMON_COLOR // using common colors
#include "inspire3d_display.h"

#include <stdio.h>

char display_buffer[sizeof(Inspire3D_Display)]; // memory for display
Inspire3D_Display *display = (Inspire3D_Display*)display_buffer;
uint8_t image[125][3]; // image for display
uint8_t image_rotated[125][3]; // rotated image

void generate_random_image(){
    for (uint8_t i = 0; i < 125; i++)
    {
        uint16_t x,y,z;
        Inspire3D_Display_Index2Coords(i, &x, &y, &z);
        // only set color if not on the edge
        if( x == 0 || y == 0 || z == 0 || x == 4 || y == 4 || z == 4){
            image[i][0] = 0;
            image[i][1] = 0;
            image[i][2] = 0;
            continue;
        }
        uint8_t r = JOY_random() % 256;
        uint8_t g = JOY_random() % 256;
        uint8_t b = JOY_random() % 256;
        image[i][0] = r;
        image[i][1] = g;
        image[i][2] = b;
        Inspire3D_Display_SetColor(display, i, Inspire3D_Color_setRGB(r, g, b));
    }
    Inspire3D_Display_Update(display);
}

struct coords{
    uint8_t x;
    uint8_t y;
    uint8_t z;
    Inspire3D_Color color;
};

#define cos30 0.86602540378
#define sin30 0.5

void rotate(uint8_t dir){
    // Refernce: https://stackoverflow.com/a/49179544
    // dir 0: up
    // dir 1: down
    // dir 2: left
    // dir 3: right
    // each iterate by 30 degree
    // cos(30) = 0.86602540378
    // sin(30) = 0.5
    int8_t min_new_x = 0;
    int8_t max_new_x = 0;
    int8_t min_new_y = 0;
    int8_t max_new_y = 0;
    int8_t min_new_z = 0;
    int8_t max_new_z = 0;
    struct coords new_coords[125];
    for(int8_t z=0;z<5;z++)
        for(int8_t y=0;y<5;y++)
            for(int8_t x=0;x<5;x++){
                int8_t newx = 0,newy = 0,newz = 0;
                switch(dir){
                    case 0:
                        z = -z;
                    case 1:
                        newx = x*cos30 + z*sin30;
                        newy = y;
                        newz = z*sin30 - x*cos30;
                        break;
                    case 2:
                        newy = z*sin30*2;
                        newz = -y*sin30*2;
                    case 3:
                        newx = x;
                        newy += y*cos30 - z*sin30 ;
                        newz += y*sin30 + z*cos30;

                        break;
                }
            }
}



int main(void) {
    // initialize everthing
    SystemInit();
    GPIO_ADCinit();
    MY_I2C_init();

    // initialize display
    Inspire3D_Display_Init(display, GPIOA, PA2);
    Inspire3D_Display_Clear(display);
    // set seed for random
    uint16_t seed = 0;
    while (1)
    {
        ABCD_KEY abcd = abcd_key_down(abcd_key_read_ADC());
        ARROW_KEY arrow = arrow_key_down(arrow_key_read_ADC());
        if (abcd != ABCD_NOT_FOUND || arrow != ARROW_NOT_FOUND)
        {
            break;
        }
        seed++;
        if (seed >= 65530)
        {
            seed = 0;
        }
        Delay_Ms(10);
    }
    JOY_setseed(seed);
    // randomize image
    generate_random_image();
    for(uint8_t i = 0; i < 125; i++){
        image_rotated[i][0] = image[i][0];
        image_rotated[i][1] = image[i][1];
        image_rotated[i][2] = image[i][2];
    }
    while(1){
        ABCD_KEY abcd = abcd_key_down(abcd_key_read_ADC());
        ARROW_KEY arrow = arrow_key_down(arrow_key_read_ADC());
        if(abcd == ABCD_A){
            generate_random_image();
            for(uint8_t i = 0; i < 125; i++){
                image_rotated[i][0] = image[i][0];
                image_rotated[i][1] = image[i][1];
                image_rotated[i][2] = image[i][2];
            }
        }else if(abcd == ABCD_B){
            for(uint8_t i = 0; i < 125; i++){
                image_rotated[i][0] = image[i][0];
                image_rotated[i][1] = image[i][1];
                image_rotated[i][2] = image[i][2];
            }
        }
        if(arrow == ARROW_UP){
            rotate(0);
        }else if(arrow == ARROW_DOWN){
            rotate(1);
        }else if(arrow == ARROW_LEFT){
            rotate(2);
        }else if(arrow == ARROW_RIGHT){
            rotate(3);
        }
    }
}
