TEMPLATE = app

QT += qml quick network gui

include (curl/qtcurl.pri)

SOURCES += main.cpp \
    downloadprogress.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    downloadprogress.h

INCLUDEPATH += $$PWD/libcurl/include
LIBS += $$PWD/libcurl/lib/libcurldll.a
