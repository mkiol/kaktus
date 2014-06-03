TARGET = harbour-kaktus

QT += core network sql dbus

CONFIG += sailfishapp

DEFINES += SAILFISH

DEFINES += ONLINE_CHECK

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
    src/simplecrypt.h
    
#QJson if Qt < 5
lessThan(QT_MAJOR_VERSION, 5) {
    include(./QJson/json.pri)
}
    
# QHttpServer
include(qhttpserver/qhttpserver.pri)

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
    qml/ErrorPage.qml \
    qml/SignInDialog.qml \
    qml/EntryDelegate.qml \
    qml/MainMenu.qml \
    qml/AboutPage.qml \
    qml/icon.png \
    qml/SettingsPage.qml \
    qml/Notification.qml \
    qml/DashboardPage.qml \
    qml/DashboardDialog.qml \
    qml/WebPreviewPage.qml \
    qml/ControlBarWebPreview.qml \
    i18n_paths.lst \
    i18n_ts.lst \
    lupdate.sh \
    qml/CoverPage.qml \
    qml/ProgressPanel.qml
    
CODECFORTR = UTF-8

TRANSLATIONS = i18n/kaktus_en.ts \
               i18n/kaktus_pl.ts

RESOURCES += \
    resources.qrc
