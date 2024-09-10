#ifndef ENVPARSER_H
#define ENVPARSER_H

#include <string>
#include <unordered_map>

using namespace std;

class envParser
{
public:
    envParser(envParser &other) = delete;
    void operator=(const envParser &) = delete;
    static envParser *getInstance();

    string getValue(const string key) const;

private:
    envParser();
    ~envParser();
    void loadEnvFile();

    static envParser *instance;
    unordered_map<string, string> envMap;
};

#endif // ENVPARSER_H
