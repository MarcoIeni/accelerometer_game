#include "lis3dsh_sync.h"
#include <miosix.h>
#include <kernel/scheduler/scheduler.h>

using namespace miosix;

// global var because interrupts can't have parameters or returning value. SO in
// order for interrupts to access sth we need to use a glob var.
static Thread *waiting = 0;

// This pattern is used often. F.e. a scanf().
void waitForClick() {
  // Disable interrupts. This is a miosix class that disable interrupts in the
  // constructors and reenable them in the destructor (so even in case of
  // exceptions)
  FastInterruptDisableLock dLock;
  // Returns a pointer to the current executing thread
  waiting = Thread::IRQgetCurrentThread();
  while (waiting) {
    // IRQxxx is a miosix naming convention saying that the function should be
    // called w/ interrupts disabled 27'
    Thread::IRQwait();
    // The interrupts is enabled in the event of the scope
    FastInterruptEnableLock eLock(dLock);
    // Tells the scheduler to force a context switch. If no 3ds are in execution
    // it wakes up the idle 3d. This 3d is not executed until it's woken up.
    Thread::yield();
  }
}

// It ends up in the table of interrupts
// A naked function is a non-standard function having no pro/epilogue. It ends
// in a context (a thread executing) and resume in another context. In naked
// function u can only write assembly instructions.
void __attribute__((naked)) EXTI1_IRQHandler() {
  // It's a macro w/ assembly instructions that save the registers in the TCB
  // for the currently executing 3d.
  //
  saveContext();
  // branch and link to call the following function (remember the naming
  // convention)
  asm volatile("bl _Z16EXTI1HandlerImplv");
  restoreContext();
}

void __attribute__((used)) EXTI1HandlerImpl() {
  // Clear the pending flag.
  EXTI->PR = EXTI_PR_PR1;

  // If it's still 0, I've pressed the button but noone is waiting for it.
  if (waiting == 0)
    return;
  // If there's a 3d waiting for this event, we wake up
  waiting->IRQwakeup();
  // If this 3d has a higher priority than the priority of the current running
  // thread, we force the scheduler to pick it. Otw it will be executed sooner
  // or later.
  if (waiting->IRQgetPriority() >
      Thread::IRQgetCurrentThread()->IRQgetPriority())
    Scheduler::IRQfindNextThread();
  // We reset to 0 bcz of the while loop in the waitForButton
  waiting = 0;
}
