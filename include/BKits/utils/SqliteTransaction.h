#pragma once

#include "SqliteDB.h"

class SQLiteTransaction
{
public:
    explicit SQLiteTransaction(SQLiteDB& db) : db_(db), committed_(false) { db_.begin(); }

    ~SQLiteTransaction()
    {
        if (!committed_)
        {
            try
            {
                db_.rollback();
            }
            catch (...)
            {
            }
        }
    }

    void commit()
    {
        db_.commit();
        committed_ = true;
    }

private:
    SQLiteTransaction(const SQLiteTransaction&);
    SQLiteTransaction& operator=(const SQLiteTransaction&);

    SQLiteDB& db_;
    bool      committed_;
};