#ifndef OPEN_RECORD_DB_H
#define OPEN_RECORD_DB_H
#include <string>
#include <vector>


struct OpenRecordInfo {
    int64_t openRecordId;
    std::string userId;
    int64_t openTime;
    int openResult;
    int openMode;
    int64_t faceId;
    std::string cardNo;
    int cardType;
    int uploaded;
};


void initOpenRecordDB();

bool insertOpenRecordDB(const OpenRecordInfo &info);

std::vector<OpenRecordInfo> getAllRecordNotUpload();

bool uploadedOpenRecordDB(const std::vector<int64_t> &recordIds);

void deleteYearRecordDB();


#endif //OPEN_RECORD_DB_H
