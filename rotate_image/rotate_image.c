#define USE_STL_FILE // when using stl file

#define CH32V003_I2C_IMPLEMENTATION // using i2c
#include "ch32v003fun.h"
#include "driver.h"
#define INSPIRE3D_DISPLAY_COMMON_COLOR // using common colors
#include "inspire3d_display.h"
#include "funconfig.h"
#ifdef USE_STL_FILE
#include "stl_data.h"
#endif
#include <stdio.h>

char display_buffer[sizeof(Inspire3D_Display)]; // memory for display
Inspire3D_Display *display = (Inspire3D_Display*)display_buffer;

// TODO: change color of the image. Allow color filling?
// round float to int
// only work for positive number
int my_round(float x)
{
    return (int)(x + 0.5);
}



#ifndef USE_STL_FILE
struct coords{
    float x;
    float y;
    float z;
};
#define NUM_NODES 25
struct coords image[NUM_NODES];
#endif
struct coords image_rotated[NUM_NODES];

void generate_random_image(){
    for (uint8_t i = 0; i < NUM_NODES; i++)
    {
        // randomize color
        // image[i].color.r = (uint8_t)(JOY_random()>>8) % 256;
        // image[i].color.g = (uint8_t)(JOY_random()>>8) % 256;
        // image[i].color.b = (uint8_t)(JOY_random()>>8) % 256;

        // randomize position
        image[i].x = (float)(JOY_random() % 500)/(float)100.0;
        image[i].y = (float)(JOY_random() % 500)/(float)100.0;
        image[i].z = (float)(JOY_random() % 500)/(float)100.0;
        printf("x: %d.%d\n", (int)image[i].x, (int)((image[i].x - (int)image[i].x)*100));
        printf("y: %d.%d\n", (int)image[i].y, (int)((image[i].y - (int)image[i].y)*100));
        printf("z: %d.%d\n", (int)image[i].z, (int)((image[i].z - (int)image[i].z)*100));
        // printf("r: %d, g: %d, b: %d\n", image[i].color.r, image[i].color.g, image[i].color.b);
    }
    Inspire3D_Display_Update(display);
}

void imageCopyToRotated(){
    for(uint8_t i = 0; i < NUM_NODES; i++){
        image_rotated[i].x = image[i].x;
        image_rotated[i].y = image[i].y;
        image_rotated[i].z = image[i].z;
    }
}


void show_image(struct coords img[NUM_NODES]){
    Inspire3D_Display_Clear(display);
    for(uint8_t i = 0; i < NUM_NODES; i++){
        uint8_t index = Inspire3D_Display_Coords2Index(my_round(img[i].x), my_round(img[i].y), my_round(img[i].z));
        printf("x: %d, y: %d, z: %d, index: %d\n", my_round(img[i].x), my_round(img[i].y), my_round(img[i].z), index);
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Blue);// color blue is temp random color
    }
    Inspire3D_Display_Update(display);
}


#define cos30 0.86602540378
#define sin30 0.5

void rotate(uint8_t dir){
    printf("Rotating %d\n",dir);
    // dir 0: up
    // dir 1: down
    // dir 2: left
    // dir 3: right
    // each iterate by 30 degree
    // cos(30) = 0.86602540378
    // sin(30) = 0.5
    // center 2,2
    for(uint8_t i=0;i<NUM_NODES;i++){
        uint8_t newx,newy,newz;
        switch(dir){
            case 0:
                newz = my_round((image_rotated[i].z- 2)*cos30 - (image_rotated[i].y - 2)*sin30) + 2;
                newy = my_round((image_rotated[i].z- 2)*sin30 + (image_rotated[i].y - 2)*cos30) + 2;
                image_rotated[i].y = newy;
                image_rotated[i].z = newz;
                break;
            case 1:
                newz = my_round((image_rotated[i].z- 2)*cos30 + (image_rotated[i].y - 2)*sin30) + 2;
                newy = my_round(-(image_rotated[i].z- 2)*sin30 + (image_rotated[i].y - 2)*cos30) + 2;
                image_rotated[i].y = newy;
                image_rotated[i].z = newz;
                break;
            case 2:
                newz = my_round((image_rotated[i].z- 2)*cos30 - (image_rotated[i].x - 2)*sin30) + 2;
                newx = my_round((image_rotated[i].z- 2)*sin30 + (image_rotated[i].x - 2)*cos30) + 2;
                image_rotated[i].x = newx;
                image_rotated[i].z = newz;
                break;
            case 3:
                newz = my_round((image_rotated[i].z- 2)*cos30 + (image_rotated[i].x - 2)*sin30) + 2;
                newx = my_round(-(image_rotated[i].z- 2)*sin30 + (image_rotated[i].x - 2)*cos30) + 2;
                image_rotated[i].x = newx;
                image_rotated[i].z = newz;
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

    // only generate random image if not using stl file
    #ifndef USE_STL_FILE
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
    #endif
    imageCopyToRotated();
    show_image(image_rotated);
    Delay_Ms(1000);
    while(1){
        ABCD_KEY abcd = abcd_key_down(abcd_key_read_ADC());
        ARROW_KEY arrow = arrow_key_down(arrow_key_read_ADC());
        if(abcd == ABCD_A){
            #ifndef USE_STL_FILE
            generate_random_image();
            imageCopyToRotated();
            #endif
        }else if(abcd == ABCD_B){
            imageCopyToRotated();
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
        if(!(abcd == ABCD_NOT_FOUND && arrow == ARROW_NOT_FOUND)){
            show_image(image_rotated);
        }
        Delay_Ms(500);
    }
}
