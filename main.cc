#include <BKits/Base.h>
#include <BKits/Base/buffer.h>
#include <BKits/Base/string_view.h>
#include <BKits/Version.h>
#include <Bkits/base/timer.h>
#include <Bkits/os/utils.h>
#include <Bkits/utils/SqliteStmt.h>
#include <Bkits/utils/SqliteTransaction.h>
#include <iostream>

int main(int argc, char* argv[])
{
    // 设置输出控制台为 UTF8
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "BKits: " << BKits::Version::VERSION << std::endl;

    try
    {
        SQLiteDB db("test.db");
        // 建表
        db.exec("CREATE TABLE IF NOT EXISTS user("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "age  INTEGER,"
                "score REAL)");
        // 批量插入（事务 + 复用 stmt）
        {
            SQLiteTransaction tx(db);
            SQLiteStmt        stmt(db, "INSERT INTO user(name,age,score) VALUES(?,?,?)");

            const char* names[]  = {"Alice", "Bob", "Carol", "Dave"};
            int         ages[]   = {25, 30, 22, 35};
            double      scores[] = {95.5, 80.0, 88.3, 72.1};

            for (int i = 0; i < 4; i++)
            {
                stmt.bindText(1, names[i]).bindInt(2, ages[i]).bindDouble(3, scores[i]);
                stmt.execute(); // step + reset
            }
            tx.commit();
        }

        std::cout << "插入行数: " << db.changes() << "\n";

        // 查询
        {
            SQLiteStmt stmt(db, "SELECT id,name,age,score FROM user WHERE age > ?");
            stmt.bindInt(1, 23);

            while (stmt.step())
            {
                std::cout << stmt.columnInt(0) << " " << stmt.columnText(1) << " "
                          << stmt.columnInt(2) << " " << stmt.columnDouble(3) << "\n";
            }
        }

        // 更新
        {
            SQLiteStmt stmt(db, "UPDATE user SET score=? WHERE name=?");
            stmt.bindDouble(1, 99.0).bindText(2, "Alice");
            stmt.execute();
            std::cout << "更新行数: " << db.changes() << "\n";
        }

        // 删除
        {
            SQLiteStmt stmt(db, "DELETE FROM user WHERE age > ?");
            stmt.bindInt(1, 30);
            stmt.execute();
        }
    }
    catch (const SQLiteException& e)
    {
        std::cerr << "SQLite error(" << e.errcode() << "): " << e.what() << std::endl;
        return 1;
    }
    return 0;
}