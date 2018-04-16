#include "led_manager.h"
#include "player_select.h"
#include "button_sync.h"
#include <miosix.h>

#define SLEEP_MS 300
#define MIN_PLAYERS 2
#define MAX_PLAYERS 4

using namespace miosix;

int player_select() {
  int numPlayers = MIN_PLAYERS;

  // initialize leds to minimum number of players
  for (int i=0; i<MIN_PLAYERS; i++){
    player_on(i);
  }

  for (;;) {
    waitForButton();
    if(numPlayers < MAX_PLAYERS){
      player_on(numPlayers);
      numPlayers++;
    } else {
      numPlayers = MIN_PLAYERS;
      players_off(MIN_PLAYERS,MAX_PLAYERS-1);
    }
    Thread::sleep(SLEEP_MS);
  }

  return numPlayers;
}
