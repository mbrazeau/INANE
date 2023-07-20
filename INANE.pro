TEMPLATE = app
TARGET = INANE
QT = core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += sql widgets

SOURCES += \
    charactereditorwindow.cpp \
    compdatabase.cpp \
    main.cpp \
    mainmenu.cpp \
    mainwindow.cpp \
    nexusreader.cpp \
    noteditabledelegate.cpp \
    stateselectordelegate.cpp \
    taxonmanager.cpp

HEADERS += \
    charactereditorwindow.h \
    compdatabase.h \
    mainmenu.h \
    mainwindow.h \
    nexusreader.h \
    noteditabledelegate.h \
    stateselectordelegate.h \
    taxonmanager.h

QMAKE_MACOSX_DEPLOYMENT_TARGET = 13.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/external/ncl/build/ncl/release/ -lncl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/external/ncl/build/ncl/debug/ -lncl
else:unix: LIBS += -L$$PWD/external/ncl/build/ncl/ -lncl

INCLUDEPATH += $$PWD/external
INCLUDEPATH += $$PWD/external/ncl/ncl
INCLUDEPATH += $$PWD/external/ncl
DEPENDPATH += $$PWD/external/build/ncl

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/external/ncl/build/ncl/release/libncl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/external/ncl/build/ncl/debug/libncl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/external/ncl/build/ncl/release/ncl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/external/ncl/build/ncl/debug/ncl.lib
else:unix: PRE_TARGETDEPS += $$PWD/external/ncl/build/ncl/libncl.a

RESOURCES += \
    inane.qrc
