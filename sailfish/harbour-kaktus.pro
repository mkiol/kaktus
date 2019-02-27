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
    src/ai.cpp \
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
    src/ttrssfetcher.cpp \
    src/networkaccessmanagerfactory.cpp \
    src/customnetworkaccessmanager.cpp \
    src/iconprovider.cpp

HEADERS += \
    src/info.h \
    src/ai.h \
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
    src/ttrssfetcher.h \
    src/networkaccessmanagerfactory.h \
    src/customnetworkaccessmanager.h \
    feedly.h \
    key.h

# libs
include(qhttpserver/qhttpserver.pri)

SAILFISHAPP_ICONS = 86x86 108x108 128x128 150x150 256x256

CONFIG += sailfishapp_i18n_include_obsolete
TRANSLATIONS += \
    translations/kaktus_en.ts \
    translations/kaktus_pl.ts \
    translations/kaktus_ru.ts \
    translations/kaktus_cs.ts \
    translations/kaktus_nl.ts \
    translations/kaktus_tr.ts \
    translations/kaktus_es.ts \
    translations/kaktus_it.ts \
    translations/kaktus_de.ts
include(sailfishapp_i18n.pri)

images.files = images/*
images.path = /usr/share/$${TARGET}/images
INSTALLS += images

OTHER_FILES += \
    AboutPage.qml \
    AccountsDialog.qml \
    ActiveDetector.qml \
    AuthWebViewPage.qml \
    ButtonItem.qml \
    CachedImage.qml \
    ChangelogPage.qml \
    ControlBar.qml \
    CoverPage.qml \
    DashboardDialog.qml \
    DashboardPage.qml \
    DebugPage.qml \
    Dot.qml \
    EntryDelegate.qml \
    EntryPageContent.qml \
    EntryPage.qml \
    ErrorPage.qml \
    FeedIcon.qml \
    FeedlySignInDialog.qml \
    FeedPage.qml \
    FeedWebContentPage.qml \
    FirstPage.qml \
    Guide.qml \
    HintLabel.qml \
    IconBarItem.qml \
    IconBar.qml \
    IconContextMenu.qml \
    IconMenuItem.qml \
    IconPlaceholder.qml \
    IconSlider.qml \
    LogItem.qml \
    main.qml \
    MenuIconItem.qml \
    Notification.qml \
    NvSignInDialog.qml \
    OldReaderSignInDialog.qml \
    PaddedLabel.qml \
    PageMenu.qml \
    PocketAuthWebViewPage.qml \
    PocketDialog.qml \
    Pocket.qml \
    ProgressPanel.qml \
    ReadAllDialog.qml \
    SettingsPage.qml \
    ShareDialog.qml \
    ShareLinkPage.qml \
    SignOutDialog.qml \
    SmallIconButton.qml \
    Spacer.qml \
    TabPage.qml \
    TempBaner.qml \
    TextAreaItem.qml \
    TextFieldItem.qml \
    TextSwitchWithIcon.qml \
    TTRssSignInDialog.qml \
    UnreadAllDialog.qml \
    WebPreviewPage.qml

OTHER_FILES += \
    rpm/$${TARGET}.yaml \
    rpm/$${TARGET}.changes.in \
    rpm/$${TARGET}.spec

