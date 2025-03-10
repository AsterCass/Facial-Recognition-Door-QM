#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <opencv2/core.hpp>

void initFaceRecognition();

bool faceDetect(const cv::Mat &frame, cv::Rect &rect);

void faceRecognition(const cv::Mat &frame);

#endif //FACE_RECOGNITION_H
