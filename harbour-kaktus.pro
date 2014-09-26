TARGET = harbour-kaktus

DEFINES += SAILFISH
DEFINES += ONLINE_CHECK

## sailfishapp.prf ##

QT += quick qml network sql dbus
target.path = /usr/bin
qml.files = qml/sailfish
qml.path = /usr/share/$${TARGET}/qml
desktop.files = $${TARGET}.desktop
desktop.path = /usr/share/applications
icon.files = $${TARGET}.png
icon.path = /usr/share/icons/hicolor/86x86/apps
INSTALLS += target qml desktop icon
CONFIG += link_pkgconfig
PKGCONFIG += sailfishapp
INCLUDEPATH += /usr/include/sailfishapp
QMAKE_RPATHDIR += /usr/share/$${TARGET}/lib
OTHER_FILES += $$files(rpm/*) \
    qml/sailfish/Guide.qml

##

SOURCES += \
    src/main.cpp \
    src/utils.cpp \
    src/tabmodel.cpp \
    src/netvibesfetcher.cpp \
    src/listmodel.cpp \
    src/feedmodel.cpp \
    src/entrymodel.cpp \
    src/downloadmanager.cpp \
    src/databasemanager.cpp \
    src/dashboardmodel.cpp \
    src/cacheserver.cpp \
    src/settings.cpp \
    src/simplecrypt.cpp
    
HEADERS += \
    src/utils.h \
    src/tabmodel.h \
    src/netvibesfetcher.h \
    src/listmodel.h \
    src/feedmodel.h \
    src/entrymodel.h \
    src/downloadmanager.h \
    src/databasemanager.h \
    src/dashboardmodel.h \
    src/cacheserver.h \
    src/settings.h \
    src/simplecrypt.h \
    src/iconprovider.h
    
#QJson if Qt < 5
lessThan(QT_MAJOR_VERSION, 5) {
    include(./QJson/json.pri)
}
    
# QHttpServer
include(qhttpserver/qhttpserver.pri)

OTHER_FILES += \
    qml/sailfish/*.qml \
    qml/harmattan/*.qml \
    qml/symbian/*.qml \
    rpm/harbour-kaktus.spec \
    harbour-kaktus.desktop \
    i18n_paths.lst \
    i18n_ts.lst \
    lupdate.sh

CODECFORTR = UTF-8

TRANSLATIONS = i18n/kaktus_en.ts \
               i18n/kaktus_pl.ts

RESOURCES += \
    resources.qrc
