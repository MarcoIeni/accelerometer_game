#include "player_select.h"
#include "timer_logic.h"
#include <miosix.h>
#include <pthread.h>
#include <stdio.h>

using namespace miosix;

int play(int players);

int main() {
  // int players = player_select();
  int players = 2;
  play(players);

  return 0;
}

float abs(float val) {
  return val >= 0 ? val : -val;
}

int play(int players) {
  float *times = new float[players];
  for (;;) {
    for (int i = 0; i < players; ++i) {
      printf("Player %d, let's play\n", i);
      times[i] = abs(getElapsedTime() - 5);
      printf("%f\n", times[i]);
    }

    int min = 0;
    for (int i = 1; i < players; ++i) {
      if (times[i] < times[min])
        min = i;
    }

    printf("And the winner is %d\n", min);
  }
  return 0;
}
