#include "libs.h"

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


// парсер
void mainScraper()
{
    // вектора данных метеовышек
    vector<string> link;
    vector<string> city;
    vector<double> temperature;
    vector<double> latitude;
    vector<double> longitude;
    vector<int> height;

    // получение верстки сайта с погодой
    string url = "http://www.pogodaiklimat.ru/monitor.php";
    string html_string = GetWebsiteData(url);


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
                city.push_back(sCity);
                link.push_back(fullLink);
                QString city1 = QString::fromStdString(sCity);
                //qDebug() << city1;
            }

            if (sName == "Яшкуль") break; // проверка на последний город

            // очистка памяти
            xmlFree(href);
            xmlFree(content);
        }
    }

    // парсинг всех сайтов с данными о российских метеовышках
    for (string l : link)
    {
        // получение со сгенерированной ссылки информации о средней сегодняшней температуры
        string html_link = GetWebsiteData(l);
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
            temperature.push_back(temp);

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

        latitude.push_back(stod(latitude));
        longitude.push_back(stod(longitude));
        altitude.push_back(stoi(altitude));

    }


    // удаление прошлой бд
    const char* filename = "statistic.sqlite";
    remove(filename);

    // подключение SQLite3 и создание новой бд
    sqlite3* db;
    int rc = sqlite3_open("statistic.sqlite", &db);

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
    rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr);

    for (size_t i = 0; i < link.size(); ++i)
    {
        // заполнение таблицы
        sqlite3_bind_text(stmt, 1, city[i].c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, temperature[i]);
        sqlite3_bind_double(stmt, 3, latitude[i]);
        sqlite3_bind_double(stmt, 4, longitude[i]);
        sqlite3_bind_int(stmt, 5, height[i]);

        rc = sqlite3_step(stmt);
        rc = sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // очистка памяти
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);
    xmlCleanupParser();

}
