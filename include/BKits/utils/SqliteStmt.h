#pragma once

#include "SqliteDB.h"
#include <string>

class SQLiteStmt {
public:
  SQLiteStmt(SQLiteDB &db, const std::string &sql)
      : stmt_(NULL), db_(db.handle()) {
    int rc = sqlite3_prepare_v2(db.handle(), sql.c_str(), -1, &stmt_, NULL);
    if (rc != SQLITE_OK) {
      throw SQLiteException(sqlite3_errmsg(db.handle()), rc);
    }
  }

  ~SQLiteStmt() {
    if (stmt_) {
      sqlite3_finalize(stmt_);
    }
  }

  SQLiteStmt &bindInt(int col, int val) {
    check(sqlite3_bind_int(stmt_, col, val));
    return *this;
  }

  SQLiteStmt &bindInt64(int col, sqlite3_int64 val) {
    check(sqlite3_bind_int64(stmt_, col, val));
    return *this;
  }

  SQLiteStmt &bindDouble(int col, double val) {
    check(sqlite3_bind_double(stmt_, col, val));
    return *this;
  }

  // SQLITE_TRANSIENT: SQLite 内部复制，调用方无需保证字符串生命周期
  SQLiteStmt &bindText(int col, const std::string &val) {
    check(sqlite3_bind_text(stmt_, col, val.c_str(), (int)val.size(),
                            SQLITE_TRANSIENT));
    return *this;
  }

  SQLiteStmt &bindBlob(int col, const void *data, int size) {
    check(sqlite3_bind_blob(stmt_, col, data, size, SQLITE_TRANSIENT));
    return *this;
  }

  SQLiteStmt &bindNull(int col) {
    check(sqlite3_bind_null(stmt_, col));
    return *this;
  }

  // Step
  // 返回 true 表示有数据行（SELECT 用）
  // 返回 false 表示执行完毕（INSERT/UPDATE/DELETE 用）
  bool step() {
    int rc = sqlite3_step(stmt_);
    if (rc == SQLITE_ROW)
      return true;
    if (rc == SQLITE_DONE)
      return false;
    throw SQLiteException(sqlite3_errmsg(db_), rc);
  }

  // 重置（用于复用）
  void reset() {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
  }

  // 执行一次非查询语句（INSERT/UPDATE/DELETE）
  void execute() {
    step();
    reset();
  }

  int columnType(int col) const { return sqlite3_column_type(stmt_, col); }

  bool isNull(int col) const {
    return sqlite3_column_type(stmt_, col) == SQLITE_NULL;
  }

  int columnInt(int col) const { return sqlite3_column_int(stmt_, col); }

  sqlite3_int64 columnInt64(int col) const {
    return sqlite3_column_int64(stmt_, col);
  }

  double columnDouble(int col) const {
    return sqlite3_column_double(stmt_, col);
  }

  std::string columnText(int col) const {
    const unsigned char *p = sqlite3_column_text(stmt_, col);
    if (!p)
      return "";
    return std::string(reinterpret_cast<const char *>(p));
  }

  const void *columnBlob(int col) const {
    return sqlite3_column_blob(stmt_, col);
  }

  int columnBytes(int col) const { return sqlite3_column_bytes(stmt_, col); }

  int columnCount() const { return sqlite3_column_count(stmt_); }

  std::string columnName(int col) const {
    return sqlite3_column_name(stmt_, col);
  }

  sqlite3_stmt *handle() const { return stmt_; }

private:
  SQLiteStmt(const SQLiteStmt &);
  SQLiteStmt &operator=(const SQLiteStmt &);

  void check(int rc) {
    if (rc != SQLITE_OK) {
      throw SQLiteException(sqlite3_errmsg(db_), rc);
    }
  }

  sqlite3_stmt *stmt_;
  sqlite3 *db_;
};
