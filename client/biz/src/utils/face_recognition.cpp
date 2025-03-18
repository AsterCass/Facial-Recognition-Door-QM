#include "utils/face_recognition.h"
#include "db/face_db.h"
#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"
#include "utils/scheduled_task.h"

bool initializedFaceRec = false;

using namespace std;

std::map<int64_t, FaceUserInfo> faceUserInfoMap = {};

#ifndef WIN32x

#include "inspireface.h"
#include "intypedef.h"
#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>
#include "facedetectcnn.h"
#include <boost/json.hpp>


HFSession faceRecognitionSession = nullptr;



std::string serializeHFFaceFeature(const HFFaceFeature &feature) {
    if (feature.size <= 0 || !feature.data) {
        return "";
    }
    std::ostringstream oss;
    oss << feature.size;
    for (int i = 0; i < feature.size; i++) {
        oss << "," << feature.data[i];
    }
    return oss.str();
}

HFFaceFeature deserializeHFFaceFeature(const std::string &str) {
    HFFaceFeature feature;
    feature.size = 0;
    feature.data = nullptr;

    std::istringstream iss(str);
    char comma;

    if (!(iss >> feature.size)) {
        feature.size = 0;
        return feature;
    }

    if (feature.size <= 0) {
        return feature;
    }

    feature.data = new float[feature.size];
    for (int i = 0; i < feature.size; i++) {
        if (!(iss >> comma >> feature.data[i])) {
            delete[] feature.data;
            feature.data = nullptr;
            feature.size = 0;
            return feature;
        }
    }

    return feature;
}

void freeHFFaceFeature(HFFaceFeature &feature) {
    delete[] feature.data;
    feature.data = nullptr;
    feature.size = 0;
}

