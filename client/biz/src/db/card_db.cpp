#include "db/card_db.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
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
                   `is_enable` INTEGER  DEFAULT 1,
                   `voice_template`  varchar(100)  DEFAULT '',
                   `start_time` INTEGER  DEFAULT 0,
                   `end_time` INTEGER  DEFAULT 0,
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
            const int isEnable = query.getColumn(4);
            const std::string voiceTemplate = query.getColumn(5);
            const int64_t startTime = query.getColumn(6);
            const int64_t endTime = query.getColumn(7);

            userInfo.cardId = cardId;
            userInfo.cardNo = cardNo;
            userInfo.cardType = cardType;
            userInfo.userId = userId;
            userInfo.startTime = startTime;
            userInfo.endTime = endTime;
            userInfo.isEnable = isEnable;
            userInfo.voiceTemplate = voiceTemplate;

            ret.emplace_back(userInfo);
        }
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db insert failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return ret;
    }
    return ret;
}

bool insertCardDB(const CardUserInfo &info, int64_t *cardId) {
    if (!initializedCardDb || nullptr == dbCard) {
        return false;
    }
    try {
        SQLite::Statement insert(
            *dbCard,
            "INSERT INTO card (user_id, card_no, card_type, start_time, end_time)"
            " VALUES (?, ?, ?, ?, ?)");

        insert.bind(1, info.userId);
        insert.bind(2, info.cardNo);
        insert.bind(3, info.cardType);
        insert.bind(4, info.startTime);
        insert.bind(5, info.endTime);

        insert.exec();

        *cardId = dbCard->getLastInsertRowid();
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db insert failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool updateCardDB(const CardUserInfo &info) {
    if (!initializedCardDb || nullptr == dbCard) {
        return false;
    }

    try {
        SQLite::Statement update(
            *dbCard, "UPDATE card SET card_no = ?, card_type = ?, start_time = ?, end_time = ?"
            "update_time = (datetime('now', 'localtime')) "
            "WHERE user_id = ?");;

        update.bind(1, info.cardNo);
        update.bind(2, info.cardType);
        update.bind(3, info.startTime);
        update.bind(4, info.endTime);
        update.bind(5, info.userId);

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


bool disableCard(const std::string &cardNo, int isEnable) {
    if (!initializedCardDb || nullptr == dbCard) {
        return false;
    }

    try {
        SQLite::Statement update(
            *dbCard, "UPDATE card SET is_enable = ? "
            "update_time = (datetime('now', 'localtime')) "
            "WHERE card_no = ?");;

        update.bind(1, isEnable);
        update.bind(2, cardNo);

        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool disableCardUser(const std::string &userId, int isEnable) {
    if (!initializedCardDb || nullptr == dbCard) {
        return false;
    }

    try {
        SQLite::Statement update(
            *dbCard, "UPDATE card SET is_enable = ? "
            "update_time = (datetime('now', 'localtime')) "
            "WHERE user_id = ?");;

        update.bind(1, isEnable);
        update.bind(2, userId);

        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool voiceTmpCardUser(const std::string &userId, const std::string &voiceTmp) {
    if (!initializedCardDb || nullptr == dbCard) {
        return false;
    }

    try {
        SQLite::Statement update(
            *dbCard, "UPDATE card SET voice_template = ? "
            "update_time = (datetime('now', 'localtime')) "
            "WHERE user_id = ?");;

        update.bind(1, voiceTmp);
        update.bind(2, userId);

        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Card db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}
