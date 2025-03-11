#include "utils/face_recognition.h"

#include "inspireface.h"
#include "intypedef.h"
#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types.hpp>

#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "facedetectcnn.h"
#include "config/config.h"

using namespace std;

HFSession faceRecognitionSession = nullptr;

bool initialized = false;

void initFaceRecognition() {
    if (initialized) {
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

    string featureDb = appWorkDir + "face-feature/feature.db";

    HFFeatureHubConfiguration configuration;
    configuration.primaryKeyMode = HF_PK_AUTO_INCREMENT;
    configuration.enablePersistence = 1;
    configuration.persistenceDbPath = &featureDb[0];
    configuration.searchMode = HF_SEARCH_MODE_EAGER;
    configuration.searchThreshold = 0.48f;
    ret = HFFeatureHubDataEnable(configuration);
    if (ret != HSUCCEED) {
        logPrintln("Create face db error: " + ret, airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    logPrintln("Face model init finish", airstrip::INFO, __FUNCTION__);
    initialized = true;
}


void faceInsert(const std::string &address, const std::string &userId) {
    if (!initialized) {
        return;
    }
    const auto image = cv::imread(address);
    if (image.empty()) {
        logPrintln("Read pic error " + address, airstrip::WARN, __FUNCTION__);
        return;
    }
    faceInsert(image, userId);
}

void faceInsert(const cv::Mat &pic, const std::string &userId) {
    if (!initialized) {
        return;
    }

    logPrintln("Face insert userId = " + userId, airstrip::INFO, __FUNCTION__);

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
        return;
    }

    const auto faceNum = multipleFaceData.detectedNum;
    if (faceNum <= 0) {
        // todo error throw
        logPrintln("Face insert face not found ", airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return;
    }

    HFFaceFeature feature = {};
    ret = HFFaceFeatureExtract(faceRecognitionSession, stream,
                               multipleFaceData.tokens[0], &feature);
    if (ret != HSUCCEED) {
        logPrintln("Face insert feature extract fail " + ret,
                   airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return;
    }

    int64_t resultId = 0;
    HFFaceFeatureIdentity identity = {};
    identity.feature = &feature;
    ret = HFFeatureHubInsertFeature(identity, &resultId);
    if (ret != HSUCCEED) {
        logPrintln("Face insert face error " + ret, airstrip::WARN, __FUNCTION__);
        HFReleaseImageStream(stream);
        return;
    }

    logPrintln("Face insert finish for userId = " + userId, airstrip::INFO, __FUNCTION__);

    // todo callback

    HFReleaseImageStream(stream);
}


bool faceDetect(const cv::Mat &frame, cv::Rect &rect, int orgCols, int orgRows) {
    bool ret = false;
    if (!initialized) {
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
    if (!initialized) {
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
