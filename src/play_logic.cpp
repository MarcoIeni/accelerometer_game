#include "play_logic.h"
#include "interrupt_manager.h"
#include "led_manager.h"
#include <miosix.h>
#include <stdio.h>

using namespace miosix;

bool click_flag = false;

static float abs(float val) { return val >= 0 ? val : -val; }

unsigned player_select() {
  unsigned num_players = MIN_PLAYERS;

  // initialize leds to minimum number of players
  for (unsigned i = 0; i < MIN_PLAYERS; i++) {
    player_on(i);
  }

  while (1) {
    wait_for_interrupt();
    if (click_flag) {
      switch_off_all();
      printf("%d players selected\n", num_players);
      return num_players;
    }
    if (num_players < MAX_PLAYERS) {
      player_on(num_players);
      num_players++;
    } else {
      num_players = MIN_PLAYERS;
      players_off(MIN_PLAYERS, MAX_PLAYERS - 1);
    }
    Thread::sleep(SLEEP_MS);
  }
}

int play(unsigned players) {
  float *times = new float[players];
  printf("Waiting for game to start\n");
  wait_for_interrupt();
  Thread::sleep(SLEEP_MS);

  for (;;) {
    for (unsigned i = 0; i < players; ++i) {
      player_on(i);
      printf("Player %d, let's play\n", i + 1);
      times[i] = abs(get_elapsed_time() - 5);
      player_off(i);
    }

    unsigned winner = 0;
    for (unsigned i = 1; i < players; ++i) {
      if (times[i] < times[winner])
        winner = i;
    }
    celebrate(winner);
  }
  return 0;
}

void celebrate(unsigned player) {
  printf("And the winner is %d\n", player + 1);
  switch_off_all();
  player_blink(player, 7);
  toggle_all();
}

float get_elapsed_time() {
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
