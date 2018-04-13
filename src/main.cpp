#include "lis3dsh_usage.h"
#include "lis3dsh_sync.h"
#include <miosix.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

using namespace miosix;

typedef Gpio<GPIOD_BASE, 12> led4; // green
typedef Gpio<GPIOD_BASE, 13> led2; // orange
typedef Gpio<GPIOD_BASE, 14> led3; // red
typedef Gpio<GPIOD_BASE, 15> led1; // blue

int player_select();
void play(int players);

int main() {
  int players = player_select();
  play(players);

  return 0;
}

int player_select() { return 0; }

void delay() {
  for (int i = 0; i < 500; ++i) asm volatile ("nop");
}

void test_who_am_i() {
  printf("Test\n");
  delay();
  unsigned char c = spi_get_data(0x0F);
  printf("%x\n", c);
}

void play(int players) {
  // Simple first trial

  led1::mode(Mode::OUTPUT);
  led2::mode(Mode::OUTPUT);
  led3::mode(Mode::OUTPUT);
  led4::mode(Mode::OUTPUT);


  int numClicks = 1;
  led1::high();

  spi_init();
  LIS3DSH_init();
  LIS3DSH_interrupt_config();

  test_who_am_i();
  //LIS3DSH_click_int_config();
  for (;;) {
    waitForClick();
    numClicks++;
    if (numClicks == 2)
      led2::high();
    else if (numClicks == 3)
      led3::high();
    else if (numClicks == 4)
      led4::high();
    else {
      numClicks = 1;
      led2::low();
      led3::low();
      led4::low();
    }
  }
  return;
}
