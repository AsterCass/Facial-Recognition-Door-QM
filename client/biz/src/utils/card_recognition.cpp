#include "utils/card_recognition.h"

#include <map>
#include <boost/json.hpp>
#include "airstrip_log.h"
#include "db/card_db.h"

bool initializedCardRec = false;
std::map<std::string, CardUserInfo> cardUserInfoMap = {};

using namespace std;


void loadCardDb() {
    if (initializedCardRec) {
        return;
    }

    logPrintln("Start load card db", airstrip::INFO, __FUNCTION__);

    const auto dbData = getAllCard();

    for (auto &userInfo: dbData) {
        logPrintln("Card load userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
        logPrintln("Card load cardNo = " + userInfo.cardNo, airstrip::INFO, __FUNCTION__);
        cardUserInfoMap[userInfo.cardNo] = userInfo;
        logPrintln("Card loaded userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    }

    logPrintln("Start loaded card db", airstrip::INFO, __FUNCTION__);

    initializedCardRec = true;
}

bool cardInsert(const CardUserInfo &userInfo) {
    if (!initializedCardRec) {
        return false;
    }
    logPrintln("Insert db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    logPrintln("Insert db cardNo = " + userInfo.cardNo, airstrip::INFO, __FUNCTION__);

    int64_t cardId = 0;
    const auto dbRet = insertCardDB(userInfo, &cardId);
    if (!dbRet) {
        return false;
    }

    cardUserInfoMap[userInfo.userId] = userInfo;

    logPrintln("Insert finish userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);

    return true;
}

bool cardDelete(const CardUserInfo &userInfo) {
    if (!initializedCardRec) {
        return false;
    }
    const auto dbRet = deleteCardDB(userInfo.userId);
    if (!dbRet) {
        return false;
    }

    vector<std::string> removeCardNos = {};
    for (auto &cardUserInfo: cardUserInfoMap) {
        if (cardUserInfo.second.userId == userInfo.userId) {
            removeCardNos.push_back(cardUserInfo.first);
        }
    }

    for (auto &cardId: removeCardNos) {
        cardUserInfoMap.erase(cardId);
    }

    logPrintln("Delete finish userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    return true;
}

bool cardUpdate(const CardUserInfo &userInfo) {
    if (!initializedCardRec) {
        return false;
    }

    logPrintln("Update db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);

    const auto dbRet = updateCardDB(userInfo);
    if (!dbRet) {
        return false;
    }

    vector<std::string> updateCardIds = {};
    for (auto &cardUserInfo: cardUserInfoMap) {
        if (cardUserInfo.second.userId == userInfo.userId) {
            cardUserInfo.second = userInfo;
        }
    }

    return true;
}

bool cardDisable(const std::string &userId, const std::string &cardNo, int isEnable) {
    if (!initializedCardRec) {
        return false;
    }

    const auto dbRet = cardNo.empty() ? disableCardUser(userId, isEnable) : disableCard(cardNo, isEnable);
    if (!dbRet) {
        return false;
    }

    if (cardNo.empty()) {
        for (auto &cardUserInfo: cardUserInfoMap) {
            if (cardUserInfo.second.userId == userId) {
                cardUserInfo.second.isEnable = isEnable;
            }
        }
    } else {
        cardUserInfoMap[cardNo].isEnable = isEnable;
    }

    return true;
}

bool cardVoiceTemplate(const std::string &userId, const std::string &voiceFeature) {
    if (!initializedCardRec) {
        return false;
    }

    const auto dbRet = voiceTmpCardUser(userId, voiceFeature);
    if (!dbRet) {
        return false;
    }

    for (auto &cardUserInfo: cardUserInfoMap) {
        if (cardUserInfo.second.userId == userId) {
            cardUserInfo.second.voiceTemplate = voiceFeature;
        }
    }

    return true;
}

void cardRecognition(const std::string &cardNo) {
    const auto it = cardUserInfoMap.find(cardNo);

    if (it == cardUserInfoMap.end()) {
        logPrintln("Not found cardNo = " + cardNo, airstrip::INFO, __FUNCTION__);
        return;
    }
    logPrintln("Found cardNo = " + cardNo + " userId = " + it->second.userId,
               airstrip::INFO, __FUNCTION__);
}
