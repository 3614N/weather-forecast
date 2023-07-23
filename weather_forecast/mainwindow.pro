QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += C:/vcpkg/installed/x64-windows/include
LIBS += -LC:/vcpkg/installed/x64-windows/lib -lsqlite3

# Определение пути к директории vcpkg
VCPKG_DIR = C:/vcpkg

# Путь к заголовочным файлам библиотек
INCLUDEPATH += $$VCPKG_DIR/installed/x64-windows/include

# Путь к файлам .lib библиотек
LIBS += -L$$VCPKG_DIR/installed/x64-windows/lib

# Подключение библиотеки libxml2
LIBS += -llibxml2

# Подключение библиотеки libcurl
LIBS += -llibcurl


SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

RC_ICONS += mainwindow.ico

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
