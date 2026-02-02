#include <BKits/Base.h>
#include <BKits/Base/buffer.h>
#include <BKits/Base/string_view.h>
#include <BKits/Version.h>
#include <Bkits/base/timer.h>
#include <Bkits/os/utils.h>
#include <iostream>

int main(int argc, char* argv[])
{
    // 设置输出控制台为 UTF8
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "BKits: " << BKits::Version::VERSION << std::endl;
    BKits::Base::memory_buffer data;
    std::string                ss = "Hello, World!";
    BKits::Base::string_view   st{ss};
    data.append(ss);
    std::cout << data.data() << std::endl;

    std::cout << st.data() << std::endl;
    return 0;
}