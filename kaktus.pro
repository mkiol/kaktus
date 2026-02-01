TARGET = harbour-kaktus

CONFIG += c++1z sailfishapp json
PKGCONFIG += mlite5
QT += sql network dbus
DEFINES += QT_NO_URL_CAST_FROM_STRING

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

CONFIG(debug, debug|release) {
    CONFIG += sanitizer sanitize_address
}

INCLUDEPATH += src

include(qhttpserver/qhttpserver.pri)

OTHER_FILES += \
    $$files(qml/*.qml) \
    $$files(rpm/*) \
    $$files(scripts/*)

SOURCES += \
    src/main.cpp \
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
    src/ttrssfetcher.cpp \
    src/networkaccessmanagerfactory.cpp \
    src/customnetworkaccessmanager.cpp \
    src/iconprovider.cpp

HEADERS += \
    src/info.h \
    src/singleton.h \
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
    src/ttrssfetcher.h \
    src/networkaccessmanagerfactory.h \
    src/customnetworkaccessmanager.h \
    src/key.h

SAILFISHAPP_ICONS = 86x86 108x108 128x128 150x150 172x172 256x256
CONFIG += sailfishapp_i18n_include_obsolete
TRANSLATIONS += \
    translations/kaktus_en.ts \
    translations/kaktus_pl.ts \
    translations/kaktus_ru.ts \
    translations/kaktus_cs.ts \
    translations/kaktus_nl_NL.ts \
    translations/kaktus_nl_BE.ts \
    translations/kaktus_tr.ts \
    translations/kaktus_es.ts \
    translations/kaktus_it.ts \
    translations/kaktus_de.ts \
    translations/kaktus_zh_CN.ts \
    translations/kaktus_nb.ts
include(sailfishapp_i18n.pri)

# install

install_images.files = images/*
install_images.path = /usr/share/$${TARGET}/images
INSTALLS += install_images

install_scripts.files = scripts/*
install_scripts.path = /usr/share/$${TARGET}/scripts
INSTALLS += install_scripts

DEPENDPATH += $${INCLUDEPATH}
