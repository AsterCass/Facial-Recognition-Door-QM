#ifndef GENERAL_ENUMS_H
#define GENERAL_ENUMS_H


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

enum RUNTIME_ENV {
    PROD = 0,
    DEV = 1,
};

enum MainPage {
    MAIN_PAGE_INIT,
    MAIN_PAGE_HOME,
};

enum ZFCardType {
    ICCard = 50,
    IdCard = 36,
};


#endif //GENERAL_ENUMS_H
