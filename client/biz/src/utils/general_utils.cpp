#include "utils/general_utils.h"

#include <airstrip_log.h>
#include <boost/beast/core/detail/base64.hpp>


using namespace std;

namespace generalUtils {
    std::vector<uchar> decodeBase64(const std::string &base64String) {
        std::vector<uchar> decoded;
        // 计算解码后的大小
        const size_t decodedSize = boost::beast::detail::base64::decoded_size(base64String.size());
        decoded.resize(decodedSize);

        // 执行解码
        const auto result = boost::beast::detail::base64::decode(
            decoded.data(),
            base64String.c_str(),
            base64String.length()
        );

        // 调整为实际解码大小
        decoded.resize(result.first);

        return decoded;
    }

    cv::Mat base64ToMat(const std::string &base64String) {
        // 解码Base64字符串
        const std::vector<uchar> decodedData = decodeBase64(base64String);

        // 检查解码后的数据是否为空
        if (decodedData.empty()) {
            logPrintln("Base64 decoding failed or resulted in empty data",
                       airstrip::ERROR, __FUNCTION__);
            return cv::Mat();
        }

        // 从二进制数据创建Mat对象
        cv::Mat img = imdecode(decodedData, cv::IMREAD_COLOR);

        // 检查Mat是否为空或者无效
        if (img.empty()) {
            logPrintln("Failed to decode image data", airstrip::ERROR, __FUNCTION__);
            return cv::Mat();
        }

        // 检查图像的基本属性
        if (img.rows <= 0 || img.cols <= 0 || img.channels() != 3) {
            logPrintln("Error: Invalid image dimensions or channels",
                       airstrip::ERROR, __FUNCTION__);
            return cv::Mat();
        }

        return img;
    }

    std::string matToBase64(const cv::Mat &img) {
        std::vector<uchar> buffer;
        // 编码图像到内存缓冲区
        imencode(".jpg", img, buffer);

        // 计算 base64 编码后的大小
        const size_t encodedSize = boost::beast::detail::base64::encoded_size(buffer.size());
        std::string base64String(encodedSize, 0);

        // 执行编码
        const auto result = boost::beast::detail::base64::encode(
            &base64String[0],
            buffer.data(),
            buffer.size()
        );

        // 调整为实际编码大小
        base64String.resize(result);
        return base64String;
    }

    cv::Mat matCompress(const cv::Mat &img) {
        const cv::Size newSize(img.cols / 2, img.rows / 2);
        cv::Mat resizedImage;
        resize(img, resizedImage, newSize, 0, 0, cv::INTER_AREA);
        std::vector<uchar> buffer;
        std::vector<int> compressionParams;
        compressionParams.push_back(cv::IMWRITE_JPEG_QUALITY);
        compressionParams.push_back(50);
        imencode(".jpg", resizedImage, buffer, compressionParams);
        return imdecode(buffer, cv::IMREAD_COLOR);
    }
}
