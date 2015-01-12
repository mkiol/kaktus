# Add more folders to ship with the application, here
folder_01.source = qml/kaktus
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

QT += core network sql

symbian {
    TARGET.UID3 = 0xE7EAC7DC
    TARGET.CAPABILITY += NetworkServices
    DEPLOYMENT.display_name = "Kaktus"
    VERSION = 1.2.2
    ICON = kaktus.svg
}

# Add dependency to Symbian components
CONFIG += qt-components

#DEFINES += ONLINE_CHECK

#QJson if Qt < 5
lessThan(QT_MAJOR_VERSION, 5) {
    include(./QJson/json.pri)
}

# QHttpServer
include(qhttpserver/qhttpserver.pri)

SOURCES += \
    src/main_symbian.cpp \
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

CODECFORTR = UTF-8

TRANSLATIONS = i18n/kaktus_en.ts \
               i18n/kaktus_pl.ts \
               i18n/kaktus_fa.ts \
               i18n/kaktus_ru.ts \
               i18n/kaktus_nl.ts \
               i18n/kaktus_cs.ts

RESOURCES += resources.qrc

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()
