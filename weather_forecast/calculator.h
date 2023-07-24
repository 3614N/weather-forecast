#include "libs.h"

#define R 6371.0 // радиус Земли
#define M_PI 3.14159265358979323846 // число пи

using namespace std;

// градусы в радианы
double toRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

// расстояние между двумя точками
double calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    double dLat = toRadians(lat2 - lat1);
    double dLon = toRadians(lon2 - lon1);

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(toRadians(lat1)) * cos(toRadians(lat2)) *
                   sin(dLon / 2) * sin(dLon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = R * c;

    return distance;
}

// среднее арифметическое
double calculateMean(const vector<double>& data)
{
    double sum = 0.0;
    for (double value : data)
    {
        sum += value;
    }
    return sum / data.size();
}

// вычисление t-статистики
double calculateTStatistic(const vector<double>& group1, const vector<double>& group2)
{
    double mean1 = calculateMean(group1);
    double mean2 = calculateMean(group2);
    double omega1 = 0.0;
    double omega2 = 0.0;

    for (double value : group1)
    {
        omega1 += pow(value - mean1, 2);
    }
    for (double value : group2)
    {
        omega2 += pow(value - mean2, 2);
    }

    omega1 = sqrt(omega1 / group1.size());
    omega2 = sqrt(omega2 / group2.size());

    double m1 = omega1 / sqrt(mean1);
    double m2 = omega2 / sqrt(mean2);

    double t_statistic = (mean1 - mean2) / sqrt(pow(m1, 2) + pow(m2, 2));

    if (abs(t_statistic) >= 0.05) {
        //qDebug() << QString::number(t_statistic);
        return 0;
    }
    else {
        //qDebug() << QString::number(t_statistic);
        return 1;
    }
}

// калькулятор
int mainProcess(double x, double y)
{
    // вектора характеристик метеовышек
    vector<string> city;
    vector<double> temperature;
    vector<double> latitude;
    vector<double> longitude;
    vector<double> altitude;

    vector<vector<double>> nearleft;
    vector<vector<double>> nearright;
    int k = 0;

    // подключение библиотеки и открытие таблицы в базе данных
    sqlite3* db;
    int rc = sqlite3_open("statistic.sqlite", &db);
    sqlite3_stmt* stmt;
    const char* query = "SELECT * FROM weather";
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    // импорт значений с таблицы
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        city.push_back(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
        temperature.push_back(sqlite3_column_double(stmt, 1));
        latitude.push_back(sqlite3_column_double(stmt, 2));
        longitude.push_back(sqlite3_column_double(stmt, 3));
        altitude.push_back(sqlite3_column_double(stmt, 4));

    }

    qDebug() << QString::number(city.size());

    // вычисление ближайших метеостанций  радиусе 500км
    for (int i = 0; i < city.size(); i++)
    {
        double distance = calculateDistance(x, y, latitude[i], longitude[i]);
        if (distance < 500)
        {
            k++;
            vector<double> row = { temperature[i], latitude[i], longitude[i] , distance };

            if (y >= longitude[i]) nearleft.push_back(row); // левые вышки
            else nearright.push_back(row); // правые вышки
        }
    }

    // определение используемой процедуры
    if ((k >= 10) && (nearleft.size() >= 3) && (nearright.size() >= 3))
    {
        return calculateTStatistic(nearleft[0], nearright[0]);
    }
    else return 2; // :)


    // закрываем базу данных
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
