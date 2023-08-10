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
       // qDebug() << QString::number(t_statistic);
        return 0;
    }
    else {
       // qDebug() << QString::number(t_statistic);
        return 1;
    }
}

struct WeatherData {
    string city;
    string time;
    string date;
    string windDirection;
    string windAverageSpeed;
    string vision;
    double temperature;
    double humidity;
    double pressure;
};

vector<int> counters;

// Функция для выполнения SQL-запроса и сохранения результатов в вектор
int executeAndSaveQuery(sqlite3* db, const std::string& query, const std::string& city, std::vector<WeatherData>& weatherData, int w) {
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    // Извлекаем данные из запроса и сохраняем их в вектор
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        WeatherData data;
        data.city = city;
        data.time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        data.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        data.windDirection = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        data.windAverageSpeed = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        data.vision = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        data.temperature = stod(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        data.humidity = stod(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
        data.pressure = stod(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
        weatherData.push_back(data);
        counters.push_back(w);
    }

    sqlite3_finalize(stmt);
    return 0;
}

string mostCommonString(const vector<string>& arr) {
    unordered_map<string, int> freq_map;

    for (const string& str : arr) {
        freq_map[str]++;
    }

    string most_common;
    int max_frequency = 0;

    for (const auto& pair : freq_map) {
        if (pair.second > max_frequency) {
            max_frequency = pair.second;
            most_common = pair.first;
        }
    }

    return most_common;
}

double calculateWindAverageSpeed(vector<string> allWAS){
    vector<double> dWAS;
    for (string WAS : allWAS)
    {
        if (WAS.find("-") == string::npos)
        {
            dWAS.push_back(stod(WAS));
        }
        if (WAS.find("-") != string::npos)
        {
            double firstValue = stod(WAS.substr(0, WAS.find("-")));
            double secondValue = stod(WAS.substr(WAS.find("-") + 1, WAS.length()));
            dWAS.push_back(firstValue/secondValue);
        }
    }
    double sum = accumulate(dWAS.begin(), dWAS.end(), 0);
    double average = static_cast<double>(sum) / dWAS.size();
    return average;
}

double dewPoint(double temperature, double humidity) {
    // Константы для расчета точки росы
    double a = 17.27;
    double b = 237.7;

    // Формула для расчета точки росы
    double alpha = ((a * temperature) / (b + temperature)) + log(humidity/100.0);
    double dewPointTemperature = (b * alpha) / (a - alpha);

    return dewPointTemperature;
}

// калькулятор
vector<string> mainProcess(double x, double y, string dateValue, string timeValue)
{ 
    // вектора характеристик метеовышек
    vector<string> city;
    vector<double> latitude;
    vector<double> longitude;
    vector<double> altitude;

    vector<double> nlLat;
    vector<double> nlLot;
    vector<double> nlAlt;
    vector<double> nlDist;
    vector<string> nlWD;
    vector<string> nlWAS;
    vector<string> nlVis;
    vector<double> nlTemp;
    vector<double> nlHum;
    vector<double> nlPres;

    vector<double> nrLat;
    vector<double> nrLot;
    vector<double> nrAlt;
    vector<double> nrDist;
    vector<string> nrWD;
    vector<string> nrWAS;
    vector<string> nrVis;
    vector<double> nrTemp;
    vector<double> nrHum;
    vector<double> nrPres;


    int k = 0;

    // подключение библиотеки и открытие таблицы в базе данных
    sqlite3* db;
    sqlite3_open("info.sqlite", &db);
    sqlite3_stmt* stmt;
    const char* query = "SELECT * FROM info";
    sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    // импорт значений с таблицы
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        city.push_back(string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))));
        latitude.push_back(sqlite3_column_double(stmt, 3));
        longitude.push_back(sqlite3_column_double(stmt, 4));
        altitude.push_back(sqlite3_column_double(stmt, 5));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);


    sqlite3* db2;
    sqlite3_open("climat_database.sqlite", &db2);
    vector<WeatherData> weatherData;

    // Получаем список всех таблиц в базе данных
    string tableQuery = "SELECT name FROM sqlite_master WHERE type='table';";
    sqlite3_stmt* tableStmt;
    int w = 0;
    sqlite3_prepare_v2(db2, tableQuery.c_str(), -1, &tableStmt, nullptr);
    // Для каждой таблицы выполняем запрос и сохраняем результаты в вектор
    while (sqlite3_step(tableStmt) == SQLITE_ROW) {
        const char* tableName = reinterpret_cast<const char*>(sqlite3_column_text(tableStmt, 0));
        string selectQuery = "SELECT * FROM " + string(tableName) +
                             " WHERE time='" + timeValue + "' AND date='" + dateValue + "'" +
                             " AND time != '-999.0' AND date != '-999.0' AND windDirection != '-999.0'" +
                             " AND windAverageSpeed != '-999.0' AND vision != '-999.0'" +
                             " AND temperature != '-999.0' AND humidity != '-999.0'" +
                             " AND pressure != '-999.0';";
        executeAndSaveQuery(db2, selectQuery, string(tableName), weatherData, w);
        w++;
    }

    sqlite3_finalize(tableStmt);
    sqlite3_close(db2);

    int c = 0;
    int e = 0;
    // вычисление ближайших метеостанций  радиусе 200км
    for (const WeatherData& data : weatherData) {
        double distance = calculateDistance(x, y, latitude[counters[e]], longitude[counters[e]]);
        if (distance <= 20)
        {
            nlLat.push_back(latitude[counters[e]]);
            nlLot.push_back(longitude[counters[e]]);
            nlAlt.push_back(altitude[counters[e]]);
            nlDist.push_back(distance);
            nlWD.push_back(data.windDirection);
            nlWAS.push_back(data.windAverageSpeed);
            nlVis.push_back(data.vision);
            nlTemp.push_back(data.temperature);
            nlHum.push_back(data.humidity);
            nlPres.push_back(data.pressure);
            c++;
            break;

        }
        if (distance < 150)
        {

            k++;
            if (y >= longitude[counters[e]])
            {
                nlLat.push_back(latitude[counters[e]]);
                nlLot.push_back(longitude[counters[e]]);
                nlAlt.push_back(altitude[counters[e]]);
                nlDist.push_back(distance);
                nlWD.push_back(data.windDirection);
                nlWAS.push_back(data.windAverageSpeed);
                nlVis.push_back(data.vision);
                nlTemp.push_back(data.temperature);
                nlHum.push_back(data.humidity);
                nlPres.push_back(data.pressure);
            }
            else
            {
                nrLat.push_back(latitude[counters[e]]);
                nrLot.push_back(longitude[counters[e]]);
                nrAlt.push_back(altitude[counters[e]]);
                nrDist.push_back(distance);
                nrWD.push_back(data.windDirection);
                nrWAS.push_back(data.windAverageSpeed);
                nrVis.push_back(data.vision);
                nrTemp.push_back(data.temperature);
                nrHum.push_back(data.humidity);
                nrPres.push_back(data.pressure);
            }
        }
        e++;
    }
    qDebug() << QString::number(k);
    counters.clear();
    for (double i : nlTemp) qDebug() << QString::number(i);
    for (double i : nrTemp) qDebug() << QString::number(i);

    // определение используемой процедуры
    if (c == 1)
    {


        double dew = dewPoint(nlTemp[0], nlHum[0]);

        vector<string> Avgs = {to_string(nlTemp[0]), to_string(nlHum[0]), to_string(nlPres[0]), nlWD[0], nlWAS[0], to_string(dew)};

        c = 0;
        return Avgs;

    }

    else if ((k >= 10) && (nlLat.size() >= 3) && (nrLat.size() >= 3))
    {

        if (!calculateTStatistic(nlTemp, nrTemp) && !calculateTStatistic(nlHum, nrHum) && !calculateTStatistic(nlPres, nrPres))
        {
            double sum1 = accumulate(nlTemp.begin(), nlTemp.end(), 0) + accumulate(nrTemp.begin(), nrTemp.end(), 0);
            double sum2 = accumulate(nlHum.begin(), nlHum.end(), 0) + accumulate(nrHum.begin(), nrHum.end(), 0);
            double sum3 = accumulate(nlPres.begin(), nlPres.end(), 0) + accumulate(nrPres.begin(), nrPres.end(), 0);
            vector<string> allWD = nlWD;
            for (string dir : nrWD) allWD.push_back(dir);
            string avgWD = mostCommonString(allWD);
            vector<string>allWAS = nlWAS;
            for (string was : nrWAS) allWAS.push_back(was);
            double avgWAS = calculateWindAverageSpeed(allWAS);

            double dew = dewPoint(sum1/(nlTemp.size()+nrTemp.size()), sum2/(nlHum.size()+nrHum.size()));

            for (string speed: nrWAS) allWAS.push_back(speed);


            vector<string> Avgs = {to_string(sum1/(nlTemp.size()+nrTemp.size())), to_string(sum2/(nlHum.size()+nrHum.size())), to_string(sum3/(nlPres.size()+nrPres.size())), avgWD, to_string(avgWAS), to_string(dew)};
            qDebug() << QString::number(counters.size());
            return Avgs;
        }

        else
        {
            qDebug() << "АААААААААААААААААААААААААА";
            return {to_string(999.0)};
        }
    }
    else return {to_string(-999.0)}; // :)
}
