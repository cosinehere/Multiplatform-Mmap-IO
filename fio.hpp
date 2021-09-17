/**
 * @file fio.hpp
 * @author cosine (cosinehit@gmail.com)
 * @brief file i/o
 * @version 0.1
 * @date 2021-09-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#ifndef _FIO_HPP_
#define _FIO_HPP_

#include <cstdio>

#if __cplusplus <= 199711L && (!defined(_MSC_VER) || _MSC_VER < 1900) &&       \
    (!defined(__GNUC__) ||                                                     \
     (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ < 40603))
#ifndef CXX11_NOT_SUPPORT
#define CXX11_NOT_SUPPORT
#endif // CXX11_NOT_SUPPORT
#endif // __cplusplus<=199711L

#ifdef CXX11_NOT_SUPPORT
#define nullptr NULL
#define constexpr const
#define noexcept throw()
#define override
#endif // CXX11_NOT_SUPPORT

namespace fio {

enum enum_fio_mode { enum_mode_read = 0, enum_mode_rdwr };

class fio {
private:
    FILE *p_file;
    unsigned char p_mode;
    bool p_opened;

    inline const bool path_empty(const char *path) { return !path || !(*path); }

public:
    fio() noexcept : p_file(nullptr), p_mode(enum_mode_read), p_opened(false) {}

    ~fio() noexcept { close_file(); }

    bool create_file(const char *path) {
        if (p_opened || path_empty(path)) {
            return false;
        }

        p_file = fopen(path, "wb+");

        if (p_file == nullptr) {
            return false;
        }

        return p_opened = true;
    }

    bool open_file(const char *path, enum_fio_mode mode) {
        if (p_opened || path_empty(path)) {
            return false;
        }

        if (mode == enum_mode_read) {
            p_file = fopen(path, "rb");
        } else if (mode == enum_mode_rdwr) {
            p_file = fopen(path, "wb+");
        }

        if (p_file == nullptr) {
            return false;
        }

        p_mode = mode;

        return p_opened = true;
    }

    void close_file() {
        if (p_opened) {
            fclose(p_file);
            p_opened = false;
        }
    }

    size_t file_size() {
        if (!p_opened) {
            return 0;
        }

        long pos = ftell(p_file);
        fseek(p_file, 0, SEEK_END);
        long len = ftell(p_file);
        fseek(p_file, pos, SEEK_SET);
        return len;
    }

    size_t read_file(void *buffer, size_t readnum) {
        if (!p_opened || buffer == nullptr || !readnum) {
            return 0;
        }

        return fread(buffer, sizeof(char), readnum, p_file);
    }

    size_t write_file(const void *buffer, size_t writenum) {
        if (!p_opened || buffer == nullptr || !writenum) {
            return 0;
        }

        return fwrite(buffer, sizeof(char), writenum, p_file);
    }

    off_t seek_file(int pos, off_t offset) {
        if (!p_opened || p_file == nullptr) {
            return -1;
        }

        fseek(p_file, offset, pos);
        return ftell(p_file);
    }

    bool flush_file() {
        if (!p_opened || p_mode == enum_mode_read) {
            return false;
        }
        return (fflush(p_file) == 0);
    }
};

} // namespace fio

#endif
