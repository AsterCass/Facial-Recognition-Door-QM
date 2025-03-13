#include "db/card_db.h"
#include <SQLiteCpp/Database.h>


bool initializedCardDb = false;
SQLite::Database *_dbCard = nullptr;
