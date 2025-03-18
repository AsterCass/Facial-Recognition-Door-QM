#include "db/open_record_db.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"
#include <sstream>


bool initializedOpenRecordDb = false;
SQLite::Database *dbOpenRecord = nullptr;

using namespace std;


void initOpenRecordDB() {
    if (initializedOpenRecordDb) {
        return;
    }
    logPrintln("Open record database initialization start", airstrip::INFO, __FUNCTION__);

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
                    `face_id` INTEGER DEFAULT '0',
                    `card_no` varchar(100) DEFAULT '',
                    `card_type` INTEGER DEFAULT '0',
                    `uploaded` INTEGER DEFAULT '0',
                    `extra` text  DEFAULT '{}',
                    `create_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP
                )
            )");

        transaction.commit();
    } catch (const SQLite::Exception &e) {
        logPrintln("Open record database initialization failed: " + string(e.what()),
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }
    initializedOpenRecordDb = true;
}


std::vector<OpenRecordInfo> getAllRecordNotUpload() {
    if (!initializedOpenRecordDb || nullptr == dbOpenRecord) {
        return {};
    }

    std::vector<OpenRecordInfo> ret = {};
    try {
        SQLite::Statement query(
            *dbOpenRecord, "SELECT * FROM open_record where uploaded = 0 limit 100");

        while (query.executeStep()) {
            OpenRecordInfo recordInfo = {};
            const int64_t openRecordId = query.getColumn(0);
            const std::string userId = query.getColumn(1);
            const int64_t openTime = query.getColumn(2);
            const int openMode = query.getColumn(3);
            const int openResult = query.getColumn(4);
            const int64_t faceId = query.getColumn(5);
            const std::string cardNo = query.getColumn(6);
            const int cardType = query.getColumn(7);

            recordInfo.userId = userId;
            recordInfo.openMode = openMode;
            recordInfo.openTime = openTime;
            recordInfo.openResult = openResult;
            recordInfo.openRecordId = openRecordId;
            recordInfo.cardNo = cardNo;
            recordInfo.faceId = faceId;
            recordInfo.cardType = cardType;

            ret.emplace_back(recordInfo);
        }
    } catch (const SQLite::Exception &e) {
        logPrintln("Open record db get failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return ret;
    }
    return ret;
}

bool uploadedOpenRecordDB(const std::vector<int64_t> &recordIds) {
    if (!initializedOpenRecordDb || nullptr == dbOpenRecord) {
        return false;
    }

    if (recordIds.empty()) {
        return true;
    }

    try {
        ostringstream oss;
        for (const auto &recordId: recordIds) {
            oss << recordId << ",";
        }
        string idsStr = oss.str();
        idsStr.pop_back();

        const string sql = "UPDATE open_record SET uploaded = 1 WHERE id IN (" + idsStr + ")";
        logPrintln("Open record db sql :" + sql, airstrip::DEBUG, __FUNCTION__);

        SQLite::Statement update(*dbOpenRecord, sql);
        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Open record db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool insertOpenRecordDB(const OpenRecordInfo &info) {
    if (!initializedOpenRecordDb || nullptr == dbOpenRecord) {
        return false;
    }
    try {
        SQLite::Statement insert(
            *dbOpenRecord,
            "INSERT INTO open_record (user_id, open_time, open_mode, open_result, face_id, card_no, card_type)"
            " VALUES (?, ?, ?, ?, ?, ?, ?)");

        insert.bind(1, info.userId);
        insert.bind(2, info.openTime);
        insert.bind(3, info.openMode);
        insert.bind(4, info.openResult);
        insert.bind(5, info.faceId);
        insert.bind(6, info.cardNo);
        insert.bind(7, info.cardType);

        insert.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Open record db insert failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

void deleteYearRecordDB() {
    if (!initializedOpenRecordDb || nullptr == dbOpenRecord) {
        return;
    }

    try {
        SQLite::Statement del(*dbOpenRecord, "DELETE FROM open_record WHERE create_time < DATE('now', '-1 year')");
        del.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Open card db delete failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
    }
}
