#include "db/face_db.h"

#include <SQLiteCpp/Database.h>

#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include <boost/json.hpp>
#include "config/config.h"

bool initializedFaceDb = false;
SQLite::Database *dbFace = nullptr;

using namespace std;

void initFaceDB() {
    if (initializedFaceDb) {
        return;
    }
    logPrintln("Face database initialization start", airstrip::INFO, __FUNCTION__);

    string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    dbFace = new SQLite::Database(appWorkDir + "db/face.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    try {
        SQLite::Transaction transaction(*dbFace);

        dbFace->exec(R"(
                CREATE TABLE IF NOT EXISTS `face` (
                   `face_id` INTEGER  PRIMARY KEY AUTOINCREMENT,
                   `user_id`  varchar(100)  DEFAULT '',
                   `is_enable` INTEGER  DEFAULT 1,
                   `voice_template`  varchar(100)  DEFAULT '',
                   `start_time` INTEGER  DEFAULT 0,
                   `end_time` INTEGER  DEFAULT 0,
                   `face_address`  varchar(100)  DEFAULT '',
                   `feature` text  DEFAULT '',
                   `extra` text  DEFAULT '{}',
                   `create_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ,
                   `update_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP
                )
            )");

        transaction.commit();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face database initialization failed: " + string(e.what()),
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }
    initializedFaceDb = true;
}

std::vector<FaceUserInfo> getAllFace() {
    if (!initializedFaceDb || nullptr == dbFace) {
        return {};
    }
    std::vector<FaceUserInfo> ret = {};
    try {
        SQLite::Statement query(
            *dbFace, "SELECT * FROM face");


        while (query.executeStep()) {
            FaceUserInfo userInfo = {};
            const int64_t faceId = query.getColumn(0);
            const std::string userId = query.getColumn(1);
            const int isEnable = query.getColumn(2);
            const std::string voiceTemplate = query.getColumn(3);
            const int64_t startTime = query.getColumn(4);
            const int64_t endTime = query.getColumn(5);
            const std::string faceAddress = query.getColumn(6);
            const std::string feature = query.getColumn(7);

            userInfo.faceId = faceId;
            userInfo.faceAddress = faceAddress;
            userInfo.faceFeat = feature;
            userInfo.userId = userId;
            userInfo.startTime = startTime;
            userInfo.endTime = endTime;
            userInfo.isEnable = isEnable;
            userInfo.voiceTemplate = voiceTemplate;

            ret.emplace_back(userInfo);
        }
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db insert failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return ret;
    }
    return ret;
}

bool insertFaceDB(const FaceUserInfo &info, int64_t *faceId) {
    if (!initializedFaceDb || nullptr == dbFace) {
        return false;
    }
    try {
        SQLite::Statement insert(
            *dbFace,
            "INSERT INTO face (user_id, start_time, end_time, face_address, feature)"
            " VALUES (?, ?, ?, ?, ?)");

        insert.bind(1, info.userId);
        insert.bind(2, info.startTime);
        insert.bind(3, info.endTime);
        insert.bind(4, info.faceAddress);
        insert.bind(5, info.faceFeat);

        insert.exec();

        *faceId = dbFace->getLastInsertRowid();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db insert failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool updateFaceDB(const FaceUserInfo &info) {
    if (!initializedFaceDb || nullptr == dbFace) {
        return false;
    }

    try {
        SQLite::Statement update(
            *dbFace, "UPDATE face SET start_time = ?, end_time = ?, face_address = ?, feature = ? "
            "update_time = (datetime('now', 'localtime')) "
            "WHERE user_id = ?");;

        update.bind(1, info.startTime);
        update.bind(2, info.endTime);
        update.bind(3, info.faceAddress);
        update.bind(4, info.faceFeat);
        update.bind(5, info.userId);

        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool deleteFaceDB(const std::string &userId) {
    if (!initializedFaceDb || nullptr == dbFace) {
        return false;
    }
    try {
        SQLite::Statement del(*dbFace, "DELETE FROM face WHERE user_id = ?");
        del.bind(1, userId);
        del.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool disableFaceUser(const std::string &userId, const int isEnable) {
    if (!initializedFaceDb || nullptr == dbFace) {
        return false;
    }

    try {
        SQLite::Statement update(
            *dbFace, "UPDATE face SET is_enable = ? "
            "update_time = (datetime('now', 'localtime')) "
            "WHERE user_id = ?");;

        update.bind(1, isEnable);
        update.bind(2, userId);

        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool voiceTmpFaceUser(const std::string &userId, const std::string &voiceTmp) {
    if (!initializedFaceDb || nullptr == dbFace) {
        return false;
    }

    try {
        SQLite::Statement update(
            *dbFace, "UPDATE face SET voice_template = ? "
            "update_time = (datetime('now', 'localtime')) "
            "WHERE user_id = ?");;

        update.bind(1, voiceTmp);
        update.bind(2, userId);

        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}
