#include "utils/global_data_manager.h"

using namespace std;

GlobalDataManager::GlobalDataManager(QObject *parent) : QObject(parent) {
    qRegisterMetaType<std::string>("std::string");
}

GlobalDataManager::~GlobalDataManager() = default;
