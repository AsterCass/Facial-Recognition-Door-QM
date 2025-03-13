#include "utils/face_recognition.h"
#include "db/face_db.h"
#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"

bool initializedFaceRec = false;

using namespace std;
#ifndef WIN32

#include "inspireface.h"
#include "intypedef.h"
#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>
#include "facedetectcnn.h"
#include <boost/json.hpp>


HFSession faceRecognitionSession = nullptr;

std::map<int64_t, FaceUserInfo> faceUserInfoMap = {};

std::string serializeHFFaceFeature(const HFFaceFeature &feature) {
    if (feature.size < 0) {
        // 检查 size 有效性
        return "";
    }
    const size_t data_bytes = feature.size * sizeof(float);
    const size_t total_bytes = sizeof(feature.size) + data_bytes;
    std::vector<char> buffer(total_bytes);

    // 拷贝 size
    memcpy(buffer.data(), &feature.size, sizeof(feature.size));

    // 拷贝 data（仅当 size > 0 时）
    if (data_bytes > 0) {
        if (!feature.data) {
            // data 指针无效
            return "";
        }
        memcpy(buffer.data() + sizeof(feature.size), feature.data, data_bytes);
    }

    return std::string(buffer.data(), buffer.size());
}

HFFaceFeature deserializeHFFaceFeature(const std::string &str) {
    HFFaceFeature feature;
    feature.size = 0;
    feature.data = nullptr;

    if (str.size() < sizeof(int)) {
        // 数据不足以读取 size
        return feature;
    }

    // 提取 size
    int size;
    memcpy(&size, str.data(), sizeof(int));
    if (size < 0) {
        // 无效的 size
        return feature;
    }

    // 验证数据长度
    const size_t expected_bytes = sizeof(int) + size * sizeof(float);
    if (str.size() != expected_bytes) {
        return feature;
    }

    feature.size = size;
    if (size > 0) {
        feature.data = new float[size];
        memcpy(feature.data, str.data() + sizeof(int), size * sizeof(float));
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

        logPrintln("Face loaded userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    }
}


bool faceInsert(const std::string &address, const FaceUserInfo &userInfo) {
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

bool faceInsert(const cv::Mat &pic, const FaceUserInfo &userInfo) {
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
    faceDbExtraJson["userId"] = userInfo.userId;
    faceDbExtraJson["startTime"] = userInfo.startTime;
    faceDbExtraJson["endTime"] = userInfo.endTime;
    faceDbExtraJson["isEnable"] = userInfo.isEnable;
    faceDbExtraJson["voiceTemplate"] = userInfo.voiceTemplate;
    const auto featureStr = serializeHFFaceFeature(feature);
    const auto dbRet = insertFaceDB(userInfo.userId, serialize(faceDbExtraJson), featureStr, &faceId);
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


bool faceUpdate(const cv::Mat &pic, const FaceUserInfo &userInfo) {
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

    boost::json::object faceDbExtraJson;
    faceDbExtraJson["userId"] = userInfo.userId;
    faceDbExtraJson["startTime"] = userInfo.startTime;
    faceDbExtraJson["endTime"] = userInfo.endTime;
    faceDbExtraJson["isEnable"] = userInfo.isEnable;
    faceDbExtraJson["voiceTemplate"] = userInfo.voiceTemplate;
    const auto dbRet = updateFaceDB(userInfo.userId, serialize(faceDbExtraJson), "");
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

    if (searchResult.id <= 0) {
        // todo not match
        HFReleaseImageStream(stream);
        return;
    }

    logPrintln("Face recognition ret id = " + to_string(searchResult.id) + " " + to_string(confidence),
               airstrip::INFO, __FUNCTION__);
    // todo match


    HFReleaseImageStream(stream);
}

#else

void initFaceRecognition() {
    if (initializedFaceRec) {
        return;
    }
    initializedFaceRec = true;
}


bool faceInsert(const cv::Mat &pic, const FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }

    // ===================== check finish, start insert =====================

    int64_t faceId = 0;
    insertFaceDB(userInfo.userId, "", "", &faceId);

    logPrintln("Insert db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    logPrintln("Insert db faceId = " + to_string(faceId), airstrip::INFO, __FUNCTION__);

    return true;
}

bool faceDelete(const FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }

    deleteFaceDB(userInfo.userId);

    logPrintln("Delete db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    return true;
}

bool faceUpdate(const cv::Mat &pic, const FaceUserInfo &userInfo) {
    if (!initializedFaceRec) {
        return false;
    }


    updateFaceDB(userInfo.userId, "", "");

    logPrintln("Update db userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    return true;
}


#endif
