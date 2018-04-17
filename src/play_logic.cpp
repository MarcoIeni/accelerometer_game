#include "play_logic.h"
#include "interrupt_manager.h"
#include "led_manager.h"
#include <miosix.h>
#include <stdio.h>

#define SLEEP_MS 300
#define MIN_PLAYERS 2
#define MAX_PLAYERS 4

using namespace miosix;

bool click_flag = false;

static float abs(float val) { return val >= 0 ? val : -val; }

int player_select() {
  int numPlayers = MIN_PLAYERS;

  // initialize leds to minimum number of players
  for (int i = 0; i < MIN_PLAYERS; i++) {
    player_on(i);
  }

  while (1) {
    wait_for_interrupt();
    printf("Main1: %d\n", click_flag);
    if (click_flag) {
      printf("Main2: %d\n", click_flag);
      switch_off_all();
      return numPlayers;
    }
    printf("Main3: %d\n", click_flag);
    if (numPlayers < MAX_PLAYERS) {
      player_on(numPlayers);
      numPlayers++;
    } else {
      numPlayers = MIN_PLAYERS;
      players_off(MIN_PLAYERS, MAX_PLAYERS - 1);
    }
    Thread::sleep(SLEEP_MS);
  }
}

int play(int players) {
  float *times = new float[players];
  printf("Waiting for game to start\n");
  wait_for_interrupt();
  Thread::sleep(SLEEP_MS);

  for (;;) {
    for (int i = 0; i < players; ++i) {
      player_on(i);
      printf("Player %d, let's play\n", i + 1);
      times[i] = abs(get_elapsed_time() - 5);
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

void celebrate(int player) {
  printf("And the winner is %d\n", player + 1);
  switch_off_all();
  player_blink(player, 10);
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
