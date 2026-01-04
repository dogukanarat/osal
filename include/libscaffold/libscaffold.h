/* libscaffold.h - Main API header for libscaffold library */

/* All Rights Reserved */

#ifndef INC_LIBSCAFFOLD_H
#define INC_LIBSCAFFOLD_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include "libscaffold_types.h"
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
const char *libscaffold_get_version(void);

/**
 * @brief Add two integers
 *
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 */
int32_t libscaffold_add(int32_t a, int32_t b);

/**
 * @brief Multiply two integers with error handling
 *
 * @param a First operand
 * @param b Second operand
 * @param result Pointer to store result
 * @return LIBSCAFFOLD_SUCCESS on success, error code otherwise
 */
libscaffold_status_t libscaffold_multiply(
    int32_t a,
    int32_t b,
    int32_t *result);

/**
 * @brief Example function that processes a string
 *
 * @param input Input string to process
 * @param output Buffer to store processed string
 * @param outputSize Size of output buffer
 * @return LIBSCAFFOLD_SUCCESS on success, error code otherwise
 */
libscaffold_status_t libscaffold_foo(
    const char *input,
    char *output,
    size_t outputSize);

/**
 * @brief Example function that validates input
 *
 * @param value Value to validate
 * @return true if valid, false otherwise
 */
bool libscaffold_bar(int32_t value);

/**
 * @brief Compute factorial of a number
 *
 * @param n Input number (must be >= 0 and <= 12)
 * @return Result structure with factorial value and status
 */
libscaffold_result_t libscaffold_factorial(int32_t n);

#ifdef __cplusplus
}
#endif

#endif /* INC_LIBSCAFFOLD_H */