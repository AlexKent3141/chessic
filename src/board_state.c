#include "board_state.h"
#include "assert.h"
#include "string.h"

#define INITIAL_STACK_SIZE 255

struct StateStack* CreateStack()
{
    struct StateStack* stack = malloc(sizeof(struct StateStack));
    struct BoardState* bs;

    stack->data = malloc(INITIAL_STACK_SIZE*sizeof(struct BoardState));
    stack->dataSize = INITIAL_STACK_SIZE;
    stack->head = 0;

    /* Initialise a default board state. */
    bs = &stack->data[0];
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
    struct StateStack* copy = malloc(sizeof(struct StateStack));
    size_t i;

    copy->data = malloc(other->dataSize*sizeof(struct BoardState));
    copy->dataSize = other->dataSize;
    copy->head = other->head;

    memcpy(copy->data, other->data, copy->dataSize*sizeof(struct BoardState));

    /* Sort out pointers: currently they will point to the original's elements. */
    for (i = 1; i <= copy->head; i++)
    {
        copy->data[i].previousState = &copy->data[i-1];
    }

    return copy;
}

/* Push a new board state to the stack and return a pointer to the new element. */
/* If the current maximum stack size has been reached then the stack gets
   re-allocated with a larger buffer. When this happens any pointers to the
   previous elements become invalid - so they should not be accessed after
   this function is called. */
struct BoardState* Push(struct StateStack* stack)
{
    struct BoardState* origData;
    size_t origSize, i;

    if (stack->head < stack->dataSize - 1)
    {
        ++stack->head;
        stack->data[stack->head].previousState = &stack->data[stack->head - 1];
    }
    else
    {
        origData = stack->data;
        origSize = stack->dataSize;
        stack->dataSize <<= 1;
        stack->data = malloc(stack->dataSize*sizeof(struct BoardState));
        memcpy(stack->data, origData, origSize*sizeof(struct BoardState));
        free(origData);

        ++stack->head;

        for (i = 1; i <= stack->head; i++)
        {
            stack->data[i].previousState = &stack->data[i-1];
        }
    }

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
