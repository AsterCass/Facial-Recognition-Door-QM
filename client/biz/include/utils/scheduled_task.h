#ifndef SCHEDULED_TASK_H
#define SCHEDULED_TASK_H
#include <future>
#include "face_recognition.h"
#include "db/open_record_db.h"


class ScheduledTask {
public:
    ScheduledTask(const ScheduledTask &) = delete;

    ScheduledTask &operator=(const ScheduledTask &) = delete;

    static ScheduledTask *getInstance() {
        static ScheduledTask instance;
        return &instance;
    }

    static void sendFaceRegRes(const FaceUserInfo &userInfo, const cv::Mat &frame, float confidence);

    static bool commonOpenDoor(const OpenRecordInfo &openRecordInfo, const std::string &userId = "",
                               float confidence = 0.0);

private:
    ScheduledTask();

    ~ScheduledTask();

    std::future<void> taskFuture;
};


#endif //SCHEDULED_TASK_H
