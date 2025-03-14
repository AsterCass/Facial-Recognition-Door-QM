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
    boost::json::object cardDbExtraJson;
    cardDbExtraJson["cardNo"] = userInfo.cardNo;
    cardDbExtraJson["cardType"] = userInfo.cardType;
    cardDbExtraJson["userId"] = userInfo.userId;
    cardDbExtraJson["startTime"] = userInfo.startTime;
    cardDbExtraJson["endTime"] = userInfo.endTime;
    cardDbExtraJson["isEnable"] = userInfo.isEnable;
    cardDbExtraJson["voiceTemplate"] = userInfo.voiceTemplate;
    const auto dbRet = insertCardDB(userInfo.cardType, userInfo.cardNo, userInfo.userId,
                                    serialize(cardDbExtraJson), &cardId);

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

    boost::json::object cardDbExtraJson;
    cardDbExtraJson["cardNo"] = userInfo.cardNo;
    cardDbExtraJson["cardType"] = userInfo.cardType;
    cardDbExtraJson["userId"] = userInfo.userId;
    cardDbExtraJson["startTime"] = userInfo.startTime;
    cardDbExtraJson["endTime"] = userInfo.endTime;
    cardDbExtraJson["isEnable"] = userInfo.isEnable;
    cardDbExtraJson["voiceTemplate"] = userInfo.voiceTemplate;
    const auto dbRet = updateCardDB(userInfo.cardType, userInfo.cardNo, userInfo.userId,
                                    serialize(cardDbExtraJson));
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

void cardRecognition(const std::string &cardNo) {
    const auto it = cardUserInfoMap.find(cardNo);

    if (it == cardUserInfoMap.end()) {
        logPrintln("Not found cardNo = " + cardNo, airstrip::INFO, __FUNCTION__);
        return;
    }
    logPrintln("Found cardNo = " + cardNo + " userId = " + it->second.userId,
               airstrip::INFO, __FUNCTION__);
}
