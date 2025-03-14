#ifndef CARD_DB_H
#define CARD_DB_H
#include <vector>

#include "utils/card_recognition.h"


void initCardDB();

std::vector<CardUserInfo> getAllCard();

bool insertCardDB(int cardType, const std::string &cardNo,
                  const std::string &userId, const std::string &extra, int64_t *cardId);

bool updateCardDB(int cardType, const std::string &cardNo,
                  const std::string &userId, const std::string &extra);

bool deleteCardDB(const std::string &userId);


#endif //CARD_DB_H
