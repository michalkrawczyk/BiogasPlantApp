QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

win32
{
    MYSQL_DIR      = $$OUT_PWD
    MYSQL_DIR      ~= s,/,\\,g
}
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Database/Src/database.cpp \
    Database/Src/db_manager.cpp \
    Database/Src/db_messages.cpp \
    Database/Src/db_mysql.cpp \
    Database/Src/db_sqlite.cpp \
    GUI/Src/login.cpp \
    Misc/Src/layouts.cpp \
    Misc/Src/utils.cpp \
    Misc/Src/validators.cpp \
    main.cpp \
    GUI/Src/menu.cpp

HEADERS += \
    Database/Inc/database.h \
    Database/Inc/db_manager.h \
    Database/Inc/db_messages.h \
    Database/Inc/db_mysql.h \
    Database/Inc/db_sqlite.h \
    GUI/Inc/login.h \
    GUI/Inc/menu.h \
    Misc/Inc/layouts.h \
    Misc/Inc/utils.h \
    Misc/Inc/validators.h

FORMS += \
    GUI/Forms/login.ui \
    GUI/Forms/menu.ui

TRANSLATIONS += \
    Prototype_ver01_pl_PL.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    TODO.txt
