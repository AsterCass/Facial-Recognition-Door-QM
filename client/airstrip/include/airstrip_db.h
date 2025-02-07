#ifndef AIRSTRIP_DB_H
#define AIRSTRIP_DB_H

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
        explicit CommonBackendConfigDbManager(const std::string &dbPath);

        void upsertConfig(const std::string &name, std::string configValueJson);

        void deleteConfig(const std::string &name);

        std::string getConfig(const std::string &name);

    private:
        void initDb();

        SQLite::Database _db;
    };
}


#endif // AIRSTRIP_DB_H
