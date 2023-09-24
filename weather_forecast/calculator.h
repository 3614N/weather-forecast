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


struct WeatherData
{
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


// функция для выполнения SQL-запроса и сохранения результатов в вектор
int executeAndSaveQuery(sqlite3* db, const std::string& query, const std::string& city, std::vector<WeatherData>& weatherData, int w)
{
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    // извлекаем данные из запроса и сохраняем их в вектор
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
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

std::vector<int> mostFrequentIndices;

// вычисление направления ветра
string calculateWindDirection(const vector<string>& strings)
{
    std::string mostFrequentString;
    int maxFrequency = 0;

    for (int i = 0; i < strings.size(); ++i) {
        const std::string& str = strings[i];
        int frequency = std::count(strings.begin(), strings.end(), str);

        if (frequency > maxFrequency) {
            maxFrequency = frequency;
            mostFrequentString = str;
            mostFrequentIndices.clear();
            mostFrequentIndices.push_back(i);
        } else if (frequency == maxFrequency) {
            mostFrequentIndices.push_back(i);
        }
    }
    return mostFrequentString;
}


// вычисление средней скорости ветра
double calculateWindAverageSpeed(vector<string> allWAS)
{
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

    double sum;
    for (int i : mostFrequentIndices){
        sum += dWAS[i];
    }
    //double sum = accumulate(dWAS.begin(), dWAS.end(), 0);
    double average = sum / mostFrequentIndices.size();
    return average;
}

// вычисление точки росы
double dewPoint(double temperature, double humidity)
{
    // Константы для расчета точки росы
    double a = 17.27;
    double b = 237.7;

    // Формула для расчета точки росы
    double alpha = ((a * temperature) / (b + temperature)) + log(humidity/100.0);
    double dewPointTemperature = (b * alpha) / (a - alpha);

    return dewPointTemperature;
}

vector<double> aDistance;
vector<double> bDistance;
vector<double> cDistance;

double interpolation(vector<double> latitudes, vector<double> longitudes, vector<double> values, double latitude, double longitude)
{
    vector<double> h;
    vector<double> nu1;

    vector<vector<double>> points;
    for (size_t i = 0; i < latitudes.size(); i++)
    {
        vector<double> row = {latitudes[i], longitudes[i], values[i]};
        points.push_back(row);
    }

    for (size_t i = 0; i < points.size() - 1; i++)
    {

        for (size_t e = i + 1; e < points.size(); e++)
        {

            double p;
            double a1;
            double delta = 0.0;
            double tempH;
            double a =pow( pow( points[i][0]-points[e][0], 2.0 )+pow( points[i][1]-points[e][1], 2.0 ), 0.5);
            double b =pow( pow( latitude-points[i][0], 2.0 )+pow( longitude-points[i][1], 2.0 ), 0.5);
            double c =pow( pow( latitude-points[e][0], 2.0 )+pow( longitude-points[e][1], 2.0 ), 0.5);


            aDistance.push_back(a);
            bDistance.push_back(b);
            cDistance.push_back(c);
            p = (a+b+c)/2.0;
            tempH = 2*(pow((p*(p-a)+(p-b)*(p-c)),0.5))/a;
            a1 = pow(b*b-tempH*tempH, 0.5);

            if (a1 < a)
            {

                delta = points[i][2] - points[e][2];

                nu1.push_back(points[e][2]+ a1/a*delta);
                h.push_back(tempH);
            }

        }
    }

    double averageNu;
    double first;
    double second;
    for (size_t i = 0; i < nu1.size(); i++)
    {
        first += 1/h[i]*nu1[i];
        second += 1/h[i];
    }
    averageNu = first/second;

    aDistance.clear();
    bDistance.clear();
    cDistance.clear();
    return averageNu;

}

// получение итоговых метеоданных
vector<string> calculateWeather(double x, double y, string dateValue, string timeValue, double radius)
{ 
    // вектора характеристик метеовышек
    vector<string> city;
    vector<double> latitude;
    vector<double> longitude;
    vector<double> altitude;

    // nl - Near Left (!)
    vector<double> nlLat;
    vector<double> nlLon;
    vector<double> nlAlt;
    vector<double> nlDist;
    vector<string> nlWD;
    vector<string> nlWAS;
    vector<string> nlVis;
    vector<double> nlTemp;
    vector<double> nlHum;
    vector<double> nlPres;

    // nr - Near Right (!)
    vector<double> nrLat;
    vector<double> nrLon;
    vector<double> nrAlt;
    vector<double> nrDist;
    vector<string> nrWD;
    vector<string> nrWAS;
    vector<string> nrVis;
    vector<double> nrTemp;
    vector<double> nrHum;
    vector<double> nrPres;



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


    vector<string> names;



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
        names.push_back(string(tableName));
        w++;
    }

    sqlite3_finalize(tableStmt);
    sqlite3_close(db2);

    bool isNearest = false; // индикатор вышки в радиусе 20км
    int e = 0; // счетчик векторов координат
    int nearCounter = 0; // счетчик ближайшик вышек


    vector<string> nlNames;

    // вычисление ближайших метеостанций в заданном радиусе
    for (const WeatherData& data : weatherData)
    {
        double distance = calculateDistance(x, y, latitude[counters[e]], longitude[counters[e]]);

        // случай, когда рассчетом показателей можно пренебречь ( вышка в менее, чем 20км от искомой точки )
        if (distance <= 20)
        {
            nlNames.push_back(names[counters[e]]);

            nlLat.push_back(latitude[counters[e]]);
            nlLon.push_back(longitude[counters[e]]);
            nlAlt.push_back(altitude[counters[e]]);
            nlDist.push_back(distance);
            nlWD.push_back(data.windDirection);
            nlWAS.push_back(data.windAverageSpeed);
            nlVis.push_back(data.vision);
            nlTemp.push_back(data.temperature);
            nlHum.push_back(data.humidity);
            nlPres.push_back(data.pressure);
            isNearest = true;
            break;
        }

        if (distance < radius)
        {

            nearCounter++;
            if (y >= longitude[counters[e]])
            {
                nlLat.push_back(latitude[counters[e]]);
                nlLon.push_back(longitude[counters[e]]);
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
                nrLon.push_back(longitude[counters[e]]);
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

    //qDebug() << QString::number(k);

    // очистка глобального вектора
    counters.clear();

    for (int i = 0; i < nlLat.size(); i++)
    {
        cout << nlHum[i]<< endl;
    }
    for (int i = 0; i < nrLat.size(); i++)
    {
        cout << nrHum[i]<< endl;
    }

    // определение используемой процедуры
    if (isNearest) // если есть вышка в 20км
    {
        double dew = dewPoint(nlTemp[0], nlHum[0]);

        vector<string> Avgs = {to_string(nlTemp[0]), to_string(nlHum[0]), to_string(nlPres[0]), nlWD[0], nlWAS[0], to_string(dew)};

        return Avgs;
    }
    else if ((nearCounter >= 10) && (nlLat.size() >= 3) && (nrLat.size() >= 3)) // если ближайших вышек достаточно для рассчета метеопоказателей
    {
        vector<string> allWD = nlWD;
        for (string dir : nrWD) allWD.push_back(dir);
        string avgWD = calculateWindDirection(allWD);

        vector<string>allWAS = nlWAS;
        for (string was : nrWAS) allWAS.push_back(was);
        double avgWAS = calculateWindAverageSpeed(allWAS);

        for (int i = 0; i < mostFrequentIndices.size(); i++){
            cout << mostFrequentIndices[i] << endl;
        }
        mostFrequentIndices.clear();

        // удволетворяет критерию Стьюдента
        /*if (!calculateTStatistic(nlTemp, nrTemp) && !calculateTStatistic(nlHum, nrHum) && !calculateTStatistic(nlPres, nrPres))
        {

            double sum1 = accumulate(nlTemp.begin(), nlTemp.end(), 0) + accumulate(nrTemp.begin(), nrTemp.end(), 0);
            double sum2 = accumulate(nlHum.begin(), nlHum.end(), 0) + accumulate(nrHum.begin(), nrHum.end(), 0);
            double sum3 = accumulate(nlPres.begin(), nlPres.end(), 0) + accumulate(nrPres.begin(), nrPres.end(), 0);



            double dew = dewPoint(sum1/(nlTemp.size()+nrTemp.size()), sum2/(nlHum.size()+nrHum.size()));

            for (string speed: nrWAS) allWAS.push_back(speed);


            vector<string> Avgs = {to_string(sum1/(nlTemp.size()+nrTemp.size())), to_string(sum2/(nlHum.size()+nrHum.size())), to_string(sum3/(nlPres.size()+nrPres.size())), avgWD, to_string(avgWAS), to_string(dew)};

            return Avgs;
        }*/

        // не удволетворяет
        //else
        //{
            vector<double> allLat = nlLat;
            for (double i : nrLat) allLat.push_back(i);
            vector<double> allLon = nlLon;
            for (double i : nrLon) allLon.push_back(i);
            vector<double> allTemp = nlTemp;
            for (double i : nrTemp) allTemp.push_back(i);
            vector<double> allHum = nlHum;
            for (double i : nrHum) allHum.push_back(i);
            vector<double> allPres = nlPres;
            for (double i : nrPres) allPres.push_back(i);

            double dew = dewPoint(interpolation(allLat, allLon, allTemp, x, y),interpolation(allLat, allLon, allHum, x, y));

            vector<string> Avgs = {to_string(interpolation(allLat, allLon, allTemp, x, y)), to_string(interpolation(allLat, allLon, allHum, x, y)), to_string(interpolation(allLat, allLon, allPres, x, y)), avgWD, to_string(avgWAS), to_string(dew)};

            return Avgs;
        //}
    }
    else return {to_string(-999.0)}; // :)
}
