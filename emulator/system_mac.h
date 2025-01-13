#ifndef SYSTEM_MAC_H
#define SYSTEM_MAC_H

#include <ApplicationServices/ApplicationServices.h>
#include <pthread.h>
#include <stdbool.h>

#define A_Key 0
#define B_Key 11
#define C_Key 8
#define D_Key 2
#define E_Key 14
#define F_Key 3
#define G_Key 5
#define H_Key 4
#define I_Key 34
#define J_Key 38
#define K_Key 40
#define L_Key 37
#define M_Key 46
#define N_Key 45
#define O_Key 31
#define P_Key 35
#define Q_Key 12
#define R_Key 15
#define S_Key 1
#define T_Key 17
#define U_Key 32
#define W_Key 13
#define X_Key 7
#define Y_Key 16
#define Z_Key 6
#define _0_Key 29
#define _1_Key 18
#define _2_Key 19
#define _3_Key 20
#define _4_Key 21
#define _5_Key 23
#define _6_Key 22
#define _7_Key 26
#define _8_Key 28
#define _9_Key 25
#define _Arrow_Up_Key 126
#define _Arrow_Down_Key 125
#define _Arrow_Left_Key 123
#define _Arrow_Right_Key 124

pthread_t thread;

#define MAX_KEYS 6
CGKeyCode pressedKeys[MAX_KEYS];
int pressedKeyCount_;
static const CGKeyCode ABCD[] = {A_Key, B_Key, C_Key, D_Key};
static const CGKeyCode _0123456789ABCDEF[] = {_0_Key, _1_Key, _2_Key, _3_Key, _4_Key,
    _5_Key, _6_Key, _7_Key, _8_Key, _9_Key, A_Key, B_Key, C_Key, D_Key, E_Key, F_Key};

void pthread_init();

bool is_key_pressed(CGKeyCode keyCode);

#endif