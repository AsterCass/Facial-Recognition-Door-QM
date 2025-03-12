#include "airstrip_db.h"

#include <airstrip_log.h>
#include <iomanip>
#include <sstream>

using namespace std;

namespace airstrip {
    void CommonBackendConfigDbManager::initDb(const std::string &dbPath) {
        if (!mtx.try_lock() || _db != nullptr) {
            logPrintln("Database initialization has finished", WARN, __FUNCTION__);
            return;
        }
        _db = new SQLite::Database(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        try {
            SQLite::Transaction transaction(*_db);

            _db->exec(R"(
                CREATE TABLE IF NOT EXISTS common_backend_config (
                    config_name TEXT PRIMARY KEY,
                    config_value_json TEXT NOT NULL,
                    update_time DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            )");

            transaction.commit();
        } catch (const SQLite::Exception &e) {
            logPrintln("Database initialization failed: " + string(e.what()),
                       CRITICAL, __FUNCTION__);
            exit(-1);
        }
    }

    void CommonBackendConfigDbManager::upsertConfig(const string &name, const string &configValueJson) const {
        if (name.empty()) {
            return;
        }
        try {
            SQLite::Transaction transaction(*_db);

            SQLite::Statement query(
                *_db, "SELECT * FROM common_backend_config WHERE config_name = ?");
            query.bind(1, name);

            if (query.executeStep()) {
                SQLite::Statement update(
                    *_db, "UPDATE common_backend_config SET config_value_json = ?, "
                    "update_time = (datetime('now', 'localtime')) "
                    "WHERE config_name = ?");
                update.bind(1, configValueJson);
                update.bind(2, name);
                update.exec();
            } else {
                SQLite::Statement insert(
                    *_db, "INSERT INTO common_backend_config (config_name, config_value_json) VALUES (?, ?)");
                insert.bind(1, name);
                insert.bind(2, configValueJson);
                insert.exec();
            }
            transaction.commit();
        } catch (const SQLite::Exception &e) {
            logPrintln("Upsert failed: " + string(e.what()),
                       CRITICAL, __FUNCTION__);
        }
    }


    void CommonBackendConfigDbManager::deleteConfig(const string &name) const {
        if (name.empty()) {
            return;
        }
        try {
            SQLite::Transaction transaction(*_db);

            SQLite::Statement query(*_db, "DELETE FROM common_backend_config WHERE config_name = ?");
            query.bind(1, name);
            query.exec();

            transaction.commit();
        } catch (const SQLite::Exception &e) {
            logPrintln("Delete failed: " + string(e.what()),
                       CRITICAL, __FUNCTION__);
        }
    }

    string CommonBackendConfigDbManager::getConfig(const string &name) const {
        string ret;
        if (name.empty()) {
            return ret;
        }
        try {
            SQLite::Statement query(
                *_db, "SELECT config_value_json FROM common_backend_config WHERE config_name = ?");
            query.bind(1, name);

            while (query.executeStep()) {
                ret = query.getColumn(0).getString();
                break;
            }
        } catch (const SQLite::Exception &e) {
            logPrintln("Query failed: " + string(e.what()),
                       CRITICAL, __FUNCTION__);
        }
        return ret;
    }
}
