#include "utils/general_utils.h"

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

        // 从二进制数据创建Mat对象
        cv::Mat img = imdecode(decodedData, cv::IMREAD_COLOR);

        return img;
    }
}
