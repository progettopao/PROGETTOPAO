QT       += core gui widgets

CONFIG += c++17

# Elimina il flag ereditato AGL senza rompere i componenti grafici su macOS
macx: LIBS -= -framework AGL

SOURCES += \
    abstract_activity.cpp \
    activitydetailwidget.cpp \
    activitylistwidget.cpp \
    activitymanager.cpp \
    activitysearchdialog.cpp \
    bill.cpp \
    hometask.cpp \
    jsonpersistencemanager.cpp \
    leisuretimetask.cpp \
    main.cpp \
    mainwindow.cpp \
    shoppingtask.cpp \
    vehiclemaintenance.cpp \
    xmlpersistencemanager.cpp

HEADERS += \
    abstract_activity.h \
    activitydetailwidget.h \
    activitylistwidget.h \
    activitymanager.h \
    activitysearchdialog.h \
    bill.h \
    hometask.h \
    jsonpersistencemanager.h \
    leisuretimetask.h \
    mainwindow.h \
    shoppingtask.h \
    vehiclemaintenance.h \
    xmlpersistencemanager.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
# Dice a qmake di ignorare OpenGL su macOS
CONFIG -= opengl
QMAKE_LFLAGS += -F/Users/sofia/Desktop/FakeFrameworks
