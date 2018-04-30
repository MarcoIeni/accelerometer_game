#ifndef _INTERRUPT_MANAGER_H
#define _INTERRUPT_MANAGER_H

/*!
 * Whenever there is an interrupt this variable is used to understand whether
 * it was generated by the button or by the accelerometer.
 * In fact, if this variable is true, the user tapped on the board.
 */
extern bool click_flag;

/**
 *  \brief This is a blocking function that is woken up when an interrupt is
 *         triggered.
 */
void wait_for_interrupt();

/**
 *  \brief Enables the interrupts generated by the button.
 *
 *  It listens to the rising edge trigger on interrupt line 0.
 */
void button_interrupts_enable();

/**
 *  \brief This function disables the interrupt generated by the button.
 */
void button_interrupts_disable();

/**
 *  \brief Enables the interrupts generated by the accelerometer.
 *
 *  It listens to the rising edge trigger on interrupt line 1.
 */
void accelerometer_interrupts_enable();

#endif
