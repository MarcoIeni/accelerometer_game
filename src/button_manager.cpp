#include "button_manager.h"
#include <miosix.h>

using namespace miosix;

void button_interrupt_on() {
  button::mode(Mode::INPUT_PULL_DOWN);
  EXTI->IMR |= EXTI_IMR_MR0;
  EXTI->RTSR |= EXTI_RTSR_TR0;
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI0_IRQn, 15);
}

void button_interrupt_off() { NVIC_DisableIRQ(EXTI0_IRQn); }
