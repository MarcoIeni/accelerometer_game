#ifndef _LED_MANAGER_H
#define _LED_MANAGER_H

#include <miosix.h>

using namespace miosix;

typedef Gpio<GPIOD_BASE, 12> led_g; // player 4
typedef Gpio<GPIOD_BASE, 13> led_o; // player 1
typedef Gpio<GPIOD_BASE, 14> led_r; // player 3
typedef Gpio<GPIOD_BASE, 15> led_b; // player 2

/**
 *  \brief Initialize all the leds in order to be toggled
 */
void led_init();

/**
 *  \brief Switch off all the leds
 */
void switch_off_all();

/**
 *  \brief Switch on the led of a given player
 *  \param player the player that you want to switch on the led
 */
void player_on(unsigned player);

/**
 *  \brief Switch off the led of a given player
 *  \param player the player that you want to switch off the led
 */
void player_off(unsigned player);

/**
 *  \brief Blink the led of the corresponding player a given number of times
 *  \param player The player that you want to blink the led
 *  \param times How many times you want to blink the led
 */
void player_blink(unsigned player, unsigned times);

/**
 *  \brief Switch off and on all the leds repeatedly
 */
void toggle_all();

/**
 *  \brief Switch off the leds of the given range of players
 *  \param first The first player that you want to switch off the led
 *  \param last The last player that you want to switch off the led
 */
void players_off(unsigned first, unsigned last);

#endif
