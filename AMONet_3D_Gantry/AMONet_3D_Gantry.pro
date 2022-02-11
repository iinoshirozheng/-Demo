QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AMONet_Controller.cpp \
    argument_dialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AMONet_Controller.h \
    argument_dialog.h \
    mainwindow.h

FORMS += \
    argument_dialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

macx|win32: LIBS += -L'C:/Program Files (x86)/Advantech/AMONet/Lib/X64/' -lAMONet

INCLUDEPATH += 'C:/Program Files (x86)/Advantech/AMONet/Lib/X64'
DEPENDPATH += 'C:/Program Files (x86)/Advantech/AMONet/Lib/X64'


INCLUDEPATH += 'C:\Program Files (x86)\Advantech\AMONet\Include'
