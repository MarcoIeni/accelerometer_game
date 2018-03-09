#include "lis3dsh_sync.h"
#include <kernel/scheduler/scheduler.h>
#include <miosix.h>

using namespace miosix;

// global var because interrupts can't have parameters or returning value. SO in
// order for interrupts to access sth we need to use a glob var.
static Thread *waiting = 0;

/**
 *  \brief Wait for the click event
 *
 *  \return void
 */
void waitForClick() {
  // This is a miosix class that disable interrupts in the
  // constructors and reenable them in the destructor (so even in case of
  // exceptions). In this way we create a critical section.
  FastInterruptDisableLock dLock;
  // Returns a pointer to the current executing thread
  waiting = Thread::IRQgetCurrentThread();
  while (waiting) {
    // IRQxxx is a miosix naming convention saying that the function should be
    // called w/ interrupts disabled
    // This specific function informs the OS that this thread is no longer
    // capable to have CPU time.
    Thread::IRQwait();
    // The interrupt is enabled till the end of the scope
    FastInterruptEnableLock eLock(dLock);
    // Tells the scheduler to force a context switch. If no threads are in
    // execution it wakes up the idle thread. This thread is not executed until
    // it's woken up.
    Thread::yield();
  }
}

/**
 *  \brief Save the context (i.e. registers) in the TCB for the currently
 *  executing thread, then invoke the interrupt handler and lastly restore the
 *  context.
 *
 *  A naked function is a non-standard function having no pro/epilogue. It ends
 *  in a context (a thread executing) and resume in another context. For naked
 *  functions the compiler doesn't save the context and restore it at the end,
 *  therefore the OS needs to explicitly do it.
 *  \return void
 */
void __attribute__((naked)) EXTI1_IRQHandler() {
  // It ends up in the table of interrupts
  // A naked function is a non-standard function having no pro/epilogue. It ends
  // in a context (a thread executing) and resume in another context. In naked
  // function you can only write assembly instructions.

  // It's a macro w/ assembly instructions that save the registers in the TCB
  // for the currently executing 3d.
  saveContext();
  // branch and link to call the following function (remember the naming
  // convention)
  asm volatile("bl _Z16EXTI1HandlerImplv");
  restoreContext();
}
/**
 *  \brief The interrupt handler function
 *
 *  \return void
 */
void __attribute__((used)) EXTI1HandlerImpl() {
  // Clear the pending flag.
  EXTI->PR = EXTI_PR_PR1;

  // If it's still 0, I've clicked but noone is waiting for it.
  if (waiting == 0)
    return;
  // If there's a thread waiting for this event, we wake up
  waiting->IRQwakeup();
  // If this 3d has a higher priority than the priority of the current running
  // thread, we force the scheduler to pick it. Otw it will be executed sooner
  // or later.
  if (waiting->IRQgetPriority() >
      Thread::IRQgetCurrentThread()->IRQgetPriority())
    // Force the scheduler to do the context switch executing the higher
    // priority thread
    Scheduler::IRQfindNextThread();
  // We reset to 0 bcz of the while loop in the waitForButton
  waiting = 0;
}
