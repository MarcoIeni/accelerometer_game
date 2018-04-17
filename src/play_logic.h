#ifndef _PLAY_LOGIC_H
#define _PLAY_LOGIC_H

#define SLEEP_MS 300
#define MIN_PLAYERS 2
#define MAX_PLAYERS 4

unsigned player_select();
void celebrate(unsigned player);
int play(unsigned players);
float get_elapsed_time();

#endif
