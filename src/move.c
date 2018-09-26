#include "../include/chessic.h"
#include "stdlib.h"

move create_move(char start, char end)
{
    return (end << 8) + start;
}
