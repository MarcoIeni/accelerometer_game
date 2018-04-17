#ifndef _BUTTON_MANAGER_H
#define _BUTTON_MANAGER_H
#include <miosix.h>

using namespace miosix;

typedef Gpio<GPIOA_BASE, 0> button;

void button_interrupt_on();
void button_interrupt_off();

#endif
