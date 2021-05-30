#include "chessic.h"
#include "token_tests.h"
#include "minunit.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

char* TokenTest_StringIsEmpty()
{
    char str[100] = "";
    char* token;
    struct CSC_TokenState state;

    printf("Token test string is empty\n");

    token = CSC_Token(str, 'a', &state);

    mu_assert("There should be no tokens.", token == NULL);

    return NULL;
}

char* TokenTest_EntireStringIsDelimiter()
{
    /* All characters are a space. */
    char str[100] = "          ";
    char* token;
    struct CSC_TokenState state;

    printf("Token test entire string is delimiter\n");

    token = CSC_Token(str, ' ', &state);

    mu_assert("There should be no tokens.", token == NULL);

    return NULL;
}

char* TokenTest_SplitBySpace()
{
    char str[100] = "Saitama wins with one punch";
    char* token;
    struct CSC_TokenState state;

    printf("Token test split by space: 'Saitama wins with one punch'\n");

    token = CSC_Token(str, ' ', &state);

    mu_assert("First token is not 'Saitama'", strcmp(token, "Saitama") == 0);

    token = CSC_Token(NULL, ' ', &state);

    mu_assert("Second token is not 'wins'", strcmp(token, "wins") == 0);

    token = CSC_Token(NULL, ' ', &state);

    mu_assert("Third token is not 'with'", strcmp(token, "with") == 0);

    token = CSC_Token(NULL, ' ', &state);

    mu_assert("Fourth token is not 'one'", strcmp(token, "one") == 0);

    token = CSC_Token(NULL, ' ', &state);

    mu_assert("Fifth token is not 'punch'", strcmp(token, "punch") == 0);

    token = CSC_Token(NULL, ' ', &state);

    mu_assert("Shouldn't have any more tokens", token == NULL);

    return NULL;
}

char* AllTokenTests()
{
    mu_run_test(TokenTest_StringIsEmpty);
    mu_run_test(TokenTest_EntireStringIsDelimiter);
    mu_run_test(TokenTest_SplitBySpace);

    return NULL;
}
