#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <openssl/evp.h>
#include <fcntl.h>
#include <errno.h>
#include "utils.h"
#include "log.h"

int run_command(const char *path, char *const argv[]) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        /* child process */
        /* redirect stdout/stderr to /dev/null if not already handled?
           Original system calls had > /dev/null sometimes.
           We'll leave it to the caller or default to inheriting.
        */
        execvp(path, argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        /* parent process */
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}

int safe_snprintf(char *str, size_t size, const char *format, ...) {
    va_list args;
    int ret;

    va_start(args, format);
    ret = vsnprintf(str, size, format, args);
    va_end(args);
    return ret;
}

static void bytes_to_hex(const unsigned char *bytes, size_t len, char *hex_out) {
    for (size_t i = 0; i < len; i++) {
        sprintf(hex_out + (i * 2), "%02x", bytes[i]);
    }
    hex_out[len * 2] = '\0';
}

int compute_hashes(const char *path, char *md5_out, char *sha1_out) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    EVP_MD_CTX *md5_ctx = EVP_MD_CTX_new();
    EVP_MD_CTX *sha1_ctx = EVP_MD_CTX_new();

    if (!md5_ctx || !sha1_ctx) {
        fclose(f);
        if (md5_ctx) EVP_MD_CTX_free(md5_ctx);
        if (sha1_ctx) EVP_MD_CTX_free(sha1_ctx);
        return -1;
    }

    EVP_DigestInit_ex(md5_ctx, EVP_md5(), NULL);
    EVP_DigestInit_ex(sha1_ctx, EVP_sha1(), NULL);

    unsigned char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), f)) != 0) {
        EVP_DigestUpdate(md5_ctx, buffer, bytes);
        EVP_DigestUpdate(sha1_ctx, buffer, bytes);
    }

    unsigned char md5_res[EVP_MAX_MD_SIZE];
    unsigned char sha1_res[EVP_MAX_MD_SIZE];
    unsigned int md5_len, sha1_len;

    EVP_DigestFinal_ex(md5_ctx, md5_res, &md5_len);
    EVP_DigestFinal_ex(sha1_ctx, sha1_res, &sha1_len);

    EVP_MD_CTX_free(md5_ctx);
    EVP_MD_CTX_free(sha1_ctx);
    fclose(f);

    bytes_to_hex(md5_res, md5_len, md5_out);
    bytes_to_hex(sha1_res, sha1_len, sha1_out);

    return 0;
}

int copy_file(const char *src, const char *dst) {
    int input, output;
    if ((input = open(src, O_RDONLY)) == -1) return -1;
    if ((output = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
        close(input);
        return -1;
    }

    char buf[4096];
    ssize_t bytes;
    while ((bytes = read(input, buf, sizeof(buf))) > 0) {
        if (write(output, buf, bytes) != bytes) {
            close(input);
            close(output);
            return -1;
        }
    }

    close(input);
    close(output);
    return (bytes == -1) ? -1 : 0;
}
