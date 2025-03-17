#ifndef SCHEDULED_TASK_H
#define SCHEDULED_TASK_H
#include <future>
#include "face_recognition.h"


class ScheduledTask {
public:
    ScheduledTask(const ScheduledTask &) = delete;

    ScheduledTask &operator=(const ScheduledTask &) = delete;

    static ScheduledTask *getInstance() {
        static ScheduledTask instance;
        return &instance;
    }

    static void sendFaceRegRes(const FaceUserInfo &userInfo);

private:
    ScheduledTask();

    ~ScheduledTask();

    std::future<void> taskFuture;
};


#endif //SCHEDULED_TASK_H
