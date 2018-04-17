#include "timer_logic.h"
#include "interrupt_manager.h"
#include <cstdio>
#include <miosix.h>

#define SLEEP_MS 300

using namespace miosix;

float getElapsedTime() {
  Timer timer0;
  wait_for_interrupt();
  printf("Timer starting\n");
  timer0.start();
  Thread::sleep(SLEEP_MS);
  wait_for_interrupt();
  timer0.stop();
  printf("Timer stopped\n");
  Thread::sleep(SLEEP_MS);
  float time = timer0.interval();
  time /= TICK_FREQ;
  timer0.clear();
  printf("time:%0.3fs\n", time);
  return time;
}
