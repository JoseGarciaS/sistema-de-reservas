#pragma once
#include <cstdint>
#include <string>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace std;

class mongoDBHandler
{
public:
  mongoDBHandler(mongoDBHandler &other) = delete;
  void operator=(const mongoDBHandler &) = delete;
  static mongoDBHandler *getInstance();

  void connect(const string &uri);
  void disconnect();

  bool ping();
  void createDocument(const string &collectionName, const bsoncxx::v_noabi::document::value &document);
  void readDocument(const string &collectionName, const bsoncxx::document::view &filter);
  void updateDocument(const string &collectionName, const bsoncxx::document::view &filter, const bsoncxx::document::view &update);
  void deleteDocument(const string &collectionName, const bsoncxx::document::view &filter);

private:
  mongoDBHandler();
  ~mongoDBHandler();

  static mongoDBHandler *instance;
  mongocxx::client *connection;
  mongocxx::instance m_instance{};
};