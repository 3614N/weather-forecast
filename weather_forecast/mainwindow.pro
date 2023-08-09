QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -L.\lib -llibxml2 -llibcurl -lsqlite3


SOURCES += \
    averagenum.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    averagenum.h \
    calculator.h \
    curl/curl.h \
    curl/curlver.h \
    curl/easy.h \
    curl/header.h \
    curl/mprintf.h \
    curl/multi.h \
    curl/options.h \
    curl/stdcheaders.h \
    curl/system.h \
    curl/typecheck-gcc.h \
    curl/urlapi.h \
    curl/websockets.h \
    detector.h \
    libs.h \
    libxml/HTMLparser.h \
    libxml/HTMLtree.h \
    libxml/SAX.h \
    libxml/SAX2.h \
    libxml/c14n.h \
    libxml/catalog.h \
    libxml/chvalid.h \
    libxml/debugXML.h \
    libxml/dict.h \
    libxml/encoding.h \
    libxml/entities.h \
    libxml/globals.h \
    libxml/hash.h \
    libxml/list.h \
    libxml/nanoftp.h \
    libxml/nanohttp.h \
    libxml/parser.h \
    libxml/parserInternals.h \
    libxml/pattern.h \
    libxml/relaxng.h \
    libxml/schemasInternals.h \
    libxml/schematron.h \
    libxml/threads.h \
    libxml/tree.h \
    libxml/uri.h \
    libxml/valid.h \
    libxml/xinclude.h \
    libxml/xlink.h \
    libxml/xmlIO.h \
    libxml/xmlautomata.h \
    libxml/xmlerror.h \
    libxml/xmlexports.h \
    libxml/xmlmemory.h \
    libxml/xmlmodule.h \
    libxml/xmlreader.h \
    libxml/xmlregexp.h \
    libxml/xmlsave.h \
    libxml/xmlschemas.h \
    libxml/xmlschemastypes.h \
    libxml/xmlstring.h \
    libxml/xmlunicode.h \
    libxml/xmlversion.h \
    libxml/xmlwriter.h \
    libxml/xpath.h \
    libxml/xpathInternals.h \
    libxml/xpointer.h \
    mainwindow.h \
    sqlite3/sqlite3-vcpkg-config.h \
    sqlite3/sqlite3.h

FORMS += \
    averagenum.ui \
    mainwindow.ui

RC_ICONS += mainwindow.ico

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
