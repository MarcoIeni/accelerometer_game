#ifndef _INTERRUPT_MANAGER_H
#define _INTERRUPT_MANAGER_H

extern bool click_flag;

void wait_for_interrupt();
void button_interrupts_enable();
void button_interrupts_disable();
void accelerometer_interrupts_enable();

#endif
