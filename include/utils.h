#ifndef __CA_UTILS_H__
#define __CA_UTILS_H__

#include <stdarg.h>
#include <unistd.h>
#include "capanalysis.h"

/* Replaces system() with a fork+execvp implementation.
 * argv must be NULL terminated.
 */
int run_command(const char *path, char *const argv[]);

/* Safely formats a string into a buffer.
 * Returns the number of characters printed (excluding null byte).
 */
int safe_snprintf(char *str, size_t size, const char *format, ...);

/* Computes MD5 and SHA1 of a file using OpenSSL EVP API.
 * Returns 0 on success, -1 on failure.
 */
int compute_hashes(const char *path, char *md5_out, char *sha1_out);

/* Copy file from src to dst. */
int copy_file(const char *src, const char *dst);

#endif
