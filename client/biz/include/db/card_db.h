#ifndef CARD_DB_H
#define CARD_DB_H
#include <vector>

#include "utils/card_recognition.h"


void initCardDB();

std::vector<CardUserInfo> getAllCard();

bool insertCardDB(const CardUserInfo &info, int64_t *cardId);

bool updateCardDB(const CardUserInfo &info);

bool deleteCardDB(const std::string &userId);

bool disableCard(const std::string &cardNo, int isEnable);

bool disableCardUser(const std::string &userId, int isEnable);

bool voiceTmpCardUser(const std::string &userId, const std::string &voiceTmp);


#endif //CARD_DB_H
