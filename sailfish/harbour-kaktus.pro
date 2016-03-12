TARGET = harbour-kaktus

DEFINES += SAILFISH
DEFINES += ONLINE_CHECK

## sailfishapp.prf ##

QT += quick qml network sql
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
    qml/sailfish/NvSignInDialog.qml

##

SOURCES += \
    src/main_sailfish.cpp \
    src/utils.cpp \
    src/tabmodel.cpp \
    src/listmodel.cpp \
    src/feedmodel.cpp \
    src/entrymodel.cpp \
    src/downloadmanager.cpp \
    src/databasemanager.cpp \
    src/dashboardmodel.cpp \
    src/cacheserver.cpp \
    src/settings.cpp \
    src/simplecrypt.cpp \
    src/nviconprovider.cpp \
    src/fetcher.cpp \
    src/oldreaderfetcher.cpp \
    src/nvfetcher.cpp \
    src/feedlyfetcher.cpp \
    src/networkaccessmanagerfactory.cpp \
    src/customnetworkaccessmanager.cpp \
    src/debugunit.cpp

HEADERS += \
    src/utils.h \
    src/tabmodel.h \
    src/listmodel.h \
    src/feedmodel.h \
    src/entrymodel.h \
    src/downloadmanager.h \
    src/databasemanager.h \
    src/dashboardmodel.h \
    src/cacheserver.h \
    src/settings.h \
    src/simplecrypt.h \
    src/iconprovider.h \
    src/nviconprovider.h \
    src/fetcher.h \
    src/oldreaderfetcher.h \
    src/nvfetcher.h \
    src/feedlyfetcher.h \
    src/networkaccessmanagerfactory.h \
    src/customnetworkaccessmanager.h \
    feedly.h \
    key.h \
    src/debugunit.h
    
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
               i18n/kaktus_pl.ts \
               i18n/kaktus_fa.ts \
               i18n/kaktus_ru.ts \
               i18n/kaktus_cs.ts \
               i18n/kaktus_nl.ts \
               i18n/kaktus_tr.ts \
               i18n/kaktus_de.ts \
               i18n/kaktus_es.ts \
               i18n/kaktus_fi.ts \
               i18n/kaktus_it.ts \
               i18n/kaktus_zh_CN.ts

RESOURCES += \
    resources.qrc

DISTFILES += \
    qml/sailfish/DebugPage.qml \
    qml/sailfish/EntryPageOld.qml \
    qml/sailfish/EntryPage.qml
