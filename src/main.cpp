#include "player_select.h"
#include <miosix.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

using namespace miosix;

int play(int players);

int main() {
  int players = player_select();
  play(players);

  return 0;
}

int play(int players) { return 0; }