void initFaceRecognition() {
    if (initializedFaceRec) {
        return;
    }

    logPrintln("Start init face model", airstrip::INFO, __FUNCTION__);

    string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    const string modelPath = appWorkDir + "model/Pikachu";
    HResult ret = HFLaunchInspireFace(modelPath.c_str());
    if (ret != HSUCCEED) {
        logPrintln("Load resource error: " + ret, airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    constexpr HOption option = HF_ENABLE_FACE_RECOGNITION;
    constexpr HFDetectMode detMode = HF_DETECT_MODE_ALWAYS_DETECT;
    constexpr HInt32 maxDetectNum = 1;
    constexpr HInt32 detectPixelLevel = 160;
    ret = HFCreateInspireFaceSessionOptional(
        option, detMode, maxDetectNum, detectPixelLevel, -1, &faceRecognitionSession);
    if (ret != HSUCCEED) {
        logPrintln("Create face context error: " + ret, airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    HFSessionSetTrackPreviewSize(faceRecognitionSession, detectPixelLevel);
    HFSessionSetFilterMinimumFacePixelSize(faceRecognitionSession, 30);

    HFFeatureHubConfiguration configuration;
    configuration.primaryKeyMode = HF_PK_MANUAL_INPUT;
    configuration.enablePersistence = 0;
    configuration.persistenceDbPath = nullptr;
    configuration.searchMode = HF_SEARCH_MODE_EAGER;
    configuration.searchThreshold = 0.48f;
    ret = HFFeatureHubDataEnable(configuration);
    if (ret != HSUCCEED) {
        logPrintln("Create face db error: " + ret, airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    logPrintln("Face model init finish", airstrip::INFO, __FUNCTION__);
    initializedFaceRec = true;
}

void loadFaceDb() {
    if (!initializedFaceRec) {
        return;
    }
    logPrintln("Start load face db", airstrip::INFO, __FUNCTION__);

    auto dbData = getAllFace();

    for (auto &userInfo: dbData) {
        logPrintln("Face load userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
        try {
            HFFaceFeatureIdentity identity = {};
            auto faceId = userInfo.faceId;
            identity.id = faceId;
            auto feat = deserializeHFFaceFeature(userInfo.faceFeat);
            identity.feature = &feat;

            const auto ret = HFFeatureHubInsertFeature(identity, &faceId);
            freeHFFaceFeature(feat);
            if (ret != HSUCCEED) {
                logPrintln("Face insert face error " + ret, airstrip::WARN, __FUNCTION__);
                continue;
            }

            userInfo.faceFeat = "";
            faceUserInfoMap[faceId] = userInfo;
        } catch (const exception &e) {
            ostringstream errMsg;
            errMsg << e.what();
            logPrintln("Face load error " + errMsg.str()
                       , airstrip::ERROR, __FUNCTION__);
        }
        logPrintln("Face loaded userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    }

    logPrintln("Start loaded face db", airstrip::INFO, __FUNCTION__);
}


bool faceInsert(const std::string &address, FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }
    const auto image = cv::imread(address);
    if (image.empty()) {
        logPrintln("Read pic error " + address, airstrip::WARN, __FUNCTION__);
        return false;
    }
    return faceInsert(image, userInfo);
}

bool faceInsert(const cv::Mat &pic, FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }

    logPrintln("Face insert userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);

    HFImageStream stream = nullptr;
    HFImageData imageData = {};
    imageData.data = pic.data;
    imageData.format = HF_STREAM_BGR;
    imageData.height = pic.rows;
    imageData.width = pic.cols;
    imageData.rotation = HF_CAMERA_ROTATION_0;
    HResult ret = HFCreateImageStream(&imageData, &stream);
    if (ret != HSUCCEED) {
        logPrintln("Face insert build image fail " + ret,
                   airstrip::WARN, __FUNCTION__);
    }

    HFMultipleFaceData multipleFaceData = {};
    ret = HFExecuteFaceTrack(faceRecognitionSession, stream, &multipleFaceData);
    if (ret != HSUCCEED) {
        logPrintln("Face insert track image fail " + ret,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return false;
    }

    const auto faceNum = multipleFaceData.detectedNum;
    if (faceNum <= 0) {
        // todo error throw
        logPrintln("Face insert face not found ", airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return false;
    }

    HFFaceFeature feature = {};
    ret = HFFaceFeatureExtract(faceRecognitionSession, stream,
                               multipleFaceData.tokens[0], &feature);
    if (ret != HSUCCEED) {
        logPrintln("Face insert feature extract fail " + ret,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return false;
    }

    // ===================== check finish, start insert =====================

    int64_t faceId = 0;
    boost::json::object faceDbExtraJson;
    const auto featureStr = serializeHFFaceFeature(feature);
    logPrintln("Insert db user feature = " + featureStr, airstrip::DEBUG, __FUNCTION__);
    userInfo.faceFeat = featureStr;
    const auto dbRet = insertFaceDB(userInfo, &faceId);
    if (!dbRet) {
        HFReleaseImageStream(stream);
        return false;
    }

    logPrintln("Insert db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    logPrintln("Insert db faceId = " + to_string(faceId), airstrip::INFO, __FUNCTION__);

    HFFaceFeatureIdentity identity = {};
    identity.id = faceId;
    identity.feature = &feature;
    ret = HFFeatureHubInsertFeature(identity, &faceId);
    if (ret != HSUCCEED) {
        logPrintln("Face insert face error " + ret, airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return false;
    }

    userInfo.faceId = faceId;
    faceUserInfoMap[faceId] = userInfo;

    logPrintln("Insert finish userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    logPrintln("Insert finish faceId = " + to_string(faceId), airstrip::INFO, __FUNCTION__);

    // todo callback

    HFReleaseImageStream(stream);

    return true;
}

bool faceDelete(const FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }

    // ===================== check finish, start insert =====================

    logPrintln("Delete db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);

    const auto dbRet = deleteFaceDB(userInfo.userId);
    if (!dbRet) {
        return false;
    }

    vector<int64_t> removeFaceIds = {};
    for (auto &faceUserInfo: faceUserInfoMap) {
        if (faceUserInfo.second.userId == userInfo.userId) {
            HFFeatureHubFaceRemove(faceUserInfo.first);
            removeFaceIds.push_back(faceUserInfo.first);
        }
    }

    for (auto &faceId: removeFaceIds) {
        faceUserInfoMap.erase(faceId);
    }

    logPrintln("Delete finish userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);

    return true;
}


bool faceUpdate(const cv::Mat &pic, FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }

    logPrintln("Face update userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);

    HFImageStream stream = nullptr;
    HFImageData imageData = {};
    imageData.data = pic.data;
    imageData.format = HF_STREAM_BGR;
    imageData.height = pic.rows;
    imageData.width = pic.cols;
    imageData.rotation = HF_CAMERA_ROTATION_0;
    HResult ret = HFCreateImageStream(&imageData, &stream);
    if (ret != HSUCCEED) {
        logPrintln("Face update build image fail " + ret,
                   airstrip::WARN, __FUNCTION__);
    }

    HFMultipleFaceData multipleFaceData = {};
    ret = HFExecuteFaceTrack(faceRecognitionSession, stream, &multipleFaceData);
    if (ret != HSUCCEED) {
        logPrintln("Face update track image fail " + ret,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return false;
    }

    const auto faceNum = multipleFaceData.detectedNum;
    if (faceNum <= 0) {
        // todo error throw
        logPrintln("Face update face not found ", airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return false;
    }

    HFFaceFeature feature = {};
    ret = HFFaceFeatureExtract(faceRecognitionSession, stream,
                               multipleFaceData.tokens[0], &feature);
    if (ret != HSUCCEED) {
        logPrintln("Face update feature extract fail " + ret,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return false;
    }

    // ===================== check finish, start insert =====================

    logPrintln("Update db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);


    const auto featureStr = serializeHFFaceFeature(feature);
    userInfo.faceFeat = featureStr;
    logPrintln("Update db user feature = " + featureStr, airstrip::DEBUG, __FUNCTION__);
    const auto dbRet = updateFaceDB(userInfo);
    if (!dbRet) {
        HFReleaseImageStream(stream);
        return false;
    }

    vector<int64_t> updateFaceIds = {};
    for (auto &faceUserInfo: faceUserInfoMap) {
        if (faceUserInfo.second.userId == userInfo.userId) {
            HFFaceFeatureIdentity identity = {};
            identity.id = faceUserInfo.first;
            identity.feature = &feature;
            HFFeatureHubFaceUpdate(identity);
            faceUserInfo.second = userInfo;
        }
    }

    logPrintln("Update finish userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);

    return true;
}

bool faceDisable(const std::string &userId, int isEnable) {
    if (!initializedFaceRec) {
        return false;
    }

    const auto dbRet = disableFaceUser(userId, isEnable);
    if (!dbRet) {
        return false;
    }

    for (auto &cardUserInfo: faceUserInfoMap) {
        if (cardUserInfo.second.userId == userId) {
            cardUserInfo.second.isEnable = isEnable;
        }
    }

    return true;
}

bool faceVoiceTemplate(const std::string &userId, const std::string &voiceFeature) {
    if (!initializedFaceRec) {
        return false;
    }

    const auto dbRet = voiceTmpFaceUser(userId, voiceFeature);
    if (!dbRet) {
        return false;
    }

    for (auto &cardUserInfo: faceUserInfoMap) {
        if (cardUserInfo.second.userId == userId) {
            cardUserInfo.second.voiceTemplate = voiceFeature;
        }
    }

    return true;
}

bool faceDetect(const cv::Mat &frame, cv::Rect &rect, int orgCols, int orgRows) {
    bool ret = false;
    if (!initializedFaceRec) {
        return ret;
    }

    const int *pResults = nullptr;
    auto *pBuffer = static_cast<unsigned char *>(malloc(0x9000));
    pResults = facedetect_cnn(pBuffer, frame.data, frame.cols, frame.rows, frame.step);
    const int faceNum = pResults ? *pResults : 0;

    if (faceNum > 0) {
        ret = true;

        // 最大人脸
        const auto *p = (short *) (pResults + 1);
        const int x = p[1];
        const int y = p[2];
        const int w = p[3];
        const int h = p[4];

        // 校正
        const int maxWidth = frame.cols;
        const int maxHeight = frame.rows;
        const int faceX = std::max(0, x);
        const int faceY = std::max(0, y);
        int faceW = std::max(0, w);
        int faceH = std::max(0, h);
        faceW = faceX + faceW > maxWidth ? maxWidth - faceX : faceW;
        faceH = faceY + faceH > maxHeight ? maxHeight - faceY : faceH;

        // 缩放前对应人形方框所在区域
        const int origX = static_cast<int>(faceX / IR_SCALE);
        const int origY = static_cast<int>(faceY / IR_SCALE);
        const int origWidth = static_cast<int>(faceW / IR_SCALE);
        const int origHeight = static_cast<int>(faceH / IR_SCALE);

        // 二次校正
        rect.x = std::min(origX, orgCols);
        rect.y = std::min(origY, orgRows);;
        rect.width = std::min(orgCols - origX, origWidth);
        rect.height = std::min(orgRows - origY, origHeight);
    }

    free(pBuffer);
    pBuffer = nullptr;
    return ret;
}

void faceRecognition(const cv::Mat &frame, const cv::Rect &rect) {
    if (!initializedFaceRec) {
        return;
    }

    HFImageStream stream = nullptr;
    HFImageData imageData = {};
    imageData.data = frame.data;
    imageData.format = HF_STREAM_BGR;
    imageData.height = frame.rows;
    imageData.width = frame.cols;
    imageData.rotation = HF_CAMERA_ROTATION_0;
    HResult ret = HFCreateImageStream(&imageData, &stream);
    if (ret != HSUCCEED) {
        logPrintln("Face recognition build image fail " + ret,
                   airstrip::WARN, __FUNCTION__);
    }

    HFMultipleFaceData multipleFaceData = {};
    ret = HFExecuteFaceTrack(faceRecognitionSession, stream, &multipleFaceData);
    if (ret != HSUCCEED) {
        logPrintln("Face recognition track image fail " + ret,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return;
    }

    const auto faceNum = multipleFaceData.detectedNum;
    logPrintln("Num of face: " + to_string(faceNum), airstrip::INFO, __FUNCTION__);

    if (multipleFaceData.detectedNum <= 0) {
        // logPrintln("Face recognition face not found",airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return;
    }

    HFFaceFeature feature = {};
    ret = HFFaceFeatureExtract(faceRecognitionSession, stream,
                               multipleFaceData.tokens[0], &feature);
    if (ret != HSUCCEED) {
        logPrintln("Face recognition feature extract fail " + ret,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return;
    }

    HFloat confidence;
    HFFaceFeatureIdentity searchResult = {};
    ret = HFFeatureHubFaceSearch(feature, &confidence, &searchResult);
    if (ret != HSUCCEED) {
        logPrintln("Face recognition feature search fail " + ret,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return;
    }

    if (searchResult.id <= 0 || faceUserInfoMap.find(searchResult.id) == faceUserInfoMap.end()) {
        ScheduledTask::sendFaceRegRes({});
        HFReleaseImageStream(stream);
        return;
    }

    logPrintln("Face recognition ret id = " + to_string(searchResult.id) + " " + to_string(confidence),
               airstrip::INFO, __FUNCTION__);
    ScheduledTask::sendFaceRegRes(faceUserInfoMap[searchResult.id]);

    HFReleaseImageStream(stream);
}

#else

void initFaceRecognition() {
    if (initializedFaceRec) {
        return;
    }
    initializedFaceRec = true;
}

void loadFaceDb() {
    if (!initializedFaceRec) {
        return;
    }
    logPrintln("Start load face db", airstrip::INFO, __FUNCTION__);

    auto dbData = getAllFace();

    for (auto &userInfo: dbData) {
        logPrintln("Face load userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
        try {
            userInfo.faceFeat = "";
            faceUserInfoMap[userInfo.faceId] = userInfo;
        } catch (const exception &e) {
            ostringstream errMsg;
            errMsg << e.what();
            logPrintln("Face load error " + errMsg.str()
                       , airstrip::ERROR, __FUNCTION__);
        }
        logPrintln("Face loaded userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    }

    logPrintln("Start loaded face db", airstrip::INFO, __FUNCTION__);
}


bool faceInsert(const cv::Mat &pic, FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }

    // ===================== check finish, start insert =====================

    int64_t faceId = 0;
    insertFaceDB(userInfo, &faceId);

    userInfo.faceFeat = "";
    faceUserInfoMap[faceId] = userInfo;

    logPrintln("Insert db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    logPrintln("Insert db faceId = " + to_string(faceId), airstrip::INFO, __FUNCTION__);

    return true;
}

bool faceDelete(const FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }

    deleteFaceDB(userInfo.userId);

    vector<int64_t> removeFaceIds = {};
    for (auto &faceUserInfo: faceUserInfoMap) {
        if (faceUserInfo.second.userId == userInfo.userId) {
            removeFaceIds.push_back(faceUserInfo.first);
        }
    }

    for (auto &faceId: removeFaceIds) {
        faceUserInfoMap.erase(faceId);
    }

    logPrintln("Delete db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    return true;
}

bool faceUpdate(const cv::Mat &pic, FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }

    updateFaceDB(userInfo);

    for (auto &faceUserInfo: faceUserInfoMap) {
        if (faceUserInfo.second.userId == userInfo.userId) {
            userInfo.faceFeat = "";
            faceUserInfo.second = userInfo;
        }
    }

    logPrintln("Update db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    return true;
}

bool faceDisable(const std::string &userId, int isEnable) {
    if (!initializedFaceRec) {
        return false;
    }

    const auto dbRet = disableFaceUser(userId, isEnable);
    if (!dbRet) {
        return false;
    }

    for (auto &cardUserInfo: faceUserInfoMap) {
        if (cardUserInfo.second.userId == userId) {
            cardUserInfo.second.isEnable = isEnable;
        }
    }

    return true;
}

bool faceVoiceTemplate(const std::string &userId, const std::string &voiceFeature) {
    if (!initializedFaceRec) {
        return false;
    }

    const auto dbRet = voiceTmpFaceUser(userId, voiceFeature);
    if (!dbRet) {
        return false;
    }

    for (auto &cardUserInfo: faceUserInfoMap) {
        if (cardUserInfo.second.userId == userId) {
            cardUserInfo.second.voiceTemplate = voiceFeature;
        }
    }

    return true;
}


#endif
