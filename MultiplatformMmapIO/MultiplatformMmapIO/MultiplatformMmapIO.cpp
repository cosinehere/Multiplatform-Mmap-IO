﻿#include <iostream>

#include "../Include/mio.hpp"
#include "../Include/mmapio.hpp"

mmapio::mmapio<mmapio::enum_mode_write> io;

mio::mio<mio::enum_mode_write> miofile;

int main()
{
// 	std::cout << mmapio::page_size() << std::endl;
// #ifdef _WIN32
// 	std::cout << io.map("e:\\test.txt", 0, 9) << std::endl;
// #else
// 	std::cout << io.map("test.txt", 0, 10) << std::endl;
// #endif
// 	for (uint64_t i = 0; i < io.size(); ++i)
// 	{
// 		std::cout << io.data()[i];
// 	}
// 	std::cout << std::endl;
//
// 	uint8_t* data = const_cast<uint8_t*>(io.data());
// 	*data = '2';
//
// 	io.unmap();

#ifdef _WIN32
	miofile.create_file("e:\\test.txt");
#else
	miofile.create_file("test.txt");
#endif
	miofile.write_file("123", 3);
	miofile.close_file();
}
