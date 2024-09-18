#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

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
  void createDocument(const string &collectionName, const bsoncxx::document::value &document);
  boost::optional<bsoncxx::document::value> findDocument(const string &collectionName, const bsoncxx::document::value &filter);
  vector<bsoncxx::document::view> findDocuments(const string &collectionName, const bsoncxx::document::value &filter);
  bool updateDocument(const string &collectionName, const bsoncxx::document::value &filter, const bsoncxx::document::value &update);
  bool deleteDocument(const string &collectionName, const bsoncxx::document::value &filter);

private:
  mongoDBHandler();
  ~mongoDBHandler();

  static mongoDBHandler *instance;
  mongocxx::client *connection;
  mongocxx::instance m_instance{};
};