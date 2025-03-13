#ifndef CARD_RECOGNITION_H
#define CARD_RECOGNITION_H

#include <string>

struct CardUserInfo {
    std::string cardId;
    std::string cardType;
    std::string userId;
    int64_t startTime;
    int64_t endTime;
    bool isDisable;
    std::string voiceTemplate;
};


#endif //CARD_RECOGNITION_H
