#include "utils/face_recognition.h"
#include <utils/general_utils.h>
#include "db/face_db.h"
#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"
#include "utils/scheduled_task.h"

bool initializedFaceRec = false;

using namespace std;

std::map<int64_t, FaceUserInfo> faceUserInfoMap = {};

#ifndef WIN32

#include "inspireface.h"
#include "intypedef.h"
#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include "airstrip_command.h"
#include "facedetectcnn.h"
#include <boost/json.hpp>


HFSession faceRecognitionSession = nullptr;

void updateLight(int expose, int gain, int light) {
    ostringstream updateExposeGainCmd;
    updateExposeGainCmd << "sh " << g_appWorkDir + "script/linux/reset_expose.sh "
            << expose << " "
            << gain << " && sh "
            << g_appWorkDir + "script/linux/reset_light.sh "
            << light;


    logPrintln("Current cmd : " + updateExposeGainCmd.str(),
               airstrip::DEBUG, __FUNCTION__);
    airstrip::execCommand(updateExposeGainCmd.str());
}


void closeLight() {
    if (currentIsNight() && g_camAutoLight) {
        logPrintln("To Close light", airstrip::INFO, __FUNCTION__);

        for (int count = EXPOSE_AND_GAIN_PARAM.size() - 1; count >= 0; count--) {
            if (EXPOSE_AND_GAIN_PARAM.at(count).at(2) == 0) {
                g_currentLightLevel = count;
                updateLight(EXPOSE_AND_GAIN_PARAM.at(g_currentLightLevel).at(0),
                            EXPOSE_AND_GAIN_PARAM.at(g_currentLightLevel).at(1),
                            EXPOSE_AND_GAIN_PARAM.at(g_currentLightLevel).at(2));
                break;
            }
        }
    }
}


