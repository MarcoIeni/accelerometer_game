#include "button_usage.h"
#include "interrupt_manager.h"
#include "led_manager.h"
#include "lis3dsh_usage.h"
#include "play_logic.h"
#include "timer_logic.h"
#include <miosix.h>
#include <pthread.h>
#include <stdio.h>

#define SLEEP_SEC 300

using namespace miosix;

int main() {

  led_init();

  spi_init();
  LIS3DSH_init();
  LIS3DSH_interrupt_config();
  LIS3DSH_click_sm_config();

  button_interrupt_on();
  int players = player_select();
  button_interrupt_off();

  play(players);

  return 0;
}
