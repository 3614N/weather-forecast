QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Путь к директории с Vcpkg
VCPKG_DIR = C:\vcpkg

# Пути к заголовочным файлам
INCLUDEPATH += $$VCPKG_DIR/installed/x64-windows/include
# Добавьте другие пути к заголовочным файлам для нескольких библиотек, если это необходимо.

# Пути к библиотекам
LIBS += -L$$VCPKG_DIR/installed/x64-windows/lib
LIBS += -llibxml -lcurl -lsqlite3
# Замените "библиотека1", "библиотека2", "библиотека3" на названия библиотек, которые вы хотите использовать.

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
