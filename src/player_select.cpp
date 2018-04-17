#include "player_select.h"
#include "interrupt_manager.h"
#include "led_manager.h"
#include <miosix.h>
#include <stdio.h>

#define SLEEP_MS 300
#define MIN_PLAYERS 2
#define MAX_PLAYERS 4

using namespace miosix;

bool click_flag = false;

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
