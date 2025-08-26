#include "utils/face_recognition.h"

#include <airstrip_thread_pool.h>
#include <camera/camera_frame.h>
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

    const string modelPath = g_appWorkDir + "model/Gundam_RV1109";
    HResult ret = HFLaunchInspireFace(modelPath.c_str());
    if (ret != HSUCCEED) {
        logPrintln("Load resource error: " + ret, airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    constexpr HOption option = HF_ENABLE_FACE_RECOGNITION;
    constexpr HFDetectMode detMode = HF_DETECT_MODE_LIGHT_TRACK;
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

bool faceDetect(const cv::Mat &frameFull, const cv::Rect &rect) {
    bool ret = false;
    if (!initializedFaceRec || frameFull.empty()) {
        return ret;
    }

    const int maxWidth = frameFull.cols;
    const int maxHeight = frameFull.rows;
    const int faceX = std::max(0, rect.x - rect.width / 2);
    const int faceY = std::max(0, rect.y - rect.height / 2);
    int faceW = std::max(0, rect.width + rect.width / 2);
    int faceH = std::max(0, rect.height + rect.height / 2);
    faceW = faceX + faceW > maxWidth ? maxWidth - faceX : faceW;
    faceH = faceY + faceH > maxHeight ? maxHeight - faceY : faceH;

    const auto frameFace = frameFull(cv::Rect(faceX, faceY, faceW, faceH));
    cv::Mat frame;
    resize(frameFace, frame, cv::Size(
               frameFace.cols / 2, frameFace.rows / 2), 0, 0, cv::INTER_AREA);


    const int *pResults = nullptr;
    auto *pBuffer = static_cast<unsigned char *>(malloc(0x9000));
    pResults = facedetect_cnn(pBuffer, frame.data, frame.cols, frame.rows, frame.step);
    const int faceNum = pResults ? *pResults : 0;

    if (faceNum > 0) {
        ret = true;

        // 最大人脸
        const auto *p = (short *) (pResults + 1);
        const int confidence = p[0];
        char sScore[256];
        snprintf(sScore, 256, "%d", confidence);

        const auto minSide = min(frame.cols, frame.rows);
        logPrintln("Size min side =  " + to_string(minSide) + " confidence is " + to_string(confidence) +
                   " faceDistance = " + to_string(g_faceDistance), airstrip::DEBUG, __FUNCTION__);
        if ((1 == g_faceDistance && minSide < 320) || (2 == g_faceDistance && minSide < 180)) {
            ret = false;
        }
        // if (confidence < 60) {
        //     ret = false;
        // }

        // 计算明暗矫正摄像头
        cv::Mat grayFrameFace;
        cvtColor(frame, grayFrameFace, cv::COLOR_BGR2GRAY);


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

bool faceDetectInspire(const cv::Mat &frame, const cv::Mat &frameIr, cv::Rect &rectOutput, bool moreAction) {
    bool ret = false;
    if (!initializedFaceRec || frame.empty() || frameIr.empty()) {
        return ret;
    }

    HFImageStream stream = nullptr;
    HFImageData imageData = {};
    imageData.data = frameIr.data;
    imageData.format = HF_STREAM_BGR;
    imageData.height = frameIr.rows;
    imageData.width = frameIr.cols;
    imageData.rotation = HF_CAMERA_ROTATION_0;
    HResult retI = HFCreateImageStream(&imageData, &stream);
    if (retI != HSUCCEED) {
        logPrintln("Face recognition build image fail " + retI,
                   airstrip::WARN, __FUNCTION__);
        return ret;
    }

    HFMultipleFaceData multipleFaceData = {};
    retI = HFExecuteFaceTrack(faceRecognitionSession, stream, &multipleFaceData);
    if (retI != HSUCCEED) {
        logPrintln("Face recognition track image fail " + retI,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return ret;
    }

    if (multipleFaceData.detectedNum <= 0) {
        HFReleaseImageStream(stream);
        return ret;
    }
    ret = true;

    logPrintln("Track id: " + to_string(multipleFaceData.trackIds[0]),
               airstrip::DEBUG, __FUNCTION__);

    const cv::Rect rectIr(multipleFaceData.rects->x, multipleFaceData.rects->y,
                          multipleFaceData.rects->width, multipleFaceData.rects->height);

    // 校正
    const int maxWidth = frameIr.cols;
    const int maxHeight = frameIr.rows;
    const int faceX = std::max(0, rectIr.x);
    const int faceY = std::max(0, rectIr.y);
    int faceW = std::max(0, rectIr.width);
    int faceH = std::max(0, rectIr.height);
    faceW = faceX + faceW > maxWidth ? maxWidth - faceX : faceW;
    faceH = faceY + faceH > maxHeight ? maxHeight - faceY : faceH;
    rectOutput = cv::Rect(faceX, faceY, faceW, faceH);

    // 获取rgb图像对应区域
    const auto frameRgbFace = frame(cv::Rect(faceX, faceY, faceW, faceH));

    // 检查最小范围
    const auto minSide = min(frameRgbFace.cols, frameRgbFace.rows);
    logPrintln("Size min side =  " + to_string(minSide) +
               " faceDistance = " + to_string(g_faceDistance), airstrip::DEBUG, __FUNCTION__);
    if (!g_longDistanceDetect && minSide < 120) {
        ret = false;
    }

    // 计算明暗矫正摄像头
    if (moreAction) {
        cv::Mat grayFrameFace;
        cvtColor(frameRgbFace, grayFrameFace, cv::COLOR_BGR2GRAY);


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

    logPrintln("Ir face release ... ", airstrip::DEBUG, __FUNCTION__);

    HFReleaseImageStream(stream);
    return ret;
}

void faceRecognition(const std::string &address, const std::string &addressIr) {
    if (!initializedFaceRec) {
        return;
    }
    const auto image = cv::imread(address);
    if (image.empty()) {
        logPrintln("Read pic error " + address, airstrip::WARN, __FUNCTION__);
        return;
    }
    const auto imageIr = cv::imread(addressIr);
    if (imageIr.empty()) {
        logPrintln("Read pic ir error " + address, airstrip::WARN, __FUNCTION__);
        return;
    }
    return faceRecognition(image, imageIr);
}

void faceRecognition(const cv::Mat &frame, const cv::Mat &frameIr) {
    if (!initializedFaceRec || frame.empty() || frameIr.empty()) {
        return;
    }

    static int isCheckFaceReco = 0;
    if (isCheckFaceReco > 1) {
        logPrintln("In Recognition ... ", airstrip::DEBUG, __FUNCTION__);
        return;
    }
    isCheckFaceReco++;
    cv::Mat frameCopy = frame.clone();
    cv::Mat frameIrCopy = frameIr.clone();

    static_cast<airstrip::ThreadPool *>(g_mainThreadPool)->enqueue([frameCopy, frameIrCopy] {
            // 确定是否执行人脸识别，还是只是检测
            bool onlyDetect = true;
            static int64_t lastMillisecondCount = 0L;
            const int64_t currentMillisecondCount =
                    std::chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).
                    count();
            // 超过间隔时间且没有在进行人脸检测则发起新的人脸检测
            if (currentMillisecondCount - lastMillisecondCount > g_faceRegCoreIvMillSec && !g_isCheckFace) {
                onlyDetect = false;
                g_isCheckFace = true;
                lastMillisecondCount = currentMillisecondCount;
            }

            logPrintln("For check face in ... ", airstrip::DEBUG, __FUNCTION__);

            // 红外人脸检测
            cv::Rect rectIr;
            const bool retDetect = faceDetectInspire(frameCopy, frameIrCopy, rectIr, !onlyDetect);

            logPrintln("For check face ret = " + to_string(retDetect) +
                       " " + to_string(onlyDetect) + " " + to_string(g_isCheckFace),
                       airstrip::DEBUG, __FUNCTION__);

            if (retDetect) {
                CameraFrame::getInstance()->setFaceRects(rectIr.x, rectIr.y, rectIr.width, rectIr.height);
            } else {
                CameraFrame::getInstance()->setFaceRects(0, 0, 0, 0);
            }

            if (onlyDetect) {
                --isCheckFaceReco;
                return;
            }

            if (!retDetect && g_enableFaceSpoof) {
                g_isCheckFace = false;
                --isCheckFaceReco;
                return;
            }

            logPrintln("Start RGA face detect ...", airstrip::DEBUG, __FUNCTION__);

            // 执行人脸检测
            HFImageStream stream = nullptr;
            HFImageData imageData = {};
            imageData.data = frameCopy.data;
            imageData.format = HF_STREAM_BGR;
            imageData.height = frameCopy.rows;
            imageData.width = frameCopy.cols;
            imageData.rotation = HF_CAMERA_ROTATION_0;
            HResult ret = HFCreateImageStream(&imageData, &stream);
            if (ret != HSUCCEED) {
                logPrintln("Face recognition build image fail " + ret,
                           airstrip::WARN, __FUNCTION__);
                g_isCheckFace = false;
                --isCheckFaceReco;
                return;
            }

            HFMultipleFaceData multipleFaceData = {};
            ret = HFExecuteFaceTrack(faceRecognitionSession, stream, &multipleFaceData);
            if (ret != HSUCCEED) {
                logPrintln("Face recognition track image fail " + ret,
                           airstrip::WARN, __FUNCTION__);
                HFReleaseImageStream(stream);
                g_isCheckFace = false;
                --isCheckFaceReco;
                return;
            }

            if (multipleFaceData.detectedNum <= 0) {
                HFReleaseImageStream(stream);
                g_isCheckFace = false;
                --isCheckFaceReco;
                return;
            }


            // 确认当前IR和RGB图片基本重合
            if (g_enableFaceSpoof) {
                const cv::Rect rectRgb(multipleFaceData.rects->x, multipleFaceData.rects->y,
                                       multipleFaceData.rects->width, multipleFaceData.rects->height);
                const cv::Point rectRgbCenter(rectRgb.x + rectRgb.width / 2, rectRgb.y + rectRgb.height / 2);
                const cv::Point rectIrCenter(rectIr.x + rectIr.width / 2, rectIr.y + rectIr.height / 2);
                if (!rectRgb.contains(rectIrCenter) || !rectIr.contains(rectRgbCenter)) {
                    logPrintln("Face fake face !!!!!", airstrip::WARN, __FUNCTION__);
                    HFReleaseImageStream(stream);
                    g_isCheckFace = false;
                    --isCheckFaceReco;
                    return;
                }
            }

            HFFaceFeature feature = {};
            ret = HFFaceFeatureExtract(faceRecognitionSession, stream, multipleFaceData.tokens[0], &feature);
            if (ret != HSUCCEED) {
                logPrintln("Face recognition feature extract fail " + ret,
                           airstrip::WARN, __FUNCTION__);
                HFReleaseImageStream(stream);
                g_isCheckFace = false;
                --isCheckFaceReco;
                return;
            }

            HFloat confidence;
            HFFaceFeatureIdentity searchResult = {};
            ret = HFFeatureHubFaceSearch(feature, &confidence, &searchResult);
            if (ret != HSUCCEED) {
                logPrintln("Face recognition feature search fail " + ret,
                           airstrip::WARN, __FUNCTION__);
                HFReleaseImageStream(stream);
                g_isCheckFace = false;
                --isCheckFaceReco;
                return;
            }

            if (searchResult.id <= 0 || faceUserInfoMap.find(searchResult.id) == faceUserInfoMap.end()) {
                ScheduledTask::sendFaceRegRes({}, frameCopy, 0.0);
                HFReleaseImageStream(stream);
                g_isCheckFace = false;
                --isCheckFaceReco;
                return;
            }

            const auto userData = faceUserInfoMap[searchResult.id];
            logPrintln("Face recognition ret id = " + to_string(searchResult.id)
                       + " userId = " + userData.userId + " " + to_string(confidence),
                       airstrip::INFO, __FUNCTION__);
            if ((!currentIsNight() && confidence < g_faceThreshold) || (
                    currentIsNight() && confidence < g_faceThresholdNight)) {
                ScheduledTask::sendFaceRegRes({}, frameCopy, 0.0);
            } else {
                ScheduledTask::sendFaceRegRes(userData, frameCopy, confidence);
            }

            HFReleaseImageStream(stream);
            g_isCheckFace = false;
            --isCheckFaceReco;
        }

    );
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
