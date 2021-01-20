#ifndef MAIN_PARSE_H
#define MAIN_PARSE_H

/**
 * Searches through command line flags for a particular flag's argument.
 *
 * Args:
 *     name: The flag's name, including any hyphens. For example, "-mode".
 *     argc: Number of command line arguments.
 *     argv: Array of command line argument strings.
 *
 * Returns:
 *     A pointer to the command line flag's value string, or else 0 if the flag
 *     is not specified. Flags that are set without specifying a value will
 *     cause the method to return a pointer to an empty string.
 */
const char *find_argument(const char *name, int argc, const char **argv);

/**
 * Searches through command line flags for a particular flag's argument.
 *
 * If the flag is not present, the program is terminated with EXIT_FAILURE.
 *
 * Args:
 *     name: The flag's name, including any hyphens. For example, "-mode".
 *     argc: Number of command line arguments.
 *     argv: Array of command line argument strings.
 *
 * Returns:
 *     A pointer to the command line flag's value string. Flags that are set
 *     without specifying a value will cause the method to return a pointer to
 *     an empty string.
 *
 * Terminates Program:
 *     The argument is not present.
 */
const char *require_find_argument(const char *name, int argc, const char **argv);

/**
 * Checks that all command line arguments are recognized.
 *
 * If the check fails, the program exits with a non-zero return code and prints
 * a message containing the known arguments to stderr.
 *
 * Args:
 *     known_argument_count: Number of known arguments.
 *     known_arguments: Array of known argument names.
 *     argc: Number of command line arguments.
 *     argv: Array of command line argument strings.
 */
void check_for_unknown_arguments(
    int known_argument_count, const char **known_arguments, int argc, const char **argv);

// Returns a floating point value that can be modified using command line arguments.
//
// If default_value is smaller than min_value or larger than max_value, the argument is required.
//
// If the specified value is invalid, the program exits with a non-zero return code and prints
// a message describing the problem.
//
// Args:
//     name: The name of the float flag.
//     default_value: The value to use if the flag is not specified. If this value is less than
//         min_value or larger than max_value, the flag is required.
//     min_value: Values less than this are rejected.
//     max_value: Values more than this are rejected.
//     argc: Number of command line arguments.
//     argv: Array of command line argument strings.
//
// Returns:
//     The floating point value.
//
// Exits:
//     EXIT_FAILURE:
//         The command line flag was specified but failed to parse into a float in range.
//     EXIT_FAILURE:
//         The command line flag was not specified and default_value was not in range.
float find_float_argument(
    const char *name, float default_value, float min_value, float max_value, int argc,
    const char **argv);

// Returns an integer value that can be modified using command line arguments.
//
// If default_value is smaller than min_value or larger than max_value, the argument is required.
//
// If the specified value is invalid, the program exits with a non-zero return code and prints
// a message describing the problem.
//
// Args:
//     name: The name of the int flag.
//     default_value: The value to use if the flag is not specified. If this value is less than
//         min_value or larger than max_value, the flag is required.
//     min_value: Values less than this are rejected.
//     max_value: Values more than this are rejected.
//     argc: Number of command line arguments.
//     argv: Array of command line argument strings.
//
// Returns:
//     The integer value.
//
// Exits:
//     EXIT_FAILURE:
//         The command line flag was specified but failed to parse into an int in range.
//     EXIT_FAILURE:
//         The command line flag was not specified and default_value was not in range.
int find_int_argument(
    const char *name, int default_value, int min_value, int max_value, int argc, const char **argv);

/**
 * Returns a boolean value that can be enabled using a command line argument.
 *
 * If the specified value is invalid, the program exits with a non-zero return code and prints
 * a message describing the problem.
 *
 * Args:
 *     name: The name of the boolean flag.
 *     argc: Number of command line arguments.
 *     argv: Array of command line argument strings.
 *
 * Returns:
 *     The boolean value.
 */
int find_bool_argument(const char *name, int argc, const char **argv);

// Returns the index of an argument value within an enumerated list of allowed values.
//
// Args:
//     name: The name of the enumerated flag.
//     default_index: The value to return if the flag is not specified. Set to -1 if you want the
//         program to exit if the command line argument is not specified.
//     num_known_values: The number of known values that the command line argument might be.
//     known_values: A pointer to the list of known values (in the form of strings) that the
//         command line argument might be.
//     argc: Number of command line arguments.
//     argv: Array of command line argument strings.
//
// Returns:
//     If the command line flag is not specified and default_index is non-negative, the result is
//     default_index.
//
//     If the command line flag is specified and its value is in the enumerated list, the result is
//     the list index where the value was.
//
// Exits:
//     EXIT_FAILURE:
//         The command line flag is specified but its value is not in the enumerated list.
//     EXIT_FAILURE:
//         The command line flag is not specified and default_index is negative.
int find_enum_argument(
    const char *name, int default_index, int num_known_values, const char *const *known_values,
    int argc, const char **argv);

// Returns a cleaned up version of a directory path argument's value.
//
// Note that directories may be relative to the current working directory, but for safety the
// empty string is not permitted. The current working directory can be referred to using "./".
//
// Args:
//     name: The name of the directory flag.
//     default_directory: The default value to use if the flag is not specified. If this value is
//         set to NULL, the flag is required to be present.
//     argc: Number of command line arguments.
//     argv: Array of command line argument strings.
//
// Returns:
//     A directory name formatted such that filenames can be directly concatenated to it.
//     The caller is responsible for calling `free` on the result.
//
// Exits:
//     EXIT_FAILURE:
//         The command line flag was specified to be the empty string.
//     EXIT_FAILURE:
//         The command line flag is not specified and default_directory is NULL.
char *find_directory_argument_malloc(
    const char *name, const char *default_directory, int argc, const char **argv);

#endif