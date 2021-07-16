// MultiplatformMmapIO.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

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
	miofile.open_file("e:\\test.txt");
#else
	miofile.open_file("test.txt");
#endif
	char buffer[100] = "126456";
	size_t write = miofile.write_file(buffer, 5);
	std::cout << write << std::endl;
	miofile.close_file();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
