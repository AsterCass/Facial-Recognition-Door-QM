#include "db/face_db.h"

#include <SQLiteCpp/Database.h>

#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"

bool initializedFaceDb = false;
SQLite::Database *_dbFace = nullptr;

using namespace std;

void initFaceDB() {
    if (initializedFaceDb) {
        return;
    }
    logPrintln("Face database initialization start", airstrip::INFO, __FUNCTION__);

    string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    _dbFace = new SQLite::Database(appWorkDir + "db/face.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    try {
        SQLite::Transaction transaction(*_dbFace);

        _dbFace->exec(R"(
                CREATE TABLE IF NOT EXISTS `face` (
                   `face_id` INTEGER  PRIMARY KEY AUTOINCREMENT,
                   `user_id`  varchar(100)  DEFAULT '',
                   `extra` text  DEFAULT '{}',
                   `feature` text  DEFAULT '',
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

bool insertFaceDB(const std::string &userId, const std::string &extra, const std::string &feature, int64_t *faceId) {
    if (!initializedFaceDb || nullptr == _dbFace) {
        return false;
    }
    try {
        SQLite::Statement insert(
            *_dbFace, "INSERT INTO face (user_id, extra, feature) VALUES (?, ?, ?)");
        insert.bind(1, userId);
        insert.bind(2, extra);
        insert.bind(3, feature);
        insert.exec();

        *faceId = _dbFace->getLastInsertRowid();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db insert failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool updateFaceDB(const std::string &userId, const std::string &extra,
                  const std::string &feature) {
    if (!initializedFaceDb || nullptr == _dbFace) {
        return false;
    }

    try {
        SQLite::Statement update(
            *_dbFace, "UPDATE face SET extra = ?, feature = ?,"
            "update_time = (datetime('now', 'localtime')) "
            "WHERE user_id = ?");;

        update.bind(1, extra);
        update.bind(2, feature);
        update.bind(3, userId);
        update.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}

bool deleteFaceDB(const std::string &userId) {
    if (!initializedFaceDb || nullptr == _dbFace) {
        return false;
    }
    try {
        SQLite::Statement del(*_dbFace, "DELETE FROM face WHERE user_id = ?");
        del.bind(1, userId);
        del.exec();
    } catch (const SQLite::Exception &e) {
        logPrintln("Face db update failed: " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
        return false;
    }
    return true;
}
