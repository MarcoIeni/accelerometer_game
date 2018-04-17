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
void player_on(unsigned player);
void player_off(unsigned player);
void player_blink(unsigned player, unsigned times);
void toggle_all();
void players_off(unsigned first, unsigned last);

#endif
