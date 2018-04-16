#include "button_usage.h"
#include <miosix.h>

using namespace miosix;

typedef Gpio<GPIOA_BASE, 0> button;

void button_interrupt_config()
{
  button::mode(Mode::INPUT_PULL_DOWN);
  EXTI->IMR |= EXTI_IMR_MR0;
  EXTI->RTSR |= EXTI_RTSR_TR0;
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI0_IRQn,15); //Low priority
}
