#ifndef AIRSTRIP_ENUMS_H
#define AIRSTRIP_ENUMS_H


enum LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

enum ProgramOptionInputValueType {
    STRING,
    STRING_VECTOR,
    INTEGER,
    INTEGER_VECTOR,
    DOUBLE,
    DOUBLE_VECTOR,
};

enum RequestMethod {
    GET,
    POST,
    PUT,
    DELETE,
};


#endif //AIRSTRIP_ENUMS_H
