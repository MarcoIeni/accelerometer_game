#include "button_sync.h"
#include "interrupt_manager.h"
#include "led_manager.h"
#include "lis3dsh_usage.h"
#include "player_select.h"
#include "timer_logic.h"
#include <miosix.h>
#include <pthread.h>
#include <stdio.h>

#define SLEEP_SEC 300

using namespace miosix;

int play(int players);

int main() {

  led_init();

  spi_init();
  LIS3DSH_init();
  LIS3DSH_interrupt_config();
  LIS3DSH_click_sm_config();

  button_interrupt_config();

  int players = player_select();
  play(players);

  return 0;
}

float abs(float val) { return val >= 0 ? val : -val; }

void celebrate(int player) {
  printf("And the winner is %d\n", player + 1);
  switch_off_all();
  player_blink(player, 10);
  toggle_all();
}

int play(int players) {
  float *times = new float[players];
  for (;;) {
    for (int i = 0; i < players; ++i) {
      player_on(i);
      printf("Player %d, let's play\n", i + 1);
      times[i] = abs(getElapsedTime() - 5);
      printf("%f\n", times[i]);
      player_off(i);
    }

    int win = 0;
    for (int i = 1; i < players; ++i) {
      if (times[i] < times[win])
        win = i;
    }

    celebrate(win);
  }

  return 0;
}
