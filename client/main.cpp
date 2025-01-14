#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::cout << "Hello World!" << std::endl;

    return QApplication::exec();
}
