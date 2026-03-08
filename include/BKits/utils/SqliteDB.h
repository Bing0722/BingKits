#pragma once

#include <sqlite3.h>
#include <stdexcept>
#include <string>

class SQLiteStmt;

class SQLiteException : public std::runtime_error {
public:
  explicit SQLiteException(const std::string &msg, int code = 0)
      : std::runtime_error(msg), errcode_(code) {}

  int errcode() const { return errcode_; }

private:
  int errcode_;
};

class SQLiteDB {
public:
  explicit SQLiteDB(const std::string &path,
                    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)
      : db_(NULL) {
    int rc = sqlite3_open_v2(path.c_str(), &db_, flags, NULL);
    if (rc != SQLITE_OK) {
      std::string msg = db_ ? sqlite3_errmsg(db_) : "open failed";
      sqlite3_close(db_);
      throw SQLiteException(msg, rc);
    }
    // 默认设置忙等待超时 5 秒
    sqlite3_busy_timeout(db_, 5000);
  }

  ~SQLiteDB() {
    if (db_) {
      sqlite3_close(db_);
    }
  }

  void exec(const std::string &sql) {
    char *errmsg = NULL;
    int rc = sqlite3_exec(db_, sql.c_str(), NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
      std::string msg = errmsg ? errmsg : "exec failed";
      sqlite3_free(errmsg);
      throw SQLiteException(msg, rc);
    }
  }

  // 事务辅助
  void begin() { exec("BEGIN"); }
  void commit() { exec("COMMIT"); }
  void rollback() { exec("ROLLBACK"); }

  // 获取最好插入的ID
  sqlite3_int64 lastInsertRowid() const {
    return sqlite3_last_insert_rowid(db_);
  }

  // 获取被修改或影响的行数
  int changes() const { return sqlite3_changes(db_); }

  sqlite3 *handle() const { return db_; }

private:
  SQLiteDB(const SQLiteDB &);
  SQLiteDB &operator=(const SQLiteDB &);

  sqlite3 *db_;
};
