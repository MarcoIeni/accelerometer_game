#include "button.h"
#include <pthread.h>
#include <miosix.h>
#include <unistd.h>

using namespace miosix;

typedef Gpio<GPIOD_BASE,12> led4; //green
typedef Gpio<GPIOD_BASE,13> led1; //orange
typedef Gpio<GPIOD_BASE,14> led3; //red
typedef Gpio<GPIOD_BASE,15> led2; //blue

int chooseNumPlayers() {
  int numPlayers = 2;

  led1::high();
  led2::high();
  configureButtonInterrupt();

  for(;;)
    {
      waitForButton();
      numPlayers++;
      if (numPlayers == 3)
        led3::high();
      else if (numPlayers == 4)
        led4::high();
      else {
        numPlayers = 2;
        led3::low();
        led4::low();
      }
    }
  return numPlayers; // TODO
}

int player_select()
{
  led1::mode(Mode::OUTPUT);
  led2::mode(Mode::OUTPUT);
  led3::mode(Mode::OUTPUT);
  led4::mode(Mode::OUTPUT);

  chooseNumPlayers();
}
