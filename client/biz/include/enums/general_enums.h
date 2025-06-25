#ifndef GENERAL_ENUMS_H
#define GENERAL_ENUMS_H

enum ZFTaskOperationType {
    Card = 0x1001,
    Face = 0x1002,
    Disable = 0x1006,
    Voice = 0x1014,
    Open = 0x3005,
};

enum ZFTaskOperationAction {
    Add = 1,
    Remove = 2,
    Modify = 3,
};

enum PlayWavType {
    Di = 0,
    AuthSuccess = 1,
    AuthFail = 2,
    Disabled = 3,
    Expired = 4,
    Charge_1 = 21,
    Charge_2 = 22,
    Charge_3 = 23,
    Charge_4 = 24,
    Charge_5 = 25,
    Rental_1 = 31,
    Rental_2 = 32,
    AuthFailFirst = 401,
};

enum ZFOpenDoorType {
    FaceOpen = 7,
    IcCardOpen = 15,
    IdCardOpen = 16,
    RemoteOpen = 5,
};

enum RUNTIME_ENV {
    PROD = 0,
    DEV = 1,
};

enum MainPage {
    MAIN_PAGE_INIT,
    MAIN_PAGE_HOME,
    MAIN_PAGE_SETTING_LOGIN,
    MAIN_PAGE_SETTING_TMP,
    MAIN_PAGE_SETTING_MAIN,
    MAIN_PAGE_SETTING_SERVER,
    MAIN_PAGE_SETTING_FACE,
    MAIN_PAGE_SETTING_DOOR,
};

enum ZFCardType {
    ICCard = 50,
    IdCard = 36,
};


#endif //GENERAL_ENUMS_H
