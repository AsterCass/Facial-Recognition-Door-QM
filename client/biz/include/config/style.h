#ifndef CONFIG_STYLE_H
#define CONFIG_STYLE_H

// const
#ifdef WIN32
constexpr auto SWITCH_BUTTON_ENABLE_STYLE = R"(
    QPushButton {
        background-color: rgb(50, 215, 75);
        color: white;
        margin-right: 20px;
        padding: 0px;
        font-size: 24px;
        width: 40px;
        height: 22px;
        text-align: right;
        border-radius: 10px;
    }
)";
constexpr auto SWITCH_BUTTON_DISABLE_STYLE = R"(
    QPushButton {
        background-color: rgb(61, 61, 61);
        color: white;
        margin-right: 20px;
        padding: 0px;
        font-size: 24px;
        width: 40px;
        height: 22px;
        text-align: left;
        border-radius: 10px;
    }
)";
#else
constexpr auto SWITCH_BUTTON_ENABLE_STYLE = R"(
    QPushButton {
        background-color: rgb(50, 215, 75);
        color: white;
        margin-right: 20px;
        padding: 0px;
        font-size: 48px;
        width: 80px;
        height: 44px;
        text-align: right;
        border-radius: 20px;
    }
)";
constexpr auto SWITCH_BUTTON_DISABLE_STYLE = R"(
    QPushButton {
        background-color: rgb(61, 61, 61);
        color: white;
        margin-right: 20px;
        padding: 0px;
        font-size: 48px;
        width: 80px;
        height: 44px;
        text-align: left;
        border-radius: 20px;
    }
)";
#endif


#endif //CONFIG_STYLE_H
