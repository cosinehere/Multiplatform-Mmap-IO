#include <iostream>

#include "mio.hpp"

mio::mio miofile;

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

// #ifdef _WIN32
// 	miofile.create_file("e:\\test.txt");
// #else
// 	miofile.create_file("test.txt");
// #endif
// 	miofile.write_file("123", 3);
// 	miofile.close_file();

//     miofile.open_file("test.txt", mio::enum_mode_read);
//     printf("size %lu\n",miofile.file_size());
//     printf("pos %lu\n", miofile.seek_file(mio::enum_pos_end, -2));
//     char buf;
//     miofile.read_file(&buf, 1);
//     miofile.close_file();
// 
//     printf("'%c' 0x%02x\n", buf, buf);


    miofile.create_file("test.txt", mio::enum_mode_rdwr);
    miofile.write_file("123456789", 9);
    miofile.seek_file(mio::enum_pos_set, 3);
    miofile.write_file("000", 3);
    uint8_t ch;
    miofile.read_file(&ch, 1);
    printf("%c\n", ch);
    miofile.close_file();

    getchar();
}
