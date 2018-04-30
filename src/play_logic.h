/**
 *   \file play_logic.h
 *   \brief The logic of the game.
 */
#ifndef _PLAY_LOGIC_H
#define _PLAY_LOGIC_H

#define SLEEP_MS 300
#define MIN_PLAYERS 2
#define MAX_PLAYERS 4

/**
 *  \brief Allow the selection of the number of players for the game using the
 * onboard button
 *  \return the number of players selected.
 */
unsigned player_select();

/**
 *  \brief Celebrate the winner of the game.
 *  \param player The winning player
 */
void celebrate(unsigned player);
/**
 *  \brief Starts the game with a given number of players.
 *  \param players The number of players joining the game.
 */
void play(unsigned players);

#endif
