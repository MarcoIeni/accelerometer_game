#include "interrupt_manager.h"
#include <kernel/scheduler/scheduler.h>
#include <miosix.h>

using namespace miosix;

static Thread *waiting = 0;

/**
 *  \brief Manage the interrupt request on line 0 (the button one)
 *
 *  This function ends up in the table of interrupts.
 *  It's a naked function because we want to manage by ourselves the context switch.
 */
void __attribute__((naked)) EXTI0_IRQHandler() {
  saveContext();
  // branch and link to call the EXTI0HandlerImpl() function
  asm volatile("bl _Z16EXTI0HandlerImplv");
  restoreContext();
}

/**
 *  \brief Manage the interrupt request on line 1 (the accelerometer one)
 *
 *  This function ends up in the table of interrupts.
 *  It's a naked function because we want to manage by ourselves the context switch.
 */
void __attribute__((naked)) EXTI1_IRQHandler() {
  saveContext();
  // branch and link to call the EXTI1HandlerImpl() function
  asm volatile("bl _Z16EXTI1HandlerImplv");
  restoreContext();
}

/**
 *  \brief This function is called when the button interrupt is triggered and it
 *         wakes up the thread that was waiting for this event.
 */
void __attribute__((used)) EXTI0HandlerImpl() {
  EXTI->PR = EXTI_PR_PR0; // Clear the pending flag.

  /*
   * If it's still 0, I've pressed the button but no one is waiting for this event,
   * meaning no one called wait_for_interrupt().
   */
  if (waiting == 0)
    return;
  // If there is a thread waiting for this event, I wake up it.
  waiting->IRQwakeup();
  // If this thread has a higher priority than the priority of the current
  // running thread, we force the scheduler to pick it.
  // Otherwise it will be executed sooner or later.
  if (waiting->IRQgetPriority() >
      Thread::IRQgetCurrentThread()->IRQgetPriority())
    Scheduler::IRQfindNextThread();
  // We reset to 0 because of the while loop in the wait_for_interrupt()
  waiting = 0;
}

/**
 *  \brief This function is called when the accelerometer interrupt is triggered and it
 *         wakes up the thread that was waiting for this event.
 */
void __attribute__((used)) EXTI1HandlerImpl() {
  EXTI->PR = EXTI_PR_PR1; // Clear the pending flag.

  /*
   * If it's still 0, I've tapped on the board but no one is waiting for this event,
   * meaning no one called wait_for_interrupt().
   */
  if (waiting == 0)
    return;
  // If there is a thread waiting for this event, I wake up it.
  waiting->IRQwakeup();
  // If this thread has a higher priority than the priority of the current
  // running thread, we force the scheduler to pick it.
  // Otherwise it will be executed sooner or later.
  if (waiting->IRQgetPriority() >
      Thread::IRQgetCurrentThread()->IRQgetPriority())
    Scheduler::IRQfindNextThread();
  click_flag = true;
  // We reset to 0 because of the while loop in the wait_for_interrupt()
  waiting = 0;
}

void accelerometer_interrupts_enable() {
  EXTI->IMR |= EXTI_IMR_MR1;
  // listen to raising edge trigger on interrupt line 1
  EXTI->RTSR |= EXTI_RTSR_TR1;
  // configure the interrupt controller in order to pass the interrupt request
  // up to the CPU
  NVIC_EnableIRQ(EXTI1_IRQn);
  // Set the interrupt priority
  NVIC_SetPriority(EXTI1_IRQn, ACCELEROMETER_PRIORITY);
}

void button_interrupts_enable() {
  EXTI->IMR |= EXTI_IMR_MR0;
  // listen to raising edge trigger on interrupt line 1
  EXTI->RTSR |= EXTI_RTSR_TR0;
  // configure the interrupt controller in order to pass the interrupt request
  // up to the CPU
  NVIC_EnableIRQ(EXTI0_IRQn);
  // Set the interrupt priority
  NVIC_SetPriority(EXTI0_IRQn, BUTTON_PRIORITY);
}

void button_interrupts_disable() {
  NVIC_DisableIRQ(EXTI0_IRQn);
}

void wait_for_interrupt() {
  /* Disable interrupts. This is a miosix class that disables interrupts in the
   * constructors and reenables them in the destructor.
   */
  FastInterruptDisableLock dLock;
  // Return a pointer to the current executing thread
  waiting = Thread::IRQgetCurrentThread();
  while (waiting) {
    // IRQxxx is a miosix naming convention saying that the function should be
    // called with interrupts disabled
    Thread::IRQwait();
    // The interrupts are enabled in the event of the scope
    FastInterruptEnableLock eLock(dLock);
    /*
     * Tells the scheduler to force a context switch.
     * If no threads are in execution it wakes up the idle thread.
     */
    Thread::yield();
  }
}
