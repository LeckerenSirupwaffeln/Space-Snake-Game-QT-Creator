QT       += core gui
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    music/music0.mp3 \
    music/music1.mp3 \
    music/music10.mp3 \
    music/music11.mp3 \
    music/music12.mp3 \
    music/music13.mp3 \
    music/music14.mp3 \
    music/music15.mp3 \
    music/music16.mp3 \
    music/music17.mp3 \
    music/music18.mp3 \
    music/music19.mp3 \
    music/music2.mp3 \
    music/music3.mp3 \
    music/music4.mp3 \
    music/music5.mp3 \
    music/music6.mp3 \
    music/music7.mp3 \
    music/music8.mp3 \
    music/music9.mp3 \
    sound_effects/effect_eat.wav \
    sound_effects/effect_gameover.wav

RESOURCES += \
    snakeresource1.qrc
