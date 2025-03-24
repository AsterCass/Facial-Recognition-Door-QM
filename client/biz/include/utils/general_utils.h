#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>


namespace generalUtils {
    std::vector<uchar> decodeBase64(const std::string &base64String);

    cv::Mat base64ToMat(const std::string &base64String);

    std::string matToBase64(const cv::Mat &img);
}


#endif //GENERAL_UTILS_H
