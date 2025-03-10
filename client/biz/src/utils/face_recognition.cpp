#include "utils/face_recognition.h"

#include "inspireface.h"
#include "intypedef.h"
#include <string>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>

#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "facedetectcnn.h"
#include "config/config.h"

using namespace std;

HFSession faceRecognitionSession = {};

bool initialized = false;

void initFaceRecognition() {
    if (initialized) {
        return;
    }

    // string appWorkDir;
    // airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);
    //
    // const string modelPath = appWorkDir + "model/Pikachu";
    // HResult ret = HFLaunchInspireFace(modelPath.c_str());
    // if (ret != HSUCCEED) {
    //     logPrintln("Load Resource error: " + ret, airstrip::INFO, __FUNCTION__);
    //     exit(EXIT_FAILURE);
    // }
    //
    // HOption option = HF_ENABLE_FACE_RECOGNITION;
    // HFDetectMode detMode = HF_DETECT_MODE_ALWAYS_DETECT;
    // HInt32 maxDetectNum = 20;
    // HInt32 detectPixelLevel = 160;
    // ret = HFCreateInspireFaceSessionOptional(
    //     option, detMode, maxDetectNum, detectPixelLevel, -1, &faceRecognitionSession);
    // if (ret != HSUCCEED) {
    //     logPrintln("Create FaceContext error: " + ret, airstrip::INFO, __FUNCTION__);
    //     exit(EXIT_FAILURE);
    // }
    //
    // HFSessionSetTrackPreviewSize(faceRecognitionSession, detectPixelLevel);
    // HFSessionSetFilterMinimumFacePixelSize(faceRecognitionSession, 4);

    initialized = true;
}

void faceDetect(const cv::Mat &frame) {
    if (!initialized) {
        return;
    }

    cv::imwrite("/data/frd/test.jpg", frame);

    cv::Mat miniFrame;
    double scaleFactor = 0.2;
    resize(frame, miniFrame, cv::Size(), scaleFactor, scaleFactor, cv::INTER_LINEAR);

    int *pResults = NULL;
    auto *pBuffer = static_cast<unsigned char *>(malloc(0x9000));
    pResults = facedetect_cnn(pBuffer, miniFrame.data, miniFrame.cols, miniFrame.rows, miniFrame.step);
    int faceNum = pResults ? *pResults : 0;

    cout << faceNum << endl;

    free(pBuffer);
    pBuffer = nullptr;
}

void faceRecognition(const cv::Mat &frame) {
    cout << "======================= " << frame.cols << " " << frame.rows << endl;

    // if (!initialized) {
    //     return;
    // }

    // HFImageStream stream = nullptr;
    // HFImageData imageData = {};
    // imageData.data = frame.data;
    // imageData.format = HF_STREAM_RGB;
    // imageData.height = frame.rows;
    // imageData.width = frame.cols;
    // imageData.rotation = HF_CAMERA_ROTATION_0; // Image rotation
    // HResult ret = HFCreateImageStream(&imageData, &stream);
    //
    // HFMultipleFaceData multipleFaceData = {0};
    // ret = HFExecuteFaceTrack(faceRecognitionSession, stream, &multipleFaceData);
    //
    // auto faceNum = multipleFaceData.detectedNum;
    // airstrip::logPrintln("Num of face: " + faceNum);
    //
    // if (multipleFaceData.detectedNum > 0) {
    //     auto rect = multipleFaceData.rects;
    //     airstrip::logPrintln(to_string(rect->x));
    //     airstrip::logPrintln(to_string(rect->y));
    //     airstrip::logPrintln(to_string(rect->height));
    //     airstrip::logPrintln(to_string(rect->width));
    // }
}
