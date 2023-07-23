#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDialog>
#include <QPushButton>
#include <QString>
#include <QDebug>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <sqlite3.h>
#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"
#include <curl/curl.h>

#define R 6371.0 // радиус Земли
#define M_PI 3.14159265358979323846 // число пи

using namespace std;

// хз спизженный кусок кода
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// получение верстки сайта
string GetWebsiteData(const string& url)
{
    CURL* curl = curl_easy_init();
    string response;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

// класс таблицы
class Table
{
public:
    // объявление массивов с данными метеостанций
    vector<string> link;
    vector<string> city;
    vector<double> temperature;
    vector<double> latitude;
    vector<double> longitude;
    vector<int> height;

    void addLocation(double lat, double lon, int ht) //заполнение координат
    {
        latitude.push_back(lat);
        longitude.push_back(lon);
        height.push_back(ht);
    }
};

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

    double variance1 = 0.0;
    double variance2 = 0.0;
    for (double value : group1)
    {
        variance1 += pow(value - mean1, 2);
    }
    for (double value : group2)
    {
        variance2 += pow(value - mean2, 2);
    }

    variance1 /= (group1.size() - 1);
    variance2 /= (group2.size() - 1);

    double numerator = mean1 - mean2;
    double denominator = sqrt((variance1 / group1.size()) + (variance2 / group2.size()));
    double t_statistic = numerator / denominator;

    return t_statistic;
}

