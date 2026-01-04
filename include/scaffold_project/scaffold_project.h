/* scaffold_project.h - Main API header for scaffold_project library */

/* All Rights Reserved */

#ifndef INC_SCAFFOLD_PROJECT_H
#define INC_SCAFFOLD_PROJECT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include "scaffold_project_types.h"
#include <stdbool.h>
#include <stddef.h>

/* Configurations */


/* Definitions */


/* Types */


/* External Declarations */

/**
 * @brief Get library version string
 *
 * @return Version string in format "major.minor.patch"
 */
const char *scaffold_project_get_version(void);

/**
 * @brief Add two integers
 *
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 */
int32_t scaffold_project_add(int32_t a, int32_t b);

/**
 * @brief Multiply two integers with error handling
 *
 * @param a First operand
 * @param b Second operand
 * @param result Pointer to store result
 * @return SCAFFOLD_PROJECT_SUCCESS on success, error code otherwise
 */
scaffold_project_status_t scaffold_project_multiply(
    int32_t a,
    int32_t b,
    int32_t *result);

/**
 * @brief Example function that processes a string
 *
 * @param input Input string to process
 * @param output Buffer to store processed string
 * @param outputSize Size of output buffer
 * @return SCAFFOLD_PROJECT_SUCCESS on success, error code otherwise
 */
scaffold_project_status_t scaffold_project_foo(
    const char *input,
    char *output,
    size_t outputSize);

/**
 * @brief Example function that validates input
 *
 * @param value Value to validate
 * @return true if valid, false otherwise
 */
bool scaffold_project_bar(int32_t value);

/**
 * @brief Compute factorial of a number
 *
 * @param n Input number (must be >= 0 and <= 12)
 * @return Result structure with factorial value and status
 */
scaffold_project_result_t scaffold_project_factorial(int32_t n);

#ifdef __cplusplus
}
#endif

#endif /* INC_SCAFFOLD_PROJECT_H */