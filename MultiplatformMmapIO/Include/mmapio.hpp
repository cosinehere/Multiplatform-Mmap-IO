#pragma once

#ifndef _MMAPIO_HPP_
#define _MMAPIO_HPP_

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
#endif	// _MSC_VER < 1400
#else
#include <cstdint>
#endif	// _MSC_VER

//////////////////////////////////////////////////////////////////////////
// system header
//////////////////////////////////////////////////////////////////////////
#ifdef _WIN32	// Windows
#ifndef _AFX	// without MFC
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif	// WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif	// _AFX
#else	// Unix
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif	// _WIN32

//////////////////////////////////////////////////////////////////////////
// C++11 support
//////////////////////////////////////////////////////////////////////////
#if __cplusplus <= 199711L && \
	(!defined(_MSC_VER) || _MSC_VER < 1900) && \
	(!defined(__GNUC__) || \
	(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ < 40603))
#ifndef CXX11_NOT_SUPPORT
#define CXX11_NOT_SUPPORT
#endif	// CXX11_NOT_SUPPORT
#endif

#ifdef CXX11_NOT_SUPPORT
#define nullptr NULL
#define constexpr const
#define noexcept throw()
#define override
#endif

//////////////////////////////////////////////////////////////////////////
// namespace define
//////////////////////////////////////////////////////////////////////////
#if !defined(NAMESPACE_BEGIN)  && !defined(NAMESPACE_END)
#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END }
#endif	// !defined(NAMESPACE_BEGIN)  && !defined(NAMESPACE_END)

NAMESPACE_BEGIN(mmapio)

//////////////////////////////////////////////////////////////////////////
// mmapio typedef
//////////////////////////////////////////////////////////////////////////
#ifdef _WIN32	// Windows
typedef HANDLE HMMAPIO;
#ifndef invalid_handle
#define invalid_handle INVALID_HANDLE_VALUE
#endif	// invalid_handle
#else	// Unix
#define invalid_handle -1
typedef int HMMAPIO;
#endif	// _WIN32

// read/write mode
enum enum_mmapio_mode {
	enum_mode_read = 0,
	enum_mode_write
};

// get system memory page size
inline size_t page_size() noexcept
{
#ifdef _WIN32	// Windows
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwAllocationGranularity;
#else	// Unix
	return sysconf(_SC_PAGE_SIZE);
#endif	// _WIN32
}

static const size_t pagesize = page_size();

// align the offset by pagesize
inline uint64_t offset_aligned(const uint64_t offset) noexcept
{
	return offset / pagesize * pagesize;
}

// mmapio class
template<enum_mmapio_mode Emode>
class mmapio
{
private:
	uint8_t* p_data;
	uint64_t p_length;
	uint64_t p_mapped_size;

	HMMAPIO p_file;
#ifdef _WIN32
	HMMAPIO p_filemap;
#endif

public:
	mmapio();
	~mmapio();

#ifndef CXX11_NOT_SUPPORT
	mmapio(const mmapio&) = delete;
	mmapio(const mmapio&&) = delete;
	mmapio& operator=(const mmapio&) = delete;
	mmapio& operator=(const mmapio&&) = delete;
#endif

	bool is_open() const { return p_file != invalid_handle; }
	bool is_mapped() const
	{
#ifdef _WIN32
		return p_filemap != invalid_handle;
#else
		return is_open();
#endif	// _WIN32
	}

	uint64_t size() const noexcept { return p_length; }
	uint64_t mapped_size() const noexcept { return p_mapped_size; }
	uint64_t mapped_offset() const noexcept { return p_mapped_size - p_length; }

	const uint8_t* data() const noexcept { return p_data; }

	bool map(const char* path, const uint64_t offset, const uint64_t mapsize);
	bool map(const HMMAPIO handle, const uint64_t offset, const uint64_t mapsize);
	void unmap();

	void flush();

private:
	HMMAPIO open_file(const char* path, const enum_mmapio_mode mode);

	bool path_empty(const char* path) { return !path || (*path == '\0'); }

	uint64_t file_size(const HMMAPIO handle);

	bool memory_map(const HMMAPIO handle, const uint64_t offset, const uint64_t mapsize, const enum_mmapio_mode mode);

	const uint8_t* mapping_start() { return (!data()) ? nullptr : data() - (p_mapped_size - p_length); }
};

template<enum_mmapio_mode Emode>
mmapio<Emode>::mmapio()
{
	p_data = nullptr;
	p_length = 0;
	p_mapped_size = 0;

	p_file = invalid_handle;
#ifdef _WIN32
	p_filemap = invalid_handle;
#endif	// _WIN32
}

template<enum_mmapio_mode Emode>
mmapio<Emode>::~mmapio()
{
	// TODO
}

