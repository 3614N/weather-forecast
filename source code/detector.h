#include "libs.h"

using namespace std;

vector<string> begData;


// Функция для вычисления медианы массива
double median(vector<double> arr, int n) {
    sort(arr.begin(), arr.end());
    if (n % 2 == 0) {
        return (arr[n / 2 - 1] + arr[n / 2]) / 2.0;
    }
    else {
        return arr[n / 2];
    }
}

// Функция для вычисления абсолютного отклонения каждого элемента от медианы
vector<double> absoluteDeviations(vector<double> arr, int n, double medianValue) {
    vector<double> deviations(n);
    for (int i = 0; i < n; i++) {
        deviations[i] = abs(arr[i] - medianValue);
    }
    return deviations;
}

// Функция для вычисления медианы абсолютных отклонений
double medianAbsoluteDeviation(vector<double> arr, int n) {
    double medianValue = median(arr, n);
    vector<double> deviations = absoluteDeviations(arr, n, medianValue);
    return median(deviations, deviations.size());
}

// Функция для определения порога аномальных значений
double threshold(vector<double> arr, int n) {
    double mad = medianAbsoluteDeviation(arr, n);
    return 5 * mad;
}

double separatedAverage(string s) {
    string separator = "-";
    string first_value = s.substr(0, s.find(separator));
    string second_value = s.substr(s.find(separator) + 1, s.length());
    double first_value1 = stod(first_value);
    double second_value1 = stod(second_value);
    return ((first_value1 + second_value1) / 2);
}

double kmDelete(string s) {
    if (s.find("км") != string::npos) {
        string str = s.substr(0, s.find(" "));
    return stod(str);
}
else {
    string str = s.substr(0, s.find(" "));
    return stod(str) / 1000;
}
}

// Функция для удаления аномальных значений из массива
vector<string> removeOutliers(vector<double> arr, int n, int index, vector<string> date) {
    vector<double> indexList;
    vector<string> unfilteredValues;
    double thresholdValue = threshold(arr, n);
    for (int i = 0; i < n; i++) {
        if (not(abs(arr[i] - median(arr, n)) <= thresholdValue)) {
            int TrueIndex = index * 10 + i;
            unfilteredValues.push_back(date[TrueIndex]);
            //string strTrueIndex = to_string(TrueIndex);
            indexList.push_back(TrueIndex);
        }
    }

    return unfilteredValues;
}

int tempCallBack(void* data, int argc, char** argv, char** azColName) {
    vector<double>* values = static_cast<vector<double>*>(data);
    try {
        double value = stod(argv[6]);
        values->push_back(value); // Добавляем значение в вектор

    }
    catch (...) {
        begData.push_back(argv[1]);
    }
    return 0;
}

int windCallBack(void* data, int argc, char** argv, char** azColName) {
    vector<double>* values = static_cast<vector<double>*>(data);
    try {
        double value = separatedAverage(argv[4]);
        values->push_back(value); // Добавляем значение в вектор

    }
    catch (...) {
        begData.push_back(argv[1]);
    }
    return 0;
}

int pressureCallBack(void* data, int argc, char** argv, char** azColName) {
    vector<double>* values = static_cast<vector<double>*>(data);
    try {
        double value = stod(argv[8]);
        values->push_back(value); // Добавляем значение в вектор
    }
    catch (...) {
        begData.push_back(argv[1]);
    }
    return 0;
}

int humidityCallBack(void* data, int argc, char** argv, char** azColName) {
    vector<double>* values = static_cast<vector<double>*>(data);
    try {
        double value = stod(argv[7]);
        values->push_back(value); // Добавляем значение в вектор
    }
    catch (...) {
        begData.push_back(argv[1]);
    }
    return 0;
}


int dateCallBack(void* data, int argc, char** argv, char** azColName) {
    vector<string>* values = static_cast<vector<string>*>(data);
    values->push_back(argv[1]); // Добавляем значение в вектор
    return 0;
}

int TableCallBack(void* data, int argc, char** argv, char** azColName) {
    vector<string>* columnData = static_cast<vector<string>*>(data);

    for (int i = 0; i < argc; i++) {
        columnData->push_back(argv[i]);
    }

    return 0;
}

