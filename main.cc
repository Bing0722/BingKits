#include <BKits/Version.h>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Hello, World!\n";
    std::cout << "BKits: " << BKits::Version::VERSION << std::endl;
    return 0;
}