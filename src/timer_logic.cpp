#include "timer_logic.h"
#include "lis3dsh_sync.h"
#include <cstdio>
#include <miosix.h>

#define SLEEP_MS 300

using namespace miosix;

float getElapsedTime() {
  Timer timer0;
  waitForClick();
  timer0.start();
  Thread::sleep(SLEEP_MS);
  waitForClick();
  timer0.stop();
  Thread::sleep(SLEEP_MS);
  float time = timer0.interval();
  time /= TICK_FREQ;
  timer0.clear();
  printf("time:%0.3fs\n", time);
  return time;
}
