TARGET = harbour-kaktus

QT += core network sql

CONFIG += sailfishapp

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
    src/settings.cpp

OTHER_FILES += \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/harbour-kaktus.spec \
    rpm/harbour-kaktus.yaml \
    harbour-kaktus.desktop \
    qml/main.qml \
    qml/TabPage.qml \
    qml/InitPage.qml \
    qml/FeedPage.qml \
    qml/EntryPage.qml \
    qml/DashboardPage.qml \
    qml/WebViewPage.qml \
    qml/ErrorPage.qml \
    qml/SignInDialog.qml \
    qml/BusyPanel.qml \
    qml/DownloadDialog.qml \
    qml/EntryDelegate.qml \
    qml/MainMenu.qml \
    qml/AboutPage.qml \
    qml/icon.png \
    qml/SettingsPage.qml \
    qml/AdvancedSettingsPage.qml \
    qml/EmptyPage.qml

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
    src/settings.h

# QHttpServer
include(qhttpserver/qhttpserver.pro)
