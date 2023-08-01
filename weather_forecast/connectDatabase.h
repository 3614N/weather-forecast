#ifndef CONNECT_DATABASE_H
#define CONNECT_DATABASE_H
#include "libs.h"

using namespace std;

vector<int> parseDate(const string& dateStr) {
    vector<int> dateParts;
    stringstream ss(dateStr);
    string token;
    while (getline(ss, token, '.')) {
        dateParts.push_back(stoi(token));
    }
    return dateParts;
}

bool isDateGreater(const string& date1, const string& date2) {
    vector<int> dateParts1 = parseDate(date1);
    vector<int> dateParts2 = parseDate(date2);

    for (size_t i = 2; i >= 0; --i) {
        if (dateParts1[i] > dateParts2[i]) {
            return true;
        }
        else if (dateParts1[i] < dateParts2[i]) {
            return false;
        }
    }
    return false;
}

void connectDb() {
    sqlite3* db;
    int rc = sqlite3_open("weather_database.sqlite", &db);

    vector<string> tableNames;
    string sql = "SELECT name FROM sqlite_master WHERE type='table';";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        tableNames.push_back(tableName);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);


    sqlite3* cityDb;
    rc = sqlite3_open("city_database.sqlite", &cityDb);


    for (const string& tableName : tableNames) {
        sqlite3* weatherDb;
        rc = sqlite3_open("weather_database.sqlite", &weatherDb);


        sql = "SELECT date, temperature FROM " + tableName + ";";
        rc = sqlite3_prepare_v2(weatherDb, sql.c_str(), -1, &stmt, nullptr);

        string maxDate;
        double maxTemperature = numeric_limits<double>::min();

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double temperature = sqlite3_column_double(stmt, 1);

            if (maxDate.empty() || isDateGreater(date, maxDate)) {
                maxDate = date;
                maxTemperature = temperature;
            }
        }

        sqlite3_finalize(stmt);
        sqlite3_close(weatherDb);

        if (!maxDate.empty()) {
            sql = "UPDATE locations SET date='" + maxDate + "', temperature=" + to_string(maxTemperature) + " WHERE city='" + tableName + "';";
            rc = sqlite3_exec(cityDb, sql.c_str(), nullptr, nullptr, nullptr);

        }
    }

    sqlite3_close(cityDb);

}
#endif // CONNECT_DATABASE_H
