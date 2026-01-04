/* scaffold_project.c - Core implementation of scaffold_project library */

/* All Rights Reserved */

/* Includes */

#include "scaffold_project/scaffold_project.h"
#include <stdio.h>
#include <string.h>

/* Imports */


/* Definitions */

#define VERSION_STRING "1.0.0"
#define MAX_FACTORIAL_INPUT 12

/* Types */


/* Forward Declarations */


/* Variables */


/* Functions */

const char *scaffold_project_get_version(void)
{
    return VERSION_STRING;
}

int32_t scaffold_project_add(int32_t a, int32_t b)
{
    return a + b;
}

scaffold_project_status_t scaffold_project_multiply(
    int32_t a,
    int32_t b,
    int32_t *result)
{
    /* Validate input parameter */
    if (result == NULL)
    {
        return SCAFFOLD_PROJECT_ERROR_NULL;
    }

    /* Perform multiplication */
    *result = a * b;

    return SCAFFOLD_PROJECT_SUCCESS;
}

scaffold_project_status_t scaffold_project_foo(
    const char *input,
    char *output,
    size_t outputSize)
{
    /* Validate input parameters */
    if (input == NULL || output == NULL)
    {
        return SCAFFOLD_PROJECT_ERROR_NULL;
    }

    if (outputSize == 0)
    {
        return SCAFFOLD_PROJECT_ERROR_INVALID;
    }

    /* Get input length */
    size_t inputLen = strlen(input);

    /* Check if output buffer is large enough */
    if (inputLen + 10 >= outputSize)
    {
        return SCAFFOLD_PROJECT_ERROR_INVALID;
    }

    /* Process the string by adding a prefix */
    snprintf(output, outputSize, "Processed: %s", input);

    return SCAFFOLD_PROJECT_SUCCESS;
}

bool scaffold_project_bar(int32_t value)
{
    /* Example validation: value must be in range [0, 100] */
    return (value >= 0 && value <= 100);
}

scaffold_project_result_t scaffold_project_factorial(int32_t n)
{
    scaffold_project_result_t result;

    /* Validate input */
    if (n < 0)
    {
        result.value = 0;
        result.status = SCAFFOLD_PROJECT_ERROR_INVALID;
        return result;
    }

    /* Check for overflow protection */
    if (n > MAX_FACTORIAL_INPUT)
    {
        result.value = 0;
        result.status = SCAFFOLD_PROJECT_ERROR_INVALID;
        return result;
    }

    /* Calculate factorial */
    int32_t factorial = 1;
    for (int32_t i = 2; i <= n; i++)
    {
        factorial *= i;
    }

    result.value = factorial;
    result.status = SCAFFOLD_PROJECT_SUCCESS;

    return result;
}
