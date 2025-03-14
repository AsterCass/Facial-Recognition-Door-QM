#include "db/card_db.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <boost/json.hpp>
#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"


bool initializedCardDb = false;
SQLite::Database *dbCard = nullptr;

using namespace std;


void initCardDB() {
    if (initializedCardDb) {
        return;
    }
    logPrintln("Card database initialization start", airstrip::INFO, __FUNCTION__);

    string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    dbCard = new SQLite::Database(appWorkDir + "db/card.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    try {
        SQLite::Transaction transaction(*dbCard);

        dbCard->exec(R"(
                CREATE TABLE IF NOT EXISTS `card` (
                   `card_id` INTEGER  PRIMARY KEY AUTOINCREMENT,
                   `card_type`  INTEGER  DEFAULT 0,
                   `card_no`  varchar(100)  DEFAULT '',
                   `user_id`  varchar(100)  DEFAULT '',
                   `extra` text  DEFAULT '{}',
                   `create_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ,
                   `update_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP
                )
            )");

        transaction.commit();
    } catch (const SQLite::Exception &e) {
        logPrintln("Card database initialization failed: " + string(e.what()),
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }
    initializedCardDb = true;
}

std::vector<CardUserInfo> getAllCard() {
    if (!initializedCardDb || nullptr == dbCard) {
        return {};
    }
    std::vector<CardUserInfo> ret = {};
    try {
        SQLite::Statement query(
            *dbCard, "SELECT * FROM card");


        while (query.executeStep()) {
            CardUserInfo userInfo = {};
            const int64_t cardId = query.getColumn(0);
            const int cardType = query.getColumn(1);
            const std::string cardNo = query.getColumn(2);
            const std::string userId = query.getColumn(3);
            const std::string extra = query.getColumn(4);

            auto extraJson = boost::json::parse(extra);
            userInfo.cardId = cardId;
            userInfo.cardNo = cardNo;
            userInfo.cardType = cardType;
            userInfo.userId = userId;
            userInfo.startTime = extraJson.at("startTime").as_int64();
            userInfo.endTime = extraJson.at("endTime").as_int64();
            userInfo.isEnable = extraJson.at("isEnable").as_bool();
            userInfo.voiceTemplate = extraJson.at("voiceTemplate").as_string().c_str();

            ret.emplace_back(userInfo);
        }
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db insert failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return ret;
    }
    return ret;
}

bool insertCardDB(const int cardType, const std::string &cardNo,
                  const std::string &userId, const std::string &extra, int64_t *cardId) {
    if (!initializedCardDb || nullptr == dbCard) {
        return false;
    }
    try {
        SQLite::Statement insert(
            *dbCard, "INSERT INTO card (card_type, card_no, user_id, extra) VALUES (?, ?, ?, ?)");
        insert.bind(1, cardType);
        insert.bind(2, cardNo);
        insert.bind(3, userId);
        insert.bind(4, extra);

        insert.exec();

        *cardId = dbCard->getLastInsertRowid();
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db insert failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool updateCardDB(const int cardType, const std::string &cardNo,
                  const std::string &userId, const std::string &extra) {
    if (!initializedCardDb || nullptr == dbCard) {
        return false;
    }

    try {
        SQLite::Statement update(
            *dbCard, "UPDATE card SET extra = ?, card_type = ?, card_no = ?"
            "update_time = (datetime('now', 'localtime')) "
            "WHERE user_id = ?");;

        update.bind(1, extra);
        update.bind(2, cardType);
        update.bind(3, cardNo);
        update.bind(4, userId);
        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool deleteCardDB(const std::string &userId) {
    if (!initializedCardDb || nullptr == dbCard) {
        return false;
    }
    try {
        SQLite::Statement del(*dbCard, "DELETE FROM card WHERE user_id = ?");
        del.bind(1, userId);
        del.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}
