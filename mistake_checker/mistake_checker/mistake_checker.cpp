#include <iostream>
#include <sqlite3.h>

int main() {
    const char* dbFile = "weather_database.sqlite";
    const double conditionValue = -999.0;
    sqlite3* db;

    int rc = sqlite3_open(dbFile, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table';", -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        char deleteQuery[256];
        snprintf(deleteQuery, sizeof(deleteQuery),
            "DELETE FROM %s WHERE temperature = %f", tableName, conditionValue);

        char* errorMessage;
        rc = sqlite3_exec(db, deleteQuery, nullptr, nullptr, &errorMessage);
        if (rc != SQLITE_OK) {
            std::cerr << "Cannot delete from " << tableName << ": " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
        }
        else {
            std::cout << "Deleted rows from " << tableName << std::endl;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}