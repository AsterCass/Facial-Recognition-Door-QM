#ifndef GLOBAL_DATA_MANAGER
#define GLOBAL_DATA_MANAGER

#include <QObject>

class GlobalDataManager final : public QObject {
    Q_OBJECT

public:
    static GlobalDataManager *getInstance() {
        static GlobalDataManager instance;
        return &instance;
    }

    GlobalDataManager(const GlobalDataManager &) = delete;

    GlobalDataManager &operator=(const GlobalDataManager &) = delete;

    void updateHeaderTime(const std::string &data) {
        if (headerTime != data) {
            headerTime = data;
            emit headerTimeChange(data);
        }
    }

    void updateHeaderWired(const bool data) {
        if (headerWired != data) {
            headerWired = data;
            emit headerWiredChange(data);
        }
    }

    void updateHeaderWireless(const bool data) {
        if (headerWireless != data) {
            headerWireless = data;
            emit headerWirelessChange(data);
        }
    }

    void updateHeaderFourG(const bool data) {
        if (headerFourG != data) {
            headerFourG = data;
            emit headerFourGChange(data);
        }
    }

    void updateHeaderServer(const bool data) {
        if (headerServer != data) {
            headerServer = data;
            emit headerServerChange(data);
        }
    }

signals:
    void headerTimeChange(const std::string &data);

    void headerWiredChange(bool data);

    void headerWirelessChange(bool data);

    void headerFourGChange(bool data);

    void headerServerChange(bool data);

private:
    explicit GlobalDataManager(QObject *parent = nullptr);

    ~GlobalDataManager() override;


    std::string headerTime;
    bool headerWired = false;
    bool headerWireless = false;
    bool headerFourG = false;
    bool headerServer = false;
};

#endif //GLOBAL_DATA_MANAGER
