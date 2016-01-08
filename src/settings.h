/*
  Copyright (C) 2014 Michal Kosciesza <michal@mkiol.net>

  This file is part of Kaktus.

  Kaktus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Kaktus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Kaktus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QList>
#include <QVariant>

#ifdef BB10
#include <bb/cascades/QmlDocument>
#else
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QQuickView>
#else
#include <QDeclarativeView>
#endif
#endif

class DatabaseManager;
class DownloadManager;
class CacheServer;
//class NetvibesFetcher;
class Fetcher;

class Settings: public QObject
{
    Q_OBJECT

    Q_PROPERTY (bool offlineMode READ getOfflineMode WRITE setOfflineMode NOTIFY offlineModeChanged)
    Q_PROPERTY (bool readerMode READ getReaderMode WRITE setReaderMode NOTIFY readerModeChanged)
    Q_PROPERTY (bool showTabIcons READ getShowTabIcons WRITE setShowTabIcons NOTIFY showTabIconsChanged)
    Q_PROPERTY (bool signedIn READ getSignedIn WRITE setSignedIn NOTIFY signedInChanged)
    Q_PROPERTY (bool showStarredTab READ getShowStarredTab WRITE setShowStarredTab NOTIFY showStarredTabChanged)
    Q_PROPERTY (bool showOnlyUnread READ getShowOnlyUnread WRITE setShowOnlyUnread NOTIFY showOnlyUnreadChanged)
    Q_PROPERTY (QString dashboardInUse READ getDashboardInUse WRITE setDashboardInUse NOTIFY dashboardInUseChanged)
    Q_PROPERTY (int lastUpdateDate READ getLastUpdateDate WRITE setLastUpdateDate NOTIFY lastUpdateDateChanged)
    Q_PROPERTY (int allowedOrientations READ getAllowedOrientations WRITE setAllowedOrientations NOTIFY allowedOrientationsChanged)
    Q_PROPERTY (bool powerSaveMode READ getPowerSaveMode WRITE setPowerSaveMode NOTIFY powerSaveModeChanged)
    Q_PROPERTY (int offsetLimit READ getOffsetLimit WRITE setOffsetLimit NOTIFY offsetLimitChanged)
    Q_PROPERTY (int viewMode READ getViewMode WRITE setViewMode NOTIFY viewModeChanged)
    Q_PROPERTY (bool helpDone READ getHelpDone WRITE setHelpDone NOTIFY helpDoneChanged)
    Q_PROPERTY (bool reinitDB READ getReinitDB WRITE setReinitDB)
    Q_PROPERTY (QString locale READ getLocale WRITE setLocale NOTIFY localeChanged)
    Q_PROPERTY (int fontSize READ getFontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY (QString offlineTheme READ getOfflineTheme WRITE setOfflineTheme NOTIFY offlineThemeChanged)
    Q_PROPERTY (bool autoDownloadOnUpdate READ getAutoDownloadOnUpdate WRITE setAutoDownloadOnUpdate NOTIFY autoDownloadOnUpdateChanged)
    Q_PROPERTY (int cachingMode READ getCachingMode WRITE setCachingMode NOTIFY cachingModeChanged)
    Q_PROPERTY (int theme READ getTheme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY (int signinType READ getSigninType WRITE setSigninType NOTIFY signinTypeChanged)
    Q_PROPERTY (bool showBroadcast READ getShowBroadcast WRITE setShowBroadcast NOTIFY showBroadcastChanged)
    Q_PROPERTY (bool showOldestFirst READ getShowOldestFirst WRITE setShowOldestFirst NOTIFY showOldestFirstChanged)
    Q_PROPERTY (bool syncRead READ getSyncRead WRITE setSyncRead NOTIFY syncReadChanged)
    Q_PROPERTY (bool openInBrowser READ getOpenInBrowser WRITE setOpenInBrowser NOTIFY openInBrowserChanged)
    Q_PROPERTY (bool iconContextMenu READ getIconContextMenu WRITE setIconContextMenu NOTIFY iconContextMenuChanged)
    Q_PROPERTY (bool doublePane READ getDoublePane WRITE setDoublePane NOTIFY doublePaneChanged)

public:
    static Settings* instance();

    DatabaseManager* db;
    CacheServer* cache;
    DownloadManager* dm;
    Fetcher* fetcher;

#ifdef BB10
    bb::cascades::QmlDocument* qml;
#else
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QQuickView* view;
#else
    QDeclarativeView* view;
#endif
#endif

    //Properties
    void setOfflineMode(bool value);
    bool getOfflineMode();

    void setReaderMode(bool value);
    bool getReaderMode();

    void setPowerSaveMode(bool value);
    bool getPowerSaveMode();

    bool getShowTabIcons();
    void setShowTabIcons(bool value);

    void setSignedIn(bool value);
    bool getSignedIn();

    void setHelpDone(bool value);
    bool getHelpDone();

    bool getShowStarredTab();
    void setShowStarredTab(bool value);

    bool getShowBroadcast();
    void setShowBroadcast(bool value);

    bool getIconContextMenu();
    void setIconContextMenu(bool value);

    void setDashboardInUse(const QString &value);
    QString getDashboardInUse();

    void setLocale(const QString &value);
    QString getLocale();

    void setLastUpdateDate(int value);
    int getLastUpdateDate();

    void setCachingMode(int value);
    int getCachingMode();

    void setAllowedOrientations(int value);
    int getAllowedOrientations();

    void setOffsetLimit(int value);
    int getOffsetLimit();

    void setViewMode(int value);
    int getViewMode();

    void setReinitDB(bool value);
    bool getReinitDB();

    void setFontSize(int value);
    int getFontSize();

    void setOfflineTheme(const QString &value);
    QString getOfflineTheme();

    void setAutoDownloadOnUpdate(bool value);
    bool getAutoDownloadOnUpdate();

    void setSyncRead(bool value);
    bool getSyncRead();

    void setOpenInBrowser(bool value);
    bool getOpenInBrowser();

    void setDoublePane(bool value);
    bool getDoublePane();

    void setTheme(int value);
    int getTheme();

    // Sign in
    //  0 - Netvibes
    //  1 - Netvibes with Twitter
    //  2 - Netvibes with FB
    // 10 - Oldreader
    // 20 - Feedly
    // 22 - Feedly with FB
    void setSigninType(int);
    int getSigninType();

    // ---

    Q_INVOKABLE QString getSettingsDir();

    QString getDmCacheDir();

    Q_INVOKABLE void setCookie(const QString &value);
    Q_INVOKABLE QString getCookie();
    Q_INVOKABLE void setRefreshCookie(const QString &value);
    Q_INVOKABLE QString getRefreshCookie();

    // Username & Password
    Q_INVOKABLE void setUsername(const QString &value);
    Q_INVOKABLE QString getUsername();
    Q_INVOKABLE void setPassword(const QString &value);
    Q_INVOKABLE QString getPassword();
    void setUserId(const QString &value);
    QString getUserId();

    // Twitter & FB
    Q_INVOKABLE void setTwitterCookie(const QString &value);
    Q_INVOKABLE QString getTwitterCookie();
    Q_INVOKABLE void setAuthUrl(const QString &value);
    Q_INVOKABLE QString getAuthUrl();
    Q_INVOKABLE void setProvider(const QString &value);
    Q_INVOKABLE QString getProvider();

    Q_INVOKABLE void setDmUserAgent(const QString &value);
    Q_INVOKABLE QString getDmUserAgent();

    Q_INVOKABLE void setRetentionDays(int value);
    Q_INVOKABLE int getRetentionDays();

    bool getShowOnlyUnread();
    void setShowOnlyUnread(bool value);

    bool getShowOldestFirst();
    void setShowOldestFirst(bool value);

    void setDmConnections(int value);
    int getDmConnections();

    void setDmTimeOut(int value);
    int getDmTimeOut();

    void setDmMaxSize(int value);
    int getDmMaxSize();

    void setFeedsAtOnce(int value);
    int getFeedsAtOnce();

    void setFeedsUpdateAtOnce(int value);
    int getFeedsUpdateAtOnce();

    Q_INVOKABLE void setHint1Done(bool value);
    Q_INVOKABLE bool getHint1Done();

    Q_INVOKABLE const QList<QVariant> viewModeHistory();

signals:
    void offlineModeChanged();
    void readerModeChanged();
    void showTabIconsChanged();
    void signedInChanged();
    void showStarredTabChanged();
    void showOnlyUnreadChanged();
    void dashboardInUseChanged();
    void lastUpdateDateChanged();
    void allowedOrientationsChanged();
    void powerSaveModeChanged();
    void offsetLimitChanged();
    void viewModeChanged();
    void helpDoneChanged();
    void localeChanged();
    void offlineThemeChanged();
    void fontSizeChanged();
    void autoDownloadOnUpdateChanged();
    void themeChanged();
    void cachingModeChanged();
    void signinTypeChanged();
    void showBroadcastChanged();
    void showOldestFirstChanged();
    void syncReadChanged();
    void openInBrowserChanged();
    void iconContextMenuChanged();
    void doublePaneChanged();

    /*
    501 - Unable create settings dir
    502 - Unable create cache dir
    511 - Password decryption error
    512 - Password encryption error
     */
    void error(int);

public Q_SLOTS:
    void reset();

private:
    QSettings settings;
    static Settings *inst;

    explicit Settings(QObject *parent = 0);
};

#endif // SETTINGS_H
