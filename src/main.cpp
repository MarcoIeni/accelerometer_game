#include "miosix.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int player_select();
void play(int players);

int main() {
  int players = player_select();
  play(players);
  miosix::Gpio<GPIOA_BASE, 15> ledr;

  return 0;
}

int player_select() { return 0; }

void play(int players) { return; }
