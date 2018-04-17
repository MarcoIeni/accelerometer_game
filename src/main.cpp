#include "interrupt_manager.h"
#include "led_manager.h"
#include "lis3dsh_manager.h"
#include "play_logic.h"
#include <miosix.h>
#include <stdio.h>

#define SLEEP_SEC 300

using namespace miosix;

int main() {

  led_init();

  spi_init();
  LIS3DSH_init();
  accelerometer_interrupts_enable();
  LIS3DSH_interrupt_config();
  LIS3DSH_click_sm_config();

  button_interrupts_enable();
  printf("Select number of players using button\n");
  unsigned players = player_select();
  button_interrupts_disable();

  printf("Play using tap\n");
  play(players);

  return 0;
}
