#include "envParser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

envParser *envParser::instance = nullptr;

envParser::envParser()
{
    loadEnvFile();
}

envParser *envParser::getInstance()
{
    if (instance == nullptr)
    {
        instance = new envParser();
    }
    return instance;
}

void envParser::loadEnvFile()
{
    ifstream file;

    file.open(".env");

    if (!file.is_open())
    {
        cout << "Error opening .env file" << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        size_t pos = line.find("=");
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        envMap[key] = value;
    }

    file.close();
}

string envParser::getValue(const string key) const
{
    return envMap.at(key);
}