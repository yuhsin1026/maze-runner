QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    enemy_graph.cpp \
    enemy_text.cpp \
    game.cpp \
    hp_graph.cpp \
    hp_model.cpp \
    hp_text.cpp \
    main.cpp \
    mainwindow.cpp \
    node.cpp \
    pathfinder.cpp \
    penemy_graph.cpp \
    penemy_text.cpp \
    startwindow.cpp \
    warrior_model.cpp \
    warrior_text.cpp \
    warrior_graph.cpp \
    view.cpp \
    xenemy.cpp \
    xenemy_graph.cpp \
    xenemy_text.cpp

HEADERS += \
    config.h \
    enemy_graph.h \
    enemy_text.h \
    game.h \
    hp_graph.h \
    hp_model.h \
    hp_text.h \
    mainwindow.h \
    node.h \
    pathfinder.h \
    penemy_graph.h \
    penemy_text.h \
    startwindow.h \
    view.h \
    warrior_model.h \
    warrior_text.h \
    warrior_graph.h \
    world.h \
    world_global.h \
    xenemy.h \
    xenemy_graph.h \
    xenemy_text.h

FORMS += \
    mainwindow.ui \
    startwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Documenten/Media_Processing/Final_project/world_v3/release/ -lworld
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Documenten/Media_Processing/Final_project/world_v3/debug/ -lworld
else:unix: LIBS += -L$$PWD/world_v3/ -lworld

INCLUDEPATH += $$PWD/world_v3
DEPENDPATH += $$PWD/world_v3

DISTFILES += \
    Images/maze2.png

RESOURCES += \
    images.qrc

