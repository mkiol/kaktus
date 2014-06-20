TARGET = kaktus

QT += core network sql

CONFIG += qdeclarative-boostable

contains(MEEGO_EDITION, harmattan) {
    DEFINES += MEEGO_EDITION_HARMATTAN
}

DEFINES += ONLINE_CHECK
DEFINES += MEEGO_EDITION_HARMATTAN

symbian {
  TARGET.UID3 = 0xE7EAC7DC
  TARGET.CAPABILITY += NetworkServices
  CONFIG += qt-components
  DEPLOYMENT.display_name = "Kaktus"
  VERSION = 1.1.0
  ICON = kaktus.svg
}

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
    src/simplecrypt.cpp \
    src/customnetworkaccessmanager.cpp \
    src/networkaccessmanagerfactory.cpp

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
    key.h \
    src/customnetworkaccessmanager.h \
    src/networkaccessmanagerfactory.h

#QJson if Qt < 5
lessThan(QT_MAJOR_VERSION, 5) {
    include(./QJson/json.pri)
}
    
# QHttpServer
include(qhttpserver/qhttpserver.pri)

folder_01.source = qml/harmattan
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01
               
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/changelog \
    i18n_paths.lst \
    i18n_ts.lst \
    lupdate.sh \
    i18n/kaktus_en.ts \
    i18n/kaktus_pl.ts

CODECFORTR = UTF-8

TRANSLATIONS = i18n/kaktus_en.ts \
               i18n/kaktus_pl.ts
    
RESOURCES += resources.qrc