void updateExposeAndGain(const bool isUp) {
    if (!g_camAutoLight) {
        return;
    }

    logPrintln("Current level is " + to_string(g_currentLightLevel) +
               " want to up " + to_string(isUp), airstrip::INFO, __FUNCTION__);

    if (g_currentLightLevel <= 0 && !isUp) {
        logPrintln("Down fail", airstrip::INFO, __FUNCTION__);
    } else if (g_currentLightLevel >= EXPOSE_AND_GAIN_PARAM.size() - 1 && isUp) {
        logPrintln("Up fail", airstrip::INFO, __FUNCTION__);
    } else {
        if (isUp) {
            ++g_currentLightLevel;
        } else {
            --g_currentLightLevel;
        }
    }
    updateLight(EXPOSE_AND_GAIN_PARAM.at(g_currentLightLevel).at(0),
                EXPOSE_AND_GAIN_PARAM.at(g_currentLightLevel).at(1),
                EXPOSE_AND_GAIN_PARAM.at(g_currentLightLevel).at(2));
}


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

    const string modelPath = g_appWorkDir + "model/Pikachu";
    HResult ret = HFLaunchInspireFace(modelPath.c_str());
    if (ret != HSUCCEED) {
        logPrintln("Load resource error: " + ret, airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    constexpr HOption option = HF_ENABLE_FACE_RECOGNITION | HF_ENABLE_QUALITY;
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
    if (g_fullFaceCompare) {
        configuration.searchMode = HF_SEARCH_MODE_EXHAUSTIVE;
    } else {
        configuration.searchMode = HF_SEARCH_MODE_EAGER;
    }
    configuration.searchThreshold = static_cast<float>(std::min(g_faceThreshold, g_faceThresholdNight));
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

    imwrite(g_appWorkDir + "face/" + userInfo.userId + ".jpg", generalUtils::matCompress(pic));

    logPrintln("Insert finish userId = " + userInfo.userId, airstrip::INFO, __FUNCTION__);
    logPrintln("Insert finish faceId = " + to_string(faceId), airstrip::INFO, __FUNCTION__);

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

    airstrip::execCommand("rm " + g_appWorkDir + "face/" + userInfo.userId + ".jpg");

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

    // todo update photo

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

bool faceDetect(const cv::Mat &frame, const cv::Mat &rgaFrame, cv::Rect &rect, int orgCols, int orgRows) {
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
        const int confidence = p[0];
        const int x = p[1];
        const int y = p[2];
        const int w = p[3];
        const int h = p[4];
        char sScore[256];
        snprintf(sScore, 256, "%d", confidence);

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


        const auto minSide = min(rect.width, rect.height);
        logPrintln("Size min side =  " + to_string(minSide) + " confidence is " + to_string(confidence) +
                   " faceDistance = " + to_string(g_faceDistance), airstrip::DEBUG, __FUNCTION__);
        if ((1 == g_faceDistance && minSide < 320) || (2 == g_faceDistance && minSide < 180)) {
            ret = false;
        }
        if (confidence < 60) {
            ret = false;
        }

        // 计算明暗矫正摄像头
        const cv::Mat rgaFrameFace = rgaFrame(rect);
        cv::Mat grayFrameFace;
        cvtColor(rgaFrameFace, grayFrameFace, cv::COLOR_BGR2GRAY);


        //亮暗比例
        int darkPixels = 0;
        int brightPixels = 0;
        double brightnessSum = 0;
        const int totalPixels = grayFrameFace.rows * grayFrameFace.cols;

        for (int i = 0; i < grayFrameFace.rows; i++) {
            const uchar *row = grayFrameFace.ptr<uchar>(i);
            for (int j = 0; j < grayFrameFace.cols; j++) {
                const uchar pixel = row[j];
                brightnessSum += pixel;
                if (pixel < g_darkThreshold) {
                    darkPixels++;
                }
                if (pixel > g_lightThreshold) {
                    brightPixels++;
                }
            }
        }

        const double lightRatio = static_cast<double>(brightPixels) / totalPixels;
        const double darkRatio = static_cast<double>(darkPixels) / totalPixels;

        logPrintln("Face Detect light radio: " + to_string(lightRatio)
                   + " dark radio: " + to_string(darkRatio), airstrip::DEBUG, __FUNCTION__);

        if (lightRatio > g_lightRatio) {
            updateExposeAndGain(false);
        } else if (darkRatio > g_darkRatio) {
            updateExposeAndGain(true);
        }
    }

    free(pBuffer);
    pBuffer = nullptr;
    return ret;
}

void faceRecognition(const std::string &address, const cv::Rect &rect) {
    if (!initializedFaceRec) {
        return;
    }
    const auto image = cv::imread(address);
    if (image.empty()) {
        logPrintln("Read pic error " + address, airstrip::WARN, __FUNCTION__);
        return;
    }
    return faceRecognition(image, rect);
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

    HFloat quality;
    ret = HFFaceQualityDetect(faceRecognitionSession, multipleFaceData.tokens[0], &quality);
    logPrintln("Face quality is " + to_string(quality), airstrip::INFO, __FUNCTION__);
    // if (quality < 0.5 || ret != HSUCCEED) {
    //     logPrintln("Face quality not meet " + to_string(quality),
    //                airstrip::WARN, __FUNCTION__);
    //     HFReleaseImageStream(stream);
    //     return;
    // }

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
        ScheduledTask::sendFaceRegRes({}, frame, 0.0);
        HFReleaseImageStream(stream);
        return;
    }

    const auto userData = faceUserInfoMap[searchResult.id];
    logPrintln("Face recognition ret id = " + to_string(searchResult.id)
               + " userId = " + userData.userId + " " + to_string(confidence),
               airstrip::INFO, __FUNCTION__);
    if ((!currentIsNight() && confidence < g_faceThreshold) || (
            currentIsNight() && confidence < g_faceThresholdNight)) {
        ScheduledTask::sendFaceRegRes({}, frame, 0.0);
    } else {
        ScheduledTask::sendFaceRegRes(userData, frame, confidence);
    }

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
