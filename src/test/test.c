#include "parser_tests.h"
#include "stdio.h"

int tests_run = 0;

int main()
{
    char* result = all_parser_tests();
    if (result != NULL)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }

    printf("Tests run: %d\n", tests_run);

    return result != NULL;
}
