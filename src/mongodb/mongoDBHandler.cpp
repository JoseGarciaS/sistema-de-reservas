#include "mongoDBHandler.h"
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

using namespace std;
using namespace bsoncxx::builder;

mongoDBHandler *mongoDBHandler::instance = nullptr;

mongoDBHandler::mongoDBHandler() : connection(nullptr) {}

mongoDBHandler::~mongoDBHandler()
{
    disconnect();
}

mongoDBHandler *mongoDBHandler::getInstance()
{
    if (instance == nullptr)
    {
        instance = new mongoDBHandler();
    }
    return instance;
}

void mongoDBHandler::connect(const string &uri)
{
    if (connection == nullptr)
    {
        mongocxx::options::client client_options;
        const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
        client_options.server_api_opts(api);

        connection = new mongocxx::client(mongocxx::uri(uri), client_options);
    }
}

void mongoDBHandler::disconnect()
{
    if (connection != nullptr)
    {
        delete connection;
        connection = nullptr;
    }
}

bool mongoDBHandler::ping()
{
    if (connection != nullptr)
    {
        const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));

        auto db = connection->database("Reservas");
        auto commandResult = db.run_command(ping_cmd.view());

        auto resultView = commandResult.view();

        if (resultView["ok"])
        {
            double ok = resultView["ok"].get_int32().value;
            if (ok == 1)
            {
                cout << "Pinged your deployment. Successfully connected to MongoDB!" << endl;
                return true;
            }
            else
            {
                cout << "Failed to ping your deployment. Could not connect to MongoDB!" << endl;
                return false;
            }
        }
    }

    return false;
}

void mongoDBHandler::createDocument(const string &collectionName, const bsoncxx::document::value &document)
{
    if (connection != nullptr)
    {
        auto db = connection->database("Reservas");
        auto collection = db.collection(collectionName);

        auto insert_one_result = collection.insert_one(document.view());

        assert(insert_one_result);
        auto doc_id = insert_one_result->inserted_id();
        assert(doc_id.type() == bsoncxx::type::k_oid);
        std::cout << "Inserted document ID: " << doc_id.get_oid().value.to_string() << std::endl;
    }
}

boost::optional<bsoncxx::document::value> mongoDBHandler::findDocument(const string &collectionName, const bsoncxx::document::value &filter)
{
    if (connection != nullptr)
    {
        auto db = connection->database("Reservas");
        auto collection = db.collection(collectionName);

        auto findOneResult = collection.find_one(filter.view());
        return findOneResult;
    }
    return boost::none;
}

bool mongoDBHandler::updateDocument(const string &collectionName, const bsoncxx::document::value &filter, const bsoncxx::document::value &update)
{
    if (connection != nullptr)
    {
        auto db = connection->database("Reservas");
        auto collection = db.collection(collectionName);

        auto updateOneResult = collection.update_one(filter.view(), update.view());
        assert(updateOneResult);

        bool updateSuccessful = updateOneResult && updateOneResult->modified_count() > 0;
        return updateSuccessful;
    }

    return false;
}

bool mongoDBHandler::deleteDocument(const string &collectionName, const bsoncxx::document::value &filter)
{
    if (connection != nullptr)
    {
        auto db = connection->database("Reservas");
        auto collection = db.collection(collectionName);

        auto deleteOneResult = collection.delete_one(filter.view());
        assert(deleteOneResult);

        bool deleteSuccessful = deleteOneResult && deleteOneResult->deleted_count() == 1;
        return deleteSuccessful;
    }
    return false;
}