#ifndef _LED_MANAGER_H
#define _LED_MANAGER_H

#include <miosix.h>

using namespace miosix;

typedef Gpio<GPIOD_BASE, 12> led_g; // player 4
typedef Gpio<GPIOD_BASE, 13> led_o; // player 1
typedef Gpio<GPIOD_BASE, 14> led_r; // player 3
typedef Gpio<GPIOD_BASE, 15> led_b; // player 2

void led_init();
void switch_off_all();
void player_on(int player);
void player_off(int player);
void player_blink(int player, int times);
void toggle_all();

#endif
