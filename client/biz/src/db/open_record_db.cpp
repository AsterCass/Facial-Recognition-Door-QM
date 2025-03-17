#include "db/open_record_db.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"


bool initializedOpenRecordDb = false;
SQLite::Database *dbOpenRecord = nullptr;

using namespace std;


void initOpenRecordDB() {
    if (initializedOpenRecordDb) {
        return;
    }
    logPrintln("OpenRecord database initialization start", airstrip::INFO, __FUNCTION__);

    string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    dbOpenRecord = new SQLite::Database(appWorkDir + "db/open_record.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    try {
        SQLite::Transaction transaction(*dbOpenRecord);

        dbOpenRecord->exec(R"(
                CREATE TABLE IF NOT EXISTS `open_record` (
                    `id` INTEGER  PRIMARY KEY AUTOINCREMENT,
                    `user_id`  varchar(100) ,
                    `open_time` INTEGER ,
                    `open_mode` INTEGER ,
                    `open_result` INTEGER DEFAULT '0',
                    `uploaded` INTEGER DEFAULT '0',
                    `extra` text  DEFAULT '{}',
                    `create_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP
                )
            )");

        transaction.commit();
    } catch (const SQLite::Exception &e) {
        logPrintln("OpenRecord database initialization failed: " + string(e.what()),
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }
    initializedOpenRecordDb = true;
}


std::vector<OpenRecordInfo> getAllRecordNotUpload() {
    if (!initializedOpenRecordDb || nullptr == dbOpenRecord) {
        return {};
    }

    return {};
}

void uploadedOpenRecordDB(const std::vector<int64_t> &recordIds) {
    if (!initializedOpenRecordDb || nullptr == dbOpenRecord) {
        return;
    }
}

bool insertOpenRecordDB(const OpenRecordInfo &info) {
    if (!initializedOpenRecordDb || nullptr == dbOpenRecord) {
        return false;
    }


    return true;
}

void deleteYearRecordDB() {
    if (!initializedOpenRecordDb || nullptr == dbOpenRecord) {
        return;
    }
}
