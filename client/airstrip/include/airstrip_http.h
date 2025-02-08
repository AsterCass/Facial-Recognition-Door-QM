#ifndef AIRSTRIP_HTTP_H
#define AIRSTRIP_HTTP_H

#include <airstrip_enums.h>
#include <string>
#include <sstream>
#include <functional>
#include <map>

namespace airstrip {
    struct Response {
        bool success;
        int statusCode;
        std::string body;
        std::string error;
    };

    inline std::string methodToString(const RequestMethod method) {
        switch (method) {
            case GET: return "GET";
            case POST: return "POST";
            case PUT: return "PUT";
            case DELETE: return "DELETE";
            default: return "GET";
        }
    }


    class AirstripHttp {
    public:
        using RequestCallback = std::function<void(Response)>;

        static Response sendRequest(
            const std::string &url,
            RequestMethod method,
            const std::map<std::string, std::string> &params = {},
            const std::string &bodyJson = "",
            int timeoutSec = 30,
            const std::string &certPath = ""
        );

    private:
        static std::string buildCurlCommand(
            const std::string &url,
            RequestMethod method,
            const std::map<std::string, std::string> &params,
            const std::string &bodyJson,
            int timeoutSec,
            const std::string &certPath
        );

        static Response executeCurlCommand(const std::string &cmd);
    };
}


#endif // AIRSTRIP_HTTP_H
