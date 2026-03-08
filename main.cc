#include <BKits/Base.h>
#include <BKits/Version.h>
#include <BKits/base/buffer.h>
#include <BKits/base/string_view.h>
#include <BKits/base/timer.h>
#include <BKits/os/utils.h>
#include <BKits/utils/SqliteStmt.h>
#include <BKits/utils/SqliteTransaction.h>
#include <iostream>

int main(int argc, char *argv[]) {
  // 设置输出控制台为 UTF8
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  std::cout << "BKits: " << BKits::Version::VERSION << std::endl;

  return 0;
}
