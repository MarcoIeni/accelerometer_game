#include "led_manager.h"
#include <miosix.h>

#define BLINK_INTERVAL 200
#define TOGGLE_INTERVAL 200

using namespace miosix;

void led_init() {
  led_g::mode(Mode::OUTPUT);
  led_o::mode(Mode::OUTPUT);
  led_r::mode(Mode::OUTPUT);
  led_b::mode(Mode::OUTPUT);
}

void switch_off_all() {
  led_g::low();
  led_o::low();
  led_r::low();
  led_b::low();
}

void player_blink(int player, int times) {
  for (int i = 0; i < times; ++i) {
    player_on(player);
    Thread::sleep(BLINK_INTERVAL);
    player_off(player);
    Thread::sleep(BLINK_INTERVAL);
  }
}

void players_off(int first, int last) {
  for (int i = first; i <= last; i++) {
    player_off(i);
  }
}

void player_off(int player) {
  switch (player) {
  case 0:
    led_o::low();
    break;
  case 1:
    led_b::low();
    break;
  case 2:
    led_r::low();
    break;
  case 3:
    led_g::low();
    break;
  }
}

void player_on(int player) {
  switch (player) {
  case 0:
    led_o::high();
    break;
  case 1:
    led_b::high();
    break;
  case 2:
    led_r::high();
    break;
  case 3:
    led_g::high();
    break;
  }
}

void toggle_all() {
  switch_off_all();
  for (unsigned int i = 7; i <= 15; ++i) {
    led_o::high();
    Thread::sleep(TOGGLE_INTERVAL / i);
    led_o::low();
    Thread::sleep(TOGGLE_INTERVAL / i);
    led_r::high();
    Thread::sleep(TOGGLE_INTERVAL / i);
    led_r::low();
    Thread::sleep(TOGGLE_INTERVAL / i);
    led_b::high();
    Thread::sleep(TOGGLE_INTERVAL / i);
    led_b::low();
    Thread::sleep(TOGGLE_INTERVAL / i);
    led_g::high();
    Thread::sleep(TOGGLE_INTERVAL / i);
    led_g::low();
    Thread::sleep(TOGGLE_INTERVAL / i);
  }
}
