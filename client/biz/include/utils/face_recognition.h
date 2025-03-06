#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <opencv2/core.hpp>

void initFaceRecognition();

void faceRecognition(const cv::Mat &frame);

#endif //FACE_RECOGNITION_H
