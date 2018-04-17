#include "interrupt_manager.h"
#include <kernel/scheduler/scheduler.h>
#include <miosix.h>

using namespace miosix;

static Thread *waiting = 0;

void __attribute__((naked)) EXTI0_IRQHandler() {
  saveContext();
  asm volatile("bl _Z16EXTI0HandlerImplv");
  restoreContext();
}

void __attribute__((naked)) EXTI1_IRQHandler() {
  saveContext();
  asm volatile("bl _Z16EXTI1HandlerImplv");
  restoreContext();
}

void __attribute__((used)) EXTI0HandlerImpl() {
  // Clear the pending flag.
  EXTI->PR = EXTI_PR_PR0;

  // If it's still 0, I've pressed the button but noone is waiting for it.
  if (waiting == 0)
    return;
  // If there's a 3d waiting for this event, we wake up
  waiting->IRQwakeup();
  // If this thread has a higher priority than the priority of the current
  // running thread, we force the scheduler to pick it. Otw it will be executed
  // sooner or later.
  if (waiting->IRQgetPriority() >
      Thread::IRQgetCurrentThread()->IRQgetPriority())
    Scheduler::IRQfindNextThread();
  // We reset to 0 because of the while loop in the wait_for_button
  waiting = 0;
}

void __attribute__((used)) EXTI1HandlerImpl() {
  EXTI->PR = EXTI_PR_PR1;

  if (waiting == 0)
    return;
  waiting->IRQwakeup();
  if (waiting->IRQgetPriority() >
      Thread::IRQgetCurrentThread()->IRQgetPriority())
    Scheduler::IRQfindNextThread();
  click_flag = true;
  waiting = 0;
}

void accelerometer_interrupts_enable() {
  EXTI->IMR |= EXTI_IMR_MR1; //
  // listen to raising edge trigger
  EXTI->RTSR |= EXTI_RTSR_TR1;
  // configure the interrupt controller in order to pass the interrupt request
  // up to the CPU
  NVIC_EnableIRQ(EXTI1_IRQn);
  // Set the interrupt priority to 14 (low)
  NVIC_SetPriority(EXTI1_IRQn, 14);
}

void button_interrupts_enable() {
  EXTI->IMR |= EXTI_IMR_MR0;
  EXTI->RTSR |= EXTI_RTSR_TR0;
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI0_IRQn, 15);
}

void button_interrupts_disable() { NVIC_DisableIRQ(EXTI0_IRQn); }

// This pattern is used often. F.e. a scanf().
void wait_for_interrupt() {
  // Disable interrupts. This is a miosix class that disable interrupts in the
  // constructors and reenable them in the destructor (and so even in case of
  // exceptions)
  FastInterruptDisableLock dLock;
  // Returns a pointer to the current executing thread
  waiting = Thread::IRQgetCurrentThread();
  while (waiting) {
    // IRQxxx is a miosix naming convention saying that the function should be
    // called with interrupts disabled
    Thread::IRQwait();
    // The interrupts is enabled in the event of the scope
    FastInterruptEnableLock eLock(dLock);
    // Tells the scheduler to force a context switch. If no 3ds are in execution
    // it wakes up the idle thread. This thread is not executed until it's woken
    // up.
    Thread::yield();
  }
}
