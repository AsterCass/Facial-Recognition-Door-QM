#ifndef CONFIG_STYLE_H
#define CONFIG_STYLE_H

// const
#ifdef WIN32
constexpr auto SWITCH_BUTTON_ENABLE_STYLE = R"(
    QPushButton {
        background-color: rgb(50, 215, 75);
        color: white;
        margin-right: 20px;
        padding-left: 2px;
        padding-right: 2px;
        padding-top: 0px;
        padding-bottom: 2px;
        font-size: 22px;
        width: 40px;
        height: 22px;
        text-align: right;
        border-radius: 10px;
    }
    QPushButton:focus {
        outline: none;
    }
)";
constexpr auto SWITCH_BUTTON_DISABLE_STYLE = R"(
    QPushButton {
        background-color: rgb(61, 61, 61);
        color: white;
        margin-right: 20px;
        padding-left: 2px;
        padding-right: 2px;
        padding-top: 0px;
        padding-bottom: 2px;
        font-size: 22px;
        width: 40px;
        height: 22px;
        text-align: left;
        border-radius: 10px;
    }
    QPushButton:focus {
        outline: none;
    }
)";
#else
constexpr auto SWITCH_BUTTON_ENABLE_STYLE = R"(
    QPushButton {
        background-color: rgb(50, 215, 75);
        color: white;
        margin-right: 20px;
        padding-left: 4px;
        padding-right: 4px;
        padding-top: 0px;
        padding-bottom: 4px;
        font-size: 36px;
        width: 80px;
        height: 44px;
        text-align: right;
        border-radius: 20px;
    }
    QPushButton:focus {
        outline: none;
    }
)";
constexpr auto SWITCH_BUTTON_DISABLE_STYLE = R"(
    QPushButton {
        background-color: rgb(61, 61, 61);
        color: white;
        margin-right: 20px;
        padding-left: 4px;
        padding-right: 4px;
        padding-top: 0px;
        padding-bottom: 4px;
        font-size: 36px;
        width: 80px;
        height: 44px;
        text-align: left;
        border-radius: 20px;
    }
    QPushButton:focus {
        outline: none;
    }
)";
#endif


#endif //CONFIG_STYLE_H