template<enum_mmapio_mode Emode>
bool mmapio<Emode>::map(const char* path, const uint64_t offset, const uint64_t mapsize)
{
	p_file = open_file(path, Emode);
	if (p_file == invalid_handle)
	{
		return false;
	}

	return map(p_file, offset, mapsize);
}

template<enum_mmapio_mode Emode>
bool mmapio<Emode>::map(const HMMAPIO handle, const uint64_t offset, const uint64_t mapsize)
{
	if (handle == invalid_handle)
	{
		return false;
	}

	const uint64_t filesize = file_size(handle);
	if (filesize == 0)
	{
		return false;
	}

	if (filesize < offset + mapsize)
	{
		return false;
	}

	if (!memory_map(handle, offset, mapsize, Emode))
	{
		return false;
	}

	p_file = handle;

	return true;
}

template<enum_mmapio_mode Emode>
void mmapio<Emode>::unmap()
{
	if (!is_open()) { return; }
#ifdef _WIN32
	if (is_mapped())
	{
		UnmapViewOfFile(mapping_start());
		CloseHandle(p_filemap);
	}
#else
	if (data())
	{
		munmap(const_cast<uint8_t*>(mapping_start()), p_mapped_size);
	}
#endif

	p_data = nullptr;
	p_length = 0;
	p_mapped_size = 0;
	p_file = invalid_handle;
#ifdef _WIN32
	p_filemap = invalid_handle;
#endif
}

template<enum_mmapio_mode Emode>
void mmapio<Emode>::flush()
{
	if (!is_open())
	{
		return;
	}

	if (data())
	{
#ifdef _WIN32
		if (FlushViewOfFile(mapping_start(), p_mapped_size) == FALSE || FlushFileBuffers(p_file) == FALSE)
#else
		if (msync(mapping_start(), p_mapped_size, MS_SYNC) != 0)
#endif
		{
			return;
		}
	}

#ifdef _WIN32
	FlushFileBuffers(p_file);
#endif
}

template<enum_mmapio_mode Emode>
HMMAPIO mmapio<Emode>::open_file(const char* path, const enum_mmapio_mode mode)
{
	if (path_empty(path))
	{
		return invalid_handle;
	}

	HMMAPIO handle = invalid_handle;
#ifdef _WIN32
	handle = CreateFileA(path,
		(mode == enum_mode_read) ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
#else
	handle = open(path,
		(mode == enum_mode_read) ? O_RDONLY : O_RDWR);
#endif	// _WIN32
	return handle;
}

template<enum_mmapio_mode Emode>
inline uint64_t mmapio<Emode>::file_size(const HMMAPIO handle)
{
#ifdef _WIN32
	LARGE_INTEGER filesize;
	if (GetFileSizeEx(handle, &filesize) == FALSE)
	{
		return 0;
	}
	return static_cast<uint64_t>(filesize.QuadPart);
#else
	struct stat st;
	if (fstat(handle, &st) == -1)
	{
		return 0;
	}
	return st.st_size;
#endif	// _WIN32
}

template<enum_mmapio_mode Emode>
bool mmapio<Emode>::memory_map(const HMMAPIO handle, const uint64_t offset, const uint64_t mapsize, const enum_mmapio_mode mode)
{
	unmap();

	const uint64_t alignedoffset = offset_aligned(offset);
	const uint64_t actualmapsize = offset - alignedoffset + mapsize;

#ifdef _WIN32
	const uint64_t filesize = offset + mapsize;
	const HMMAPIO filemap = CreateFileMappingA(handle,
		0,
		(mode == enum_mode_read) ? PAGE_READONLY : PAGE_READWRITE,
		filesize >> 32,
		filesize & 0xffffffff,
		nullptr);
	if (filemap == invalid_handle)
	{
		return false;
	}

	uint8_t* mapview = static_cast<uint8_t*>(MapViewOfFile(filemap,
		(mode == enum_mode_read) ? FILE_MAP_READ : FILE_MAP_WRITE,
		alignedoffset >> 32,
		alignedoffset & 0xffffffff,
		static_cast<SIZE_T>(actualmapsize)));
	if (mapview == nullptr)
	{
		CloseHandle(filemap);
		return false;
	}
#else
	uint8_t* mapview = static_cast<uint8_t*>(mmap(0,
		actualmapsize,
		(mode == enum_mode_read) ? PROT_READ : PROT_WRITE,
		MAP_SHARED,
		handle,
		alignedoffset));
	if (mapview == MAP_FAILED)
	{
		return false;
	}
#endif	// _WIN32

	p_data = mapview + offset - alignedoffset;
	p_length = mapsize;
	p_mapped_size = actualmapsize;
#ifdef _WIN32
	p_filemap = filemap;
#endif
	return true;
}

NAMESPACE_END

#endif	// _MMAPIO_HPP_