void udalyator(string time) {
    sqlite3* db;
    sqlite3_open("climat_database.sqlite", &db);

    vector<string> cities;
    vector<double> temperature;
    vector<double> vision;
    vector<double> windSpeed;
    vector<double> humidity;
    vector<double> pressure;
    vector<string> date;
    string TableSqlQuery = "SELECT name FROM sqlite_master where type='table'";

    sqlite3_exec(db, TableSqlQuery.c_str(), TableCallBack, &cities, nullptr);

    for (size_t j = 0; j < cities.size(); j++) {
        cout << cities[j] << endl;

        string query = "DELETE FROM " + cities[j] + " WHERE "
                                                    "windAverageSpeed = -999.0 OR "
                                                    "windDirection = -999.0 OR "
                                                    "humidity = -999.0 OR "
                                                    "pressure = -999.0 OR "
                                                    "temperature = -999.0;";

        sqlite3_exec(db, query.c_str(), NULL, 0, nullptr);


        string SqlQuery = "SELECT * FROM " + cities[j] + " WHERE time = " + time +";";
        sqlite3_exec(db, SqlQuery.c_str(), tempCallBack, &temperature, nullptr);
        sqlite3_exec(db, SqlQuery.c_str(), windCallBack, &windSpeed, nullptr);
        sqlite3_exec(db, SqlQuery.c_str(), pressureCallBack, &pressure, nullptr);
        sqlite3_exec(db, SqlQuery.c_str(), humidityCallBack, &humidity, nullptr);
        sqlite3_exec(db, SqlQuery.c_str(), dateCallBack, &date, nullptr);

        for (size_t i = 0; i < begData.size(); i++) {
            query = "DELETE FROM " + cities[j] + " WHERE date = " + begData[i] + ";";
            sqlite3_exec(db, query.c_str(), NULL, 0, nullptr);
        }

        begData.clear();


        int humidity_n = humidity.size();
        vector<vector<double>> season_humidity;
        vector<double> updatable_season_humidity;

        for (int i = 0; i < humidity_n; i++) {
            if (i % 10 == 0 && not(updatable_season_humidity.empty())) {
                season_humidity.push_back(updatable_season_humidity);
                updatable_season_humidity.clear();
            }
            updatable_season_humidity.push_back(humidity[i]);
        }

        for (size_t i = 0; i < season_humidity.size(); i++) {
            vector<string> indexList = removeOutliers(season_humidity[i], season_humidity[i].size(), i, date);
            for (auto value : indexList) {
                string DelQuery = "DELETE FROM " + cities[j] + " WHERE date = " + value + ";";
                sqlite3_exec(db, DelQuery.c_str(), nullptr, nullptr, nullptr);
            }
        }
        humidity.clear();

        int pressure_n = pressure.size();
        vector<vector<double>> season_pressure;
        vector<double> updatable_season_pressure;

        for (int i = 0; i < pressure_n; i++) {
            if (i % 10 == 0 && not(updatable_season_pressure.empty())) {
                season_pressure.push_back(updatable_season_pressure);
                updatable_season_pressure.clear();
            }
            updatable_season_pressure.push_back(pressure[i]);
        }

        for (size_t i = 0; i < season_pressure.size(); i++) {
            vector<string> indexList = removeOutliers(season_pressure[i], season_pressure[i].size(), i, date);
            for (auto value : indexList) {
                string DelQuery = "DELETE FROM " + cities[j] + " WHERE date = " + value + ";";
                sqlite3_exec(db, DelQuery.c_str(), nullptr, nullptr, nullptr);
            }
        }
        pressure.clear();


        int wind_n = windSpeed.size();
        vector<vector<double>> season_wind;
        vector<double> updatable_season_wind;

        for (int i = 0; i < wind_n; i++) {
            if (i % 10 == 0 && not(updatable_season_wind.empty())) {
                season_wind.push_back(updatable_season_wind);
                updatable_season_wind.clear();
            }
            updatable_season_wind.push_back(windSpeed[i]);
        }

        for (size_t i = 0; i < season_wind.size(); i++) {
            vector<string> indexList = removeOutliers(season_wind[i], season_wind[i].size(), i, date);
            for (auto value : indexList) {
                string DelQuery = "DELETE FROM " + cities[j] + " WHERE date = " + value + ";";
                sqlite3_exec(db, DelQuery.c_str(), nullptr, nullptr, nullptr);
            }
        }
        windSpeed.clear();


        int temp_n = temperature.size(); // Размер массива
        vector<vector<double>> season_temp;
        vector<double> updatable_season_temp;

        for (int i = 0; i < temp_n; i++) {
            if (i % 10 == 0 && not(updatable_season_temp.empty())) {
                season_temp.push_back(updatable_season_temp);
                updatable_season_temp.clear();
            }
            updatable_season_temp.push_back(temperature[i]);
        }

        for (size_t i = 0; i < season_temp.size(); i++) {
            vector<string> indexList = removeOutliers(season_temp[i], season_temp[i].size(), i, date);
            for (auto value : indexList) {
                string DelQuery = "DELETE FROM " + cities[j] + " WHERE date = " + value + ";";
                sqlite3_exec(db, DelQuery.c_str(), nullptr, nullptr, nullptr);
            }
        }
        temperature.clear();
        date.clear();

    }
    sqlite3_close(db);
}
