TARGET = harbour-kaktus

CONFIG += sailfishapp

DEFINES += SAILFISH
DEFINES += ONLINE_CHECK

QT += sql network

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
    src/customnetworkaccessmanager.cpp

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
    key.h
    
#QJson if Qt < 5
lessThan(QT_MAJOR_VERSION, 5) {
    include(./QJson/json.pri)
}
    
# QHttpServer
include(qhttpserver/qhttpserver.pri)

OTHER_FILES += \
    rpm/harbour-kaktus.*

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

TRANSLATIONS = translations/kaktus_en.ts \
               translations/kaktus_pl.ts \
               translations/kaktus_ru.ts \
               translations/kaktus_cs.ts \
               translations/kaktus_nl.ts \
               translations/kaktus_tr.ts \
               translations/kaktus_es.ts \
               translations/kaktus_it.ts

translations.files = translations/*.qm
translations.path = /usr/share/$${TARGET}/translations
INSTALLS += translations

DISTFILES += \
    qml/*.qml
