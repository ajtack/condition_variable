typedef void* label;

#define ADDRESS_OF_LABEL(label_name) &&label_name

#define SET_STACK_AND_PIC_AND_GOTO(targetStack, targetPic, targetJumpAddress) \
	asm volatile("movl %0, %%eax;"     \
	             "movl %1, %%ebp;"     \
	             "movl %2, %%ebx;"     \
	             "jmp *%%eax;"         \
	             :                     \
	             : "r" (targetJumpAddress), "r" (targetStack), "r" (targetPic) \
	             : "memory", "eax"); // We explicitly do not label ebx and ebp.
	
#define RESTORE_STACK_AND_PIC_AND_GOTO(targetStack, targetPic, targetJumpAddress) \
	asm volatile ("movl %0, %%eax;"    \
	              "movl %1, %%ebp;"    \
	              "movl %2, %%ebx;"    \
	              "jmp *%%eax;"        \
	              :                    \
	              : "r" (targetJumpAddress), "r" (targetStack), "r" (targetPic)  \
	              : "memory", "eax"); // We explicitly do not label ebx and ebp.
	
#define STORE_STACK_BASE_AND_PIC(stack_base, pic) \
	asm("movl %%ebp, %0;"                       \
	    "movl %%ebx, %1;"                       \
	    : "=m" (stack_base), "=m" (pic)         \
	    :                                       \
	    : "memory");