// парсер
int mainScraper()
{

    // получение верстки сайта с погодой
    string url = "http://www.pogodaiklimat.ru/monitor.php";
    string html_string = GetWebsiteData(url);

    // создание таблицы
    Table weather;


    // подключение парсера
    xmlInitParser();
    LIBXML_TEST_VERSION
        FILE* dummyStream;
    freopen_s(&dummyStream, "nul", "w", stderr);

    // парсинг нашей верстки
    xmlDocPtr doc = htmlReadMemory(html_string.c_str(), html_string.length(), nullptr, nullptr, HTML_PARSE_RECOVER);
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);

    // поиск по верстке элементов списка класса big-blue-billet__list_link
    const xmlChar* xpathExpr = BAD_CAST "//li[contains(@class, 'big-blue-billet__list_link')]/a";
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    xmlNodeSetPtr nodes = xpathObj->nodesetval; //запись полученных элементов в nodes

    if (nodes) { // проверка на заполненость дерева
        for (int i = 0; i < nodes->nodeNr; ++i) { // прохождение по массиву циклом
            xmlNodePtr node = nodes->nodeTab[i];
            xmlChar* href = xmlGetProp(node, BAD_CAST "href"); // извлечение id города
            xmlChar* content = xmlNodeGetContent(node); // получение названия города
            string sName(reinterpret_cast<char*>(content));

            string sHref((char*)href);
            string fullLink = "http://www.pogodaiklimat.ru" + sHref; // генерация ссылки с информацией по городу
            string sCity((char*)content); // запись названия города
            if (sCity != "Ика") // город ика не содержит внутри себя данных, ломая код, поэтому его пропускаем
            {
                //добавление в массив информации
                weather.city.push_back(sCity);
                weather.link.push_back(fullLink);
            }

            if (sName == "Яшкуль") break; // проверка на последний город

            // очистка памяти
            xmlFree(href);
            xmlFree(content);
        }
    }

    // парсинг всех сайтов с данными о российских метеовышках
    for (string link : weather.link)
    {
        // получение со сгенерированной ссылки информации о средней сегодняшней температуры
        string html_link = GetWebsiteData(link);
        xmlDocPtr doc = htmlReadMemory(html_link.c_str(), html_link.length(), nullptr, nullptr, HTML_PARSE_RECOVER);
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
        const xmlChar* xpathExpr = BAD_CAST "//td[@class='green-color'][normalize-space()][1]";
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        if (nodes)
        {
            // обработка и запись информации о температуре
            string sTemp;
            for (int i = 0; i < nodes->nodeNr; ++i)
            {
                xmlNodePtr node = nodes->nodeTab[i];
                xmlChar* temperature = xmlNodeGetContent(node);
                sTemp = reinterpret_cast<char*>(temperature);
            }
            double temp = stod(sTemp);
            weather.temperature.push_back(temp);

        }
        xmlFreeDoc(doc);

        // поиск вхождения в подстроку с информацией о расположении вышки
        size_t latitudePos = html_link.find("широта") + 13;
        size_t longitudePos = html_link.find("долгота") + 15;
        size_t altitudePos = html_link.find("высота над уровнем моря") + 44;

        // запись в строку найденной информации
        string latitude = html_link.substr(latitudePos, html_link.find(",", latitudePos) - latitudePos);
        string longitude = html_link.substr(longitudePos, html_link.find(",", longitudePos) - longitudePos);
        string altitude = html_link.substr(altitudePos, html_link.find(" м.", altitudePos) - altitudePos);

        // превращение строки в другие типы данных
        double dLatitude = stod(latitude);
        double dLongitude = stod(longitude);
        int iAltitude = stoi(altitude);

        weather.addLocation(dLatitude, dLongitude, iAltitude); // запись координат
    }


    // удаление прошлой бд
    const char* filename = "statistic.sqlite";
    remove(filename);

    // подключение SQLite3 и создание новой бд
    sqlite3* db;
    int rc = sqlite3_open("statistic.sqlite", &db);

    // создание шаблона таблицы
    char* errMsg;
    string createTableQuery = "CREATE TABLE IF NOT EXISTS weather ("
        "city TEXT, "
        "temperature REAL, "
        "latitude REAL, "
        "longitude REAL, "
        "height INTEGER);";

    rc = sqlite3_exec(db, createTableQuery.c_str(), nullptr, nullptr, &errMsg);
    string insertQuery = "INSERT OR REPLACE INTO weather (city, temperature, latitude, longitude, height) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    // заполнение таблицы
    for (size_t i = 0; i < weather.link.size(); ++i)
    {
        rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr);

        sqlite3_bind_text(stmt, 1, weather.city[i].c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, weather.temperature[i]);
        sqlite3_bind_double(stmt, 3, weather.latitude[i]);
        sqlite3_bind_double(stmt, 4, weather.longitude[i]);
        sqlite3_bind_int(stmt, 5, weather.height[i]);

        rc = sqlite3_step(stmt);

        sqlite3_finalize(stmt);
    }

    return 0;

    sqlite3_close(db);

    // очистка памяти
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
    xmlCleanupParser();

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
        // проверка на однородность
        double t_statistic = calculateTStatistic(nearleft[0], nearright[0]);
        int df = nearleft.size() + nearright.size() - 2;
        double alpha = 0.05;
        double critical_t = abs(t_statistic) / sqrt((1.0 / nearleft.size()) + (1.0 / nearright.size()));

        // вывод процедуры
        if (t_statistic > critical_t)
        {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 0;
        }
        else
        {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }
    }
    else return 2; // :)


    // закрываем базу данных
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    double a = ui->latitude->text().toDouble();
    double b = ui->longitude->text().toDouble();
    int answer = mainProcess(a, b);
    QString xAnswer;
    if (answer == 0)
    {
        xAnswer = "Использовать осреднение";
    }
    else if (answer == 1)
    {
        xAnswer = "Использовать интерполяцию";
    }
    else
    {
        xAnswer = "Украсть у Вадима";
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Answer");
    QLabel *label = new QLabel("Текущий метод обработки:\n" + xAnswer, dialog);


    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(400, 100);
    dialog->exec();
}


void MainWindow::on_pushButton_2_clicked()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Status");
    QLabel *label = new QLabel("Генерируется актуальная база данных...\nОкно автоматически закроется при завершении", dialog);
    QFont font = label->font();
    font.setPointSize(16);
    label->setFont(font);
    dialog->setFixedSize(600, 100);
    dialog->exec();

    int status = mainScraper();

    dialog->close();

}

