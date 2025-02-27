#ifndef AIRSTRIP_DB_H
#define AIRSTRIP_DB_H

#include <mutex>
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

namespace airstrip {
    struct CommonBackendConfig {
        std::string configName;
        std::string configValueJson;
        std::string updateTime;
    };

    class CommonBackendConfigDbManager {
    public:
        CommonBackendConfigDbManager() {
        };

        ~CommonBackendConfigDbManager() {
            if (_db != nullptr) {
                delete _db;
                _db = nullptr;
            }
        }

        void initDb(const std::string &dbPath);

        void upsertConfig(const std::string &name, const std::string &configValueJson) const;

        void deleteConfig(const std::string &name) const;

        std::string getConfig(const std::string &name) const;

    private:
        std::mutex mtx;
        bool isInitedDb = false;
        SQLite::Database *_db = nullptr;
    };
}


#endif // AIRSTRIP_DB_H
