#include "board_state.h"
#include "assert.h"
#include "string.h"

#define INITIAL_STACK_SIZE 255

struct StateStack* CreateStack()
{
    struct StateStack* stack = malloc(sizeof(struct StateStack));
    stack->data = malloc(INITIAL_STACK_SIZE*sizeof(struct BoardState));
    stack->dataSize = INITIAL_STACK_SIZE;
    stack->head = 0;

    /* Initialise a default board state. */
    struct BoardState* bs = &stack->data[0];
    memset(bs, 0, sizeof(struct BoardState));
    bs->enPassentIndex = CSC_BAD_LOC;
    bs->previousState = NULL;
    bs->hash = 0;

    return stack;
}

void FreeStack(struct StateStack* stack)
{
    if (stack != NULL)
    {
        if (stack->data != NULL)
        {
            free(stack->data);
            stack->data = NULL;
        }

        free(stack);
        stack = NULL;
    }
}

struct StateStack* CopyStack(struct StateStack* other)
{
    struct StateStack* copy = CreateStack();
    copy->dataSize = other->dataSize;
    copy->head = other->head;

    /* Free the allocated data and re-allocate the correct amount. */
    free(copy->data);
    copy->data = malloc(copy->dataSize*sizeof(struct BoardState));
    memcpy(copy->data, other->data, copy->dataSize*sizeof(struct BoardState));

    /* Sort out pointers: currently they will point to the original's elements. */
    for (size_t i = 1; i <= copy->head; i++)
    {
        copy->data[i].previousState = &copy->data[i-1];
    }

    return copy;
}

struct BoardState* Push(struct StateStack* stack)
{
    /* TODO: This is where we might need to re-allocate the stack. */
    assert(stack->head < stack->dataSize - 1);
    ++stack->head;
    stack->data[stack->head].previousState = &stack->data[stack->head - 1];

    return &stack->data[stack->head];
}

struct BoardState* Pop(struct StateStack* stack)
{
    assert(stack->head > 0);
    return &stack->data[stack->head--];
}

struct BoardState* Top(struct StateStack* stack)
{
    return &stack->data[stack->head];
}
