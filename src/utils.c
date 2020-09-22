#include "utils.h"
#include "assert.h"
#include "stddef.h"
#include "string.h"

#include "stdio.h"

char* NextToken(
    struct TokenState* state)
{
    while (state->start < state->len
        && state->str[state->start] == '\0')
    {
        ++state->start;
    }

    return state->start >= state->len
        ? NULL
        : &state->str[state->start];
}

char* TokenFirst(
    char* str,
    char delimiter,
    struct TokenState* state)
{
    size_t i;

    assert(state != NULL);

    state->str = str;
    state->start = 0;
    state->len = strlen(state->str);

    /* First pass: replace all instances of delimiter with NUL characters. */
    for (i = 0; i < state->len; i++)
    {
        if (state->str[i] == delimiter)
        {
            state->str[i] = '\0';
        }
    }

    return NextToken(state);
}

char* TokenNext(
    struct TokenState* state)
{
    /* Move to the next NUL character. */
    while (state->start < state->len
        && state->str[state->start] != '\0')
    {
        ++state->start;
    }

    return NextToken(state);
}

char* Token(
    char* str,
    char delimiter,
    struct TokenState* state)
{
    return str
        ? TokenFirst(str, delimiter, state)
        : TokenNext(state);
}
