typedef void* label;

#define GOTO(target) asm("jmp *%0" \
                         : /* No output registers */ \
						 : "r" (target) );

#define SET_STACK_AND_GOTO(sourceStack, targetStack, targetJumpAddress) \
	asm volatile("movl %%ebp, %0;"     \
	             "pusha;"              \
		         "movl %2, %%eax;"     \
	             "movl %1, %%ebp;"     \
	             "jmp *%%eax;"         \
	             : "=mx" (sourceStack) \
	             : "r"  (targetStack), "r" (targetJumpAddress) \
	             : "memory");  // "memory" => clobber ALL registers. This is on purpose, so that no leftovers are used after the goto.
	
#define RESTORE_STACK_AND_GOTO(targetStack, targetJumpAddress) \
	asm volatile ("movl %1, %%eax;"    \
	              "movl %0, %%ebp;"    \
	              "jmp *%%eax;"        \
	              "popa;"              \
	              :                    \
	              : "r" (targetStack), "r" (targetJumpAddress) \
	              : "memory", "eax", "ebp");
	
#define SET_STACK_BASE(target)     asm("movl %%ebp, %0": "=r" (target));
