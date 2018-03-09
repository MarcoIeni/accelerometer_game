
#include <miosix.h>
#include <kernel/scheduler/scheduler.h>
#include "button.h"

using namespace miosix;

typedef Gpio<GPIOA_BASE,0> button;

// global var because interrupts can't have parameters or returning value. SO in order
// for interrupts to access sth we need to use a glob var.
static Thread *waiting=0;

// It ends up in the table of interrupts
// A naked function is a non-standard function having no pro/epilogue. It ends in a context (a thread executing) and
// resume in another context.
// In naked function u can only write assembly instructions.
void __attribute__((naked)) EXTI0_IRQHandler()
{
    // It's a macro w/ assembly instructions that save the registers in the TCB for the currently executing 3d.
    // 
    saveContext();
    // branch and link to call the following function (remember the naming convention)
    asm volatile("bl _Z16EXTI0HandlerImplv");
    restoreContext();
}
    
void __attribute__((used)) EXTI0HandlerImpl()
{
    // Clear the pending flag.
    EXTI->PR=EXTI_PR_PR0;

    // If it's still 0, I've pressed the button but noone is waiting for it.
    if(waiting==0) return;
    // If there's a 3d waiting for this event, we wake up
    waiting->IRQwakeup();
    // If this 3d has a higher priority than the priority of the current running thread, we force the scheduler
    // to pick it. Otw it will be executed sooner or later.
    if(waiting->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority())
		Scheduler::IRQfindNextThread();
    // We reset to 0 bcz of the while loop in the waitForButton
    waiting=0;
}

void configureButtonInterrupt()
{
    button::mode(Mode::INPUT_PULL_DOWN);
    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->RTSR |= EXTI_RTSR_TR0;
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_SetPriority(EXTI0_IRQn,15); //Low priority
}

// This pattern is used often. F.e. a scanf(). 
void waitForButton()
{
    // Disable interrupts. This is a miosix class that disable interrupts in the constructors
    // and reenable them in the destructor (so even in case of exceptions)
    FastInterruptDisableLock dLock;
    // Returns a pointer to the current executing thread
    waiting=Thread::IRQgetCurrentThread();
    while(waiting)
    {
	// IRQxxx is a miosix naming convention saying that the function should be called w/ interrupts disabled
	// 27'
        Thread::IRQwait();
	// The interrupts is enabled in the event of the scope
        FastInterruptEnableLock eLock(dLock);
	// Tells the scheduler to force a context switch. If no 3ds are in execution it wakes up the idle 3d.
	// This 3d is not executed until it's woken up.
        Thread::yield();
    }
}
