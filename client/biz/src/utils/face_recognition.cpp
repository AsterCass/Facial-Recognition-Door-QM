#include "utils/face_recognition.h"

#include "inspireface.h"
#include "intypedef.h"
#include <string>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>

#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"

using namespace std;

HFSession faceRecognitionSession = {};

bool initialized = false;

void initFaceRecognition() {
    if (initialized) {
        return;
    }

    string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    const string modelPath = appWorkDir + "model/Pikachu";
    HResult ret = HFLaunchInspireFace(modelPath.c_str());
    if (ret != HSUCCEED) {
        logPrintln("Load Resource error: " + ret, airstrip::INFO, __FUNCTION__);
        exit(EXIT_FAILURE);
    }

    HOption option = HF_ENABLE_FACE_RECOGNITION;
    HFDetectMode detMode = HF_DETECT_MODE_ALWAYS_DETECT;
    HInt32 maxDetectNum = 20;
    HInt32 detectPixelLevel = 160;
    ret = HFCreateInspireFaceSessionOptional(
        option, detMode, maxDetectNum, detectPixelLevel, -1, &faceRecognitionSession);
    if (ret != HSUCCEED) {
        logPrintln("Create FaceContext error: " + ret, airstrip::INFO, __FUNCTION__);
        exit(EXIT_FAILURE);
    }

    HFSessionSetTrackPreviewSize(faceRecognitionSession, detectPixelLevel);
    HFSessionSetFilterMinimumFacePixelSize(faceRecognitionSession, 4);

    initialized = true;
}

void faceRecognition(const cv::Mat &frame) {
    if (!initialized) {
        return;
    }

    cv::Mat miniFrame;
    cv::resize(frame, miniFrame, cv::Size(), 0.3, 0.3, cv::INTER_LINEAR);

    HFImageStream stream = nullptr;
    HFImageData imageData = {};
    imageData.data = miniFrame.data;
    imageData.format = HF_STREAM_RGB;
    imageData.height = miniFrame.rows;
    imageData.width = miniFrame.cols;
    imageData.rotation = HF_CAMERA_ROTATION_0; // Image rotation
    HResult ret = HFCreateImageStream(&imageData, &stream);


    HFMultipleFaceData multipleFaceData = {0};
    ret = HFExecuteFaceTrack(faceRecognitionSession, stream, &multipleFaceData);


    auto faceNum = multipleFaceData.detectedNum;
    airstrip::logPrintln("Num of face: " + faceNum);

    if (multipleFaceData.detectedNum > 0) {
        auto rect = multipleFaceData.rects;
        airstrip::logPrintln(to_string(rect->x));
        airstrip::logPrintln(to_string(rect->y));
        airstrip::logPrintln(to_string(rect->height));
        airstrip::logPrintln(to_string(rect->width));
    }
}
