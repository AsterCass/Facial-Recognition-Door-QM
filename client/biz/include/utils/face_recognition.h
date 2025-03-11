#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <opencv2/core.hpp>

void initFaceRecognition();

void faceInsert(const std::string &address, const std::string& userId);

void faceInsert(const cv::Mat &pic, const std::string& userId);

bool faceDetect(const cv::Mat &frame, cv::Rect &rect, int orgCols, int orgRows);

void faceRecognition(const cv::Mat &frame, const cv::Rect &rect);

#endif //FACE_RECOGNITION_H
