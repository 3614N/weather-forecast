#define _CRT_SECURE_NO_WARNINGS

#include "libs.h"

using namespace std;

string time()
{
    const time_t tm = time(nullptr);
    char buf[64];
    strftime(buf, size(buf), "%d.%m.%Y", localtime(&tm));
    string now = buf;
    return now;

}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

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


void mainScraper()
{
    vector<string> link;
    vector<string> city;
    vector<double> temperature;
    vector<double> latitude;
    vector<double> longitude;
    vector<int> height;
    vector<string> tables;
    string s;

    ifstream file("links.txt");

    CURL* curl = curl_easy_init();
    string response;

    while (getline(file, s))
    {
        link.push_back(s);
    }
    file.close();

    sqlite3* db;
    int rc;

    rc = sqlite3_open("weather_database.sqlite", &db);

    vector<string> tableNames;
    vector<string> lastDates;

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table'", -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* tableName = sqlite3_column_text(stmt, 0);
        if (!tableName) {
            continue;
        }
        tableNames.push_back(reinterpret_cast<const char*>(tableName));
        string query = "SELECT date FROM ";
        query += reinterpret_cast<const char*>(tableName);
        query += " ORDER BY rowid DESC LIMIT 1;";

        sqlite3_stmt* stmt2;
        rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt2, nullptr);
        if (sqlite3_step(stmt2) == SQLITE_ROW) {
            const char* dateValue = reinterpret_cast<const char*>(sqlite3_column_text(stmt2, 0));
            if (dateValue) {
                lastDates.push_back(dateValue);
            }
        }

        sqlite3_finalize(stmt2);
    }

    sqlite3_finalize(stmt);

    vector<string> allLinks;

    sqlite3_stmt* stmt3;

    for (int w = 0; w < link.size(); w++)
    {
        string insert = "INSERT INTO "+ tableNames[w] +" (date, temperature) VALUES(? , ? )";
        rc = sqlite3_prepare_v2(db, insert.c_str(), -1, &stmt3, nullptr);

        int tableDay = stoi(lastDates[w].substr(0, 2));
        string tableMonth = lastDates[w].substr(3, 2);
        string nowMonth = time().substr(3, 2);
        string tableYear = lastDates[w].substr(6, 4);
        string nowYear = time().substr(6, 4);
        //cout << tableMonth << endl << nowMonth << endl;
        for (int p = stoi(tableYear); p <= stoi(nowYear); p++)
        {
            for (int q = stoi(tableMonth); q <= stoi(nowMonth); q++)
            {
                string mo = to_string(q);
                string ye = to_string(p);
                string l2 = link[w] + "&month=" + mo + "&year=" + ye;

                int k = 1;

                string html = GetWebsiteData(l2);
                const char* charLink = html.c_str();
                htmlDocPtr doc = htmlReadMemory(charLink, strlen(charLink), nullptr, nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
                xmlXPathContextPtr context = xmlXPathNewContext(doc);
                const char* xpathExpr = "//td[contains(@class, 'green-color')][1]";
                xmlXPathObjectPtr result = xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>(xpathExpr), context);
                xmlNodeSetPtr nodes = result->nodesetval;

                int day = 1;

                for (int i = 0; i < nodes->nodeNr; ++i) {
                    xmlChar* value = xmlNodeListGetString(doc, nodes->nodeTab[i]->xmlChildrenNode, 1);
                    if (value == nullptr)
                    {
                        if (((q == stoi(tableMonth)) && (p == stoi(tableYear)) && (day > tableDay)) || ((q < stoi(tableMonth)) || (p < stoi(tableYear))))
                        {
                            string zero = "";
                            string zero2 = "";
                            if (day < 10) zero += "0";
                            if (q < 10) zero2 += "0";

                            string first = zero + to_string(day) + "." + zero2 + mo + "." + ye;
                            const char* cFirst = first.c_str();

                            cout << cFirst << endl << -999.0 << endl;

                            sqlite3_bind_text(stmt3, 1, cFirst, -1, SQLITE_STATIC);
                            sqlite3_bind_double(stmt3, 2, -999.0);
                            rc = sqlite3_step(stmt3);
                            sqlite3_reset(stmt3);
                        }
                        ++day;
                    }
                    else if (k != 1)
                    {
                        if (((q == stoi(tableMonth)) && (p == stoi(tableYear)) && (day > tableDay)) || (q != stoi(tableMonth) || p != stoi(tableYear)))
                        {
                            string zero = "";
                            string zero2 = "";
                            if (day < 10) zero += "0";
                            if (q < 10) zero2 += "0";

                            string first = zero + to_string(day) + "." + zero2 + mo + "." + ye;
                            const char* cFirst = first.c_str();

                            string sTemp = string(reinterpret_cast<const char*>(value));
                            double temp = stod(sTemp);

                            cout << cFirst << endl << temp << endl;

                            sqlite3_bind_text(stmt3, 1, cFirst, -1, SQLITE_STATIC);
                            sqlite3_bind_double(stmt3, 2, temp);
                            rc = sqlite3_step(stmt3);
                            sqlite3_reset(stmt3);
                        }
                        ++day;
                    }
                    ++k;

                }

                xmlXPathFreeObject(result);
                xmlXPathFreeContext(context);
                xmlFreeDoc(doc);
                xmlCleanupParser();

            }
        }
        sqlite3_finalize(stmt3);
    }
    sqlite3_close(db);

}
