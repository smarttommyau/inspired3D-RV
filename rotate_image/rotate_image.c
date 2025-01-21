#define USE_STL_FILE // when using stl file

#define CH32V003_I2C_IMPLEMENTATION // using i2c
#include "ch32v003fun.h"
#define WAIT_WS_CONNECT // emulator wait for ws connection
#define CLOSE_WS_ON_CLOSE // close emulator when connection is closed
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
// assume 5x5x5
#define CENTER_X 2
#define CENTER_Y 2
#define CENTER_Z 2
struct coords{
    float x;
    float y;
    float z;
};
#define NUM_NODES 25
struct coords image[NUM_NODES];
#endif
struct coords image_rotated[NUM_NODES];

#ifndef USE_STL_FILE
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
#endif
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
    printf("show image\n");
}


#define cos30 0.86602540378
#define sin30 0.5
#define cos60 sin30
#define sin60 cos30
#define cos90 0
#define sin90 1
#define cos120 -sin30
#define sin120 cos30
#define cos150 -cos30
#define sin150 sin30
#define cos180 -1
#define sin180 0

float my_cos(int degree){
    if(degree < 0){
        return my_cos(-degree);
    }
    switch(degree){
        case 30:
            return cos30;
            break;
        case 60:
            return cos60;
            break;
        case 90:
            return cos90;
            break;
        case 120:
            return cos120;
            break;
        case 150:
            return cos150;
            break;
        case 180:
            return cos180;
            break;
        default:
            return 0;
    }

}

float my_sin(int degree){
    if(degree < 0){
        return -my_sin(-degree);
    }
    switch(degree){
        case 30:
            return sin30;
            break;
        case 60:
            return sin60;
            break;
        case 90:
            return sin90;
            break;
        case 120:
            return sin120;
            break;
        case 150:
            return sin150;
            break;
        case 180:
            return sin180;
            break;
        default:
            return 0;
    }
}

void rotate(int rotate_vector[2]){
    // dir 0: up
    // dir 1: down
    // dir 2: left
    // dir 3: right
    // each iterate by 30 degree
    // cos(30) = 0.86602540378
    // sin(30) = 0.5
    // center 2,2
    // note: change to calculate once only
    // prevent error increase when rotate multiple times
    int rotatey = rotate_vector[0];
    int rotatex = rotate_vector[1];
    printf("rotatey: %d, rotatex: %d\n", rotatey, rotatex);
    for(uint8_t i=0;i<NUM_NODES;i++){
        float newx,newy,newz,cos_value,sin_value;
        if(rotatey != 0){
            cos_value = my_cos(rotatey*30);
            sin_value = my_sin(rotatey*30);
            newz = (image[i].z - CENTER_Z)*cos_value - (image[i].y - CENTER_Y)*sin_value;
            newy = (image[i].z - CENTER_Z)*sin_value + (image[i].y - CENTER_Y)*cos_value + CENTER_Y;
            printf("newz: %d, newy: %d\n", (int)newz, (int)newy);
            printf("cos: %d.%d, sin: %d.%d\n", (int)cos_value, (int)((cos_value - (int)cos_value)*100), (int)sin_value, (int)((sin_value - (int)sin_value)*100));
        }
        else{
            newz = image[i].z - CENTER_Z;
            newy = image[i].y;
        }
        if(rotatex != 0){
            cos_value = my_cos(rotatex*30);
            sin_value = my_sin(rotatex*30);
            newz = newz*cos_value - (image[i].x - CENTER_X)*sin_value + CENTER_Z;
            newx = newz*sin_value + (image[i].x - CENTER_X)*cos_value + CENTER_X;
        }
        else{
            newz = newz + CENTER_Z;
            newx = image[i].x;
        }
        image_rotated[i].x = newx;
        image_rotated[i].y = newy;
        image_rotated[i].z = newz;
        // switch(dir){
        //     case 0:
        //         newz = (image_rotated[i].z - CENTER_Z)*cos30 - (image_rotated[i].y - CENTER_Y)*sin30 + CENTER_Z;
        //         newy = (image_rotated[i].z - CENTER_Z)*sin30 + (image_rotated[i].y - CENTER_Y)*cos30 + CENTER_Y;
        //         image_rotated[i].y = newy;
        //         image_rotated[i].z = newz;
        //         break;
        //     case 1:
        //         newz =  (image_rotated[i].z - CENTER_Z)*cos30 + (image_rotated[i].y - CENTER_Y)*sin30 + CENTER_Z;
        //         newy = -(image_rotated[i].z - CENTER_Z)*sin30 + (image_rotated[i].y - CENTER_Y)*cos30 + CENTER_Y;
        //         image_rotated[i].y = newy;
        //         image_rotated[i].z = newz;
        //         break;
        //     case 2:
        //         newz = (image_rotated[i].z - CENTER_Z)*cos30 - (image_rotated[i].x - CENTER_X)*sin30 + CENTER_Z;
        //         newx = (image_rotated[i].z - CENTER_Z)*sin30 + (image_rotated[i].x - CENTER_X)*cos30 + CENTER_X;
        //         image_rotated[i].x = newx;
        //         image_rotated[i].z = newz;
        //         break;
        //     case 3:
        //         newz =  (image_rotated[i].z - CENTER_Z)*cos30 + (image_rotated[i].x - CENTER_X)*sin30 + CENTER_Z;
        //         newx = -(image_rotated[i].z - CENTER_Z)*sin30 + (image_rotated[i].x - CENTER_X)*cos30 + CENTER_X;
        //         image_rotated[i].x = newx;
        //         image_rotated[i].z = newz;
        //         break;

        // }
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
    int rotate_vector[2] = {0,0};
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
        }else if(abcd == ABCD_C){
            //decrease brightness
            Inspire3D_Display_SetBrightness(display, display->brightness - 0.1);
        }else if(abcd == ABCD_D){
            //increase brightness
            Inspire3D_Display_SetBrightness(display, display->brightness + 0.1);
        }
        if(arrow == ARROW_UP){
            rotate_vector[0] += 1;
            if(rotate_vector[0] >= 6)
                rotate_vector[0] -= 12;
            rotate_vector[0] %= 3*2+1;// 3*30 = 90 180 deg 6*30
            rotate(rotate_vector);
        }else if(arrow == ARROW_DOWN){
            rotate_vector[0] -= 1;
            if(rotate_vector[0] < -6)
                rotate_vector[0] += 12;
            rotate(rotate_vector);
        }else if(arrow == ARROW_LEFT){
            rotate_vector[1] += 1;
            if(rotate_vector[1] > 6)
                rotate_vector[1] -= 12;
            rotate(rotate_vector);
        }else if(arrow == ARROW_RIGHT){
            rotate_vector[1] -= 1;
            if(rotate_vector[1] < -6)
                rotate_vector[1] += 12;
            rotate(rotate_vector);
        }
        if(!(abcd == ABCD_NOT_FOUND && arrow == ARROW_NOT_FOUND)){
            show_image(image_rotated);
        }
        Delay_Ms(500);
    }
}
