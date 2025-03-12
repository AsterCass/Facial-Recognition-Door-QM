#ifndef NFC_TOOL_H
#define NFC_TOOL_H

#include <string>


struct NfcCardData {
    bool isExist;
    int cardType;
    std::string cardNo;
};


NfcCardData getCardData();

#endif  // NFC_TOOL_H
