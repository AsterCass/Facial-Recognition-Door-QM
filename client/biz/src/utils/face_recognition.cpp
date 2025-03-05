#include "utils/face_recognition.h"

#include "inspireface.h"
#include "intypedef.h"
#include <string>

#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "config/config.h"

using namespace std;

HFSession faceRecognitionSession = {};

void initFaceRecognition() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    const string modelPath = appWorkDir + "model/Gundam-RV1109";
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
}

void faceRecognition(void *data, int height, int width) {
    initFaceRecognition();

    HFImageStream stream = nullptr;
    HFImageData imageData = {};
    imageData.data = static_cast<uchar *>(data);
    imageData.format = HF_STREAM_RGB;
    imageData.height = height;
    imageData.width = width;
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
