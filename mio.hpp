#pragma once

#ifndef _MIO_HPP_
#define _MIO_HPP_

#include <cstddef>
#include <cstring>

#ifdef _MSC_VER
#if (_MSC_VER < 1400)
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <cstdint>
#endif  // _MSC_VER < 1400
#else
#include <cstdint>
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// system header
//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)  // Windows
#ifndef _AFX                                              // without MFC
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif  // _AFX
#include <sys/types.h>
#else   // Unix
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif  // _WIN32

//////////////////////////////////////////////////////////////////////////
// C++11 support
//////////////////////////////////////////////////////////////////////////
#if __cplusplus <= 199711L && (!defined(_MSC_VER) || _MSC_VER < 1900) && \
    (!defined(__GNUC__) ||                                               \
     (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ < 40603))
#ifndef CXX11_NOT_SUPPORT
#define CXX11_NOT_SUPPORT
#endif  // CXX11_NOT_SUPPORT
#endif  // __cplusplus<=199711L

#ifdef CXX11_NOT_SUPPORT
#define nullptr NULL
#define constexpr const
#define noexcept throw()
#define override
#endif  // CXX11_NOT_SUPPORT

namespace mio {

//////////////////////////////////////////////////////////////////////////
// mio typedef
//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)  // Windows
typedef HANDLE HMIO;
#ifndef invalid_handle
#define invalid_handle INVALID_HANDLE_VALUE
#endif  // invalid_handle
#define _MIO_WIN
#else  // Unix
#define invalid_handle -1
typedef int HMIO;
#endif  // _WIN32

// read/write mode
enum enum_mio_mode { enum_mode_read = 0, enum_mode_write };

enum enum_mio_pos { enum_pos_set = 0, enum_pos_cur, enum_pos_end };

class mio {
   private:
    HMIO p_file;
    enum_mio_mode p_mode;

    bool path_empty(const char* path) { return !path || (*path == '\0'); }

   public:
    mio();
    ~mio();
#ifndef CXX11_NOT_SUPPORT
    mio(const mio&) = delete;
    mio(const mio&&) = delete;
    mio& operator=(const mio&) = delete;
    mio& operator=(const mio&&) = delete;
#endif  // CXX11_NOT_SUPPORT

    bool is_open() noexcept { return p_file != invalid_handle; }

    bool create_file(const char* path, enum_mio_mode mode);
    bool open_file(const char* path, enum_mio_mode mode);
    void close_file();

    size_t file_size();

    size_t read_file(void* buffer, size_t readnum);
    size_t write_file(const void* buffer, size_t writenum);

    off_t seek_file(enum_mio_pos pos, off_t offset);

    bool file_flush();
};

inline mio::mio() { p_file = invalid_handle; }

inline mio::~mio() {
    if (is_open()) {
        close_file();
    }
}

inline bool mio::create_file(const char* path, enum_mio_mode mode) {
    if (path_empty(path)) {
        return false;
    }

    HMIO handle = invalid_handle;
#ifdef _MIO_WIN
    handle = CreateFile(
        path,
        (mode == enum_mode_read) ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, 0);
#else
    handle = open(
        path, (mode == enum_mode_read) ? O_RDONLY | O_CREAT : O_RDWR | O_CREAT,
        S_IRUSR | S_IWUSR);
#endif  // _MIO_WIN

    p_file = handle;
    p_mode = mode;
    return handle != invalid_handle;
}

inline bool mio::open_file(const char* path, enum_mio_mode mode) {
    if (path_empty(path)) {
        return false;
    }

    HMIO handle = invalid_handle;
#ifdef _MIO_WIN
    handle = CreateFile(
        path,
        (mode == enum_mode_read) ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);
#else
    handle = open(path, (mode == enum_mode_read) ? O_RDONLY : O_RDWR);
#endif  // _MIO_WIN

    p_file = handle;
    p_mode = mode;
    return handle != invalid_handle;
}

inline void mio::close_file() {
    if (!is_open()) {
        return;
    }

#ifdef _MIO_WIN
    CloseHandle(p_file);
#else
    close(p_file);
#endif  // _MIO_WIN

    p_file = invalid_handle;
}

inline size_t mio::file_size() {
    if (!is_open()) {
        return 0;
    }

#ifdef _MIO_WIN
    LARGE_INTEGER size;
    if (GetFileSizeEx(p_file, &size) == FALSE) {
        return 0;
    }
    return static_cast<size_t>(size.QuadPart);
#else
    struct stat st;
    if (fstat(p_file, &st) == -1) {
        return 0;
    }
    return static_cast<size_t>(st.st_size);
#endif  // _MIO_WIN
}

inline size_t mio::read_file(void* buffer, size_t readnum) {
    if (!is_open()) {
        return 0;
    }

    if (p_mode != enum_mode_read) {
        return 0;
    }

#ifdef _MIO_WIN
    DWORD dwRead = 0;
    if (!ReadFile(p_file, buffer, readnum, &dwRead, nullptr)) {
        return 0;
    }
    return dwRead;
#else
    return read(p_file, buffer, readnum);
#endif  // _MIO_WIN
}

inline size_t mio::write_file(const void* buffer, size_t writenum) {
    if (!is_open()) {
        return 0;
    }

    if (p_mode != enum_mode_write) {
        return 0;
    }

#ifdef _MIO_WIN
    DWORD dwWrite = 0;
    if (!WriteFile(p_file, buffer, writenum, &dwWrite, nullptr)) {
        return 0;
    }
    return dwWrite;
#else
    return write(p_file, buffer, writenum);
#endif  // _MIO_WIN
}

inline off_t mio::seek_file(enum_mio_pos pos, off_t offset) {
    if (!is_open()) {
        return -1;
    }

#ifdef _MIO_WIN
    DWORD move;
    switch (pos) {
        case enum_pos_set:
            move = FILE_BEGIN;
            break;
        case enum_pos_cur:
            move = FILE_CURRENT;
            break;
        case enum_pos_end:
            move = FILE_END;
            break;
        default:
            move = FILE_CURRENT;
            break;
    }
    DWORD newoff = SetFilePointer(p_file, offset, nullptr, move);

    return static_cast<off_t>(newoff);
#else
    int move;
    switch (pos) {
        case enum_pos_set:
            move = SEEK_SET;
            break;
        case enum_pos_cur:
            move = SEEK_CUR;
            break;
        case enum_pos_end:
            move = SEEK_END;
            break;
        default:
            move = SEEK_CUR;
            break;
    }
    off_t newoff = lseek(p_file, offset, move);
    return newoff;
#endif  // _MIO_WIN
}

inline bool mio::file_flush() {
    if (!is_open() || p_mode != enum_mode_write) {
        return false;
    }

#ifdef _MIO_WIN
    return (FlushFileBuffers(p_file) == TRUE);
#else
    return (fsync(p_file) == 0);
#endif  // _MIO_WIN
}

}  // namespace mio

#endif  // _MIO_HPP_
