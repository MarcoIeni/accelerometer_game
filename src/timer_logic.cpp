#include "timer_logic.h"
#include "button.h"
#include <cstdio>
#include <miosix.h>

#define SLEEP_MS 300

using namespace miosix;

float getElapsedTime() {
  configureButtonInterrupt();
  Timer timer0;
  waitForButton();
  timer0.start();
  Thread::sleep(SLEEP_MS);
  waitForButton();
  timer0.stop();
  Thread::sleep(SLEEP_MS);
  float time = timer0.interval();
  time /= TICK_FREQ;
  timer0.clear();
  printf("time:%0.3fs\n", time);
  return time;
}
