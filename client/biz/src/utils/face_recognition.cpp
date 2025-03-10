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
