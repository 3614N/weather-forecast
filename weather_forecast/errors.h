#include "libs.h"


// функция обработки пустых и ошибочных значений
void check() {
    const char* dbFile = "weather_database.sqlite";
    const double conditionValue = -999.0;
    sqlite3* db;

    int rc = sqlite3_open(dbFile, &db);
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table';", -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        char deleteQuery[256];
        snprintf(deleteQuery, sizeof(deleteQuery),
                 "DELETE FROM %s WHERE temperature = %f", tableName, conditionValue);

        char* errorMessage;
        rc = sqlite3_exec(db, deleteQuery, nullptr, nullptr, &errorMessage);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
