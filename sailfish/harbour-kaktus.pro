TARGET = harbour-kaktus

CONFIG += c++11 sailfishapp json no_lflags_merge object_parallel_to_source

QT += sql network dbus

PKGCONFIG += mlite5

linux-g++-32: CONFIG += x86
linux-g++: CONFIG += arm

DEFINES += SAILFISH
DEFINES += ONLINE_CHECK

INCLUDEPATH += src

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
    key.h

# libs
include(qhttpserver/qhttpserver.pri)

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
    translations/kaktus_zh_CN.ts
include(sailfishapp_i18n.pri)

images.files = images/*
images.path = /usr/share/$${TARGET}/images
INSTALLS += images

OTHER_FILES += \
    qml/AboutPage.qml \
    qml/AccountsDialog.qml \
    qml/ActiveDetector.qml \
    qml/AuthWebViewPage.qml \
    qml/ButtonItem.qml \
    qml/CachedImage.qml \
    qml/ChangelogPage.qml \
    qml/ControlBar.qml \
    qml/CoverPage.qml \
    qml/DashboardDialog.qml \
    qml/DashboardPage.qml \
    qml/DebugPage.qml \
    qml/Dot.qml \
    qml/EntryDelegate.qml \
    qml/EntryPageContent.qml \
    qml/EntryPage.qml \
    qml/ErrorPage.qml \
    qml/FeedIcon.qml \
    qml/FeedPage.qml \
    qml/FeedWebContentPage.qml \
    qml/FirstPage.qml \
    qml/HintLabel.qml \
    qml/IconBarItem.qml \
    qml/IconBar.qml \
    qml/IconContextMenu.qml \
    qml/IconMenuItem.qml \
    qml/IconPlaceholder.qml \
    qml/IconSlider.qml \
    qml/LogItem.qml \
    qml/main.qml \
    qml/MenuIconItem.qml \
    qml/Notification.qml \
    qml/NvSignInDialog.qml \
    qml/OldReaderSignInDialog.qml \
    qml/PaddedLabel.qml \
    qml/PageMenu.qml \
    qml/PocketAuthWebViewPage.qml \
    qml/PocketDialog.qml \
    qml/Pocket.qml \
    qml/ProgressPanel.qml \
    qml/ReadAllDialog.qml \
    qml/SettingsPage.qml \
    qml/ShareDialog.qml \
    qml/ShareLinkPage.qml \
    qml/SignOutDialog.qml \
    qml/SmallIconButton.qml \
    qml/Spacer.qml \
    qml/TabPage.qml \
    qml/TempBaner.qml \
    qml/TextAreaItem.qml \
    qml/TextFieldItem.qml \
    qml/TextSwitchWithIcon.qml \
    qml/TTRssSignInDialog.qml \
    qml/UnreadAllDialog.qml \
    qml/WebPreviewPage.qml \
    qml/ClickableLabel.qml \
    qml/SimpleListItem.qml \
    qml/UnreadBox.qml

OTHER_FILES += \
    rpm/$${TARGET}.yaml \
    rpm/$${TARGET}.changes.in \
    rpm/$${TARGET}.spec
