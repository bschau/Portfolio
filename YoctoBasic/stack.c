#include "yoctobasic.h"

#define STACKSIZE (2048)

struct stack_line {
	char *ip;
	int line_number;
	struct stack_item item;
};

static struct stack_line stack[STACKSIZE];
static int stack_ptr = -1;

/**
 * Reset stack.
 */
void stack_reset(void)
{
	stack_ptr = -1;
}

/**
 * Push item to the stack.
 */
void stack_push(void *item)
{
	stack_ptr++;
	if (stack_ptr >= STACKSIZE) {
		error("stack exhausted");
	}

	stack[stack_ptr].ip = ip;
	stack[stack_ptr].line_number = line_number;
	memmove(&stack[stack_ptr].item, item, sizeof(struct stack_item));
}

/**
 * Pop item from stack.
 */
void stack_pop(void *item)
{
	stack_peek(item);
	stack_ptr--;
}

/**
 * Peek top item.
 */
void stack_peek(void *item)
{
	if (stack_ptr < 0) {
		error("stack underflow in peek");
	}

	ip = stack[stack_ptr].ip;
	line_number = stack[stack_ptr].line_number;
	memmove(item, &stack[stack_ptr].item, sizeof(struct stack_item));
}
