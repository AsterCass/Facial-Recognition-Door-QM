#include "airstrip_db.h"

#include <airstrip_log.h>

using namespace std;

namespace airstrip {
    CommonBackendConfigDbManager::CommonBackendConfigDbManager(const string &dbPath)
        : _db(dbPath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
        initDb();
    }

    void CommonBackendConfigDbManager::initDb() {
        try {
            SQLite::Transaction transaction(_db);

            _db.exec(R"(
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

    void CommonBackendConfigDbManager::upsertConfig(const string &name, string configValueJson) {
        if (name.empty()) {
            return;
        }
        try {
            SQLite::Transaction transaction(_db);

            SQLite::Statement query(_db, "SELECT * FROM common_backend_config WHERE config_name = ?");
            query.bind(1, name);
            query.exec();

            if (query.getChanges() == 0) {
                // insert
            } else {
                //update
            }
        } catch (const SQLite::Exception &e) {
            logPrintln("Upsert failed: " + string(e.what()),
                       CRITICAL, __FUNCTION__);
        }
    }


    void CommonBackendConfigDbManager::deleteConfig(const string &name) {
    }

    string CommonBackendConfigDbManager::getConfig(const string &name) {
        return "";
    }
}
