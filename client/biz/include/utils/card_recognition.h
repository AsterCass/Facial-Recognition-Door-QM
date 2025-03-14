#ifndef CARD_RECOGNITION_H
#define CARD_RECOGNITION_H

#include <string>

struct CardUserInfo {
    int64_t cardId;
    std::string cardNo;
    int cardType;
    std::string userId;
    int64_t startTime;
    int64_t endTime;
    bool isEnable;
    std::string voiceTemplate;
};

void loadCardDb();

bool cardInsert(const CardUserInfo &userInfo);

bool cardDelete(const CardUserInfo &userInfo);

bool cardUpdate(const CardUserInfo &userInfo);

bool cardDisable(const std::string &userId, const std::string &cardNo, int isEnable);

bool cardVoiceTemplate(const std::string &userId, const std::string &voiceFeature);

void cardRecognition(const std::string &cardNo);


#endif //CARD_RECOGNITION_H
