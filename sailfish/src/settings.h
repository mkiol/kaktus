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
#include <QQmlContext>
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
    Q_PROPERTY (bool autoOffline READ getAutoOffline WRITE setAutoOffline NOTIFY autoOfflineChanged)
    Q_PROPERTY (bool readerMode READ getReaderMode WRITE setReaderMode NOTIFY readerModeChanged)
    Q_PROPERTY (bool showTabIcons READ getShowTabIcons WRITE setShowTabIcons NOTIFY showTabIconsChanged)
    Q_PROPERTY (bool signedIn READ getSignedIn WRITE setSignedIn NOTIFY signedInChanged)
    Q_PROPERTY (bool showStarredTab READ getShowStarredTab WRITE setShowStarredTab NOTIFY showStarredTabChanged)
    Q_PROPERTY (int filter READ getFilter WRITE setFilter NOTIFY filterChanged)
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
    Q_PROPERTY (float zoom READ getZoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY (bool autoDownloadOnUpdate READ getAutoDownloadOnUpdate WRITE setAutoDownloadOnUpdate NOTIFY autoDownloadOnUpdateChanged)
    Q_PROPERTY (int cachingMode READ getCachingMode WRITE setCachingMode NOTIFY cachingModeChanged)
    Q_PROPERTY (int theme READ getTheme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY (int signinType READ getSigninType WRITE setSigninType NOTIFY signinTypeChanged)
    Q_PROPERTY (bool showBroadcast READ getShowBroadcast WRITE setShowBroadcast NOTIFY showBroadcastChanged)
    Q_PROPERTY (bool showOldestFirst READ getShowOldestFirst WRITE setShowOldestFirst NOTIFY showOldestFirstChanged)
    Q_PROPERTY (bool syncRead READ getSyncRead WRITE setSyncRead NOTIFY syncReadChanged)
    Q_PROPERTY (bool doublePane READ getDoublePane WRITE setDoublePane NOTIFY doublePaneChanged)
    Q_PROPERTY (int clickBehavior READ getClickBehavior WRITE setClickBehavior NOTIFY clickBehaviorChanged)
    Q_PROPERTY (bool expandedMode READ getExpandedMode WRITE setExpandedMode NOTIFY expandedModeChanged)
    Q_PROPERTY (int webviewNavigation READ getWebviewNavigation WRITE setWebviewNavigation NOTIFY webviewNavigationChanged)
    Q_PROPERTY (bool nightMode READ getNightMode WRITE setNightMode NOTIFY nightModeChanged)
    Q_PROPERTY (bool pocketEnabled READ getPocketEnabled WRITE setPocketEnabled NOTIFY pocketEnabledChanged)
    Q_PROPERTY (bool pocketQuickAdd READ getPocketQuickAdd WRITE setPocketQuickAdd NOTIFY pocketQuickAddChanged)
    Q_PROPERTY (bool pocketFavorite READ getPocketFavorite WRITE setPocketFavorite NOTIFY pocketFavoriteChanged)
    Q_PROPERTY (QString pocketToken READ getPocketToken WRITE setPocketToken NOTIFY pocketTokenChanged)
    Q_PROPERTY (QString pocketTags READ getPocketTags WRITE setPocketTags NOTIFY pocketTagsChanged)
    Q_PROPERTY (QString pocketTagsHistory READ getPocketTagsHistory WRITE setPocketTagsHistory NOTIFY pocketTagsHistoryChanged)
    Q_PROPERTY (bool ignoreSslErrors READ getIgnoreSslErrors WRITE setIgnoreSslErrors NOTIFY ignoreSslErrorsChanged)
    Q_PROPERTY (QString imagesDir READ getImagesDir WRITE setImagesDir NOTIFY imagesDirChanged)
public:
    static Settings* instance();
    Fetcher* fetcher;

#ifdef BB10
    bb::cascades::QmlDocument* qml;
#else
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QQmlContext* context;
#else
    QDeclarativeView* view;
#endif
#endif

    //Properties
    void setOfflineMode(bool value);
    bool getOfflineMode();

    void setAutoOffline(bool value);
    bool getAutoOffline();

    void setReaderMode(bool value);
    bool getReaderMode();

    void setNightMode(bool value);
    bool getNightMode();

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

    bool getPocketEnabled();
    void setPocketEnabled(bool value);

    bool getPocketQuickAdd();
    void setPocketQuickAdd(bool value);

    bool getPocketFavorite();
    void setPocketFavorite(bool value);

    void setPocketToken(const QString &value);
    QString getPocketToken();

    void setPocketTags(const QString &value);
    QString getPocketTags();

    void setPocketTagsHistory(const QString &value);
    QString getPocketTagsHistory();

    void setDashboardInUse(const QString &value);
    QString getDashboardInUse();

    void setLocale(const QString &value);
    QString getLocale();

    void setLastUpdateDate(int value);
    int getLastUpdateDate();

    void setCachingMode(int value);
    int getCachingMode();

    void setWebviewNavigation(int value);
    int getWebviewNavigation();

    void setAllowedOrientations(int value);
    int getAllowedOrientations();

    void setOffsetLimit(int value);
    int getOffsetLimit();

    void setImagesDir(const QString &value);
    QString getImagesDir();

    /*
    View modes:
    0 - Tabs->Feeds->Entries
    1 - Tabs->Entries
    2 - Feeds->Entries
    3 - All entries
    4 - Saved entries
    5 - Slow entries
    6 - Liked entries (Old Reader)
    7 - Broadcasted entries (Old Reader)
    */
    void setViewMode(int value);
    int getViewMode();

    void setReinitDB(bool value);
    bool getReinitDB();

    void setFontSize(int value);
    int getFontSize();

    void setZoom(float value);
    float getZoom();

    void setAutoDownloadOnUpdate(bool value);
    bool getAutoDownloadOnUpdate();

    void setSyncRead(bool value);
    bool getSyncRead();

    void setDoublePane(bool value);
    bool getDoublePane();

    void setClickBehavior(int value);
    int getClickBehavior();

    void setTheme(int value);
    int getTheme();

    void setExpandedMode(bool value);
    bool getExpandedMode();

    // Sign in
    //  0 - Netvibes
    //  1 - Netvibes with Twitter
    //  2 - Netvibes with FB
    // 10 - Oldreader
    // 20 - Feedly (not supported)
    // 22 - Feedly with FB (not supported)
    // 30 - Tiny Tiny Rss
    void setSigninType(int);
    int getSigninType();

    // ---

    Q_INVOKABLE QString getSettingsDir();

    QString getDmCacheDir();

    Q_INVOKABLE void setCookie(const QString &value);
    Q_INVOKABLE QString getCookie();
    Q_INVOKABLE void setRefreshCookie(const QString &value);
    Q_INVOKABLE QString getRefreshCookie();

    // Url & Username & Password
    Q_INVOKABLE void setUrl(const QString &value);
    Q_INVOKABLE QString getUrl();
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

    int getFilter();
    void setFilter(int value);

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

    void setIgnoreSslErrors(bool value);
    bool getIgnoreSslErrors();

    Q_INVOKABLE void setHint1Done(bool value);
    Q_INVOKABLE bool getHint1Done();

    Q_INVOKABLE const QList<QVariant> viewModeHistory();

    Q_INVOKABLE QString pocketConsumerKey();

signals:
    void offlineModeChanged();
    void autoOfflineChanged();
    void readerModeChanged();
    void nightModeChanged();
    void showTabIconsChanged();
    void signedInChanged();
    void showStarredTabChanged();
    void filterChanged();
    void dashboardInUseChanged();
    void lastUpdateDateChanged();
    void allowedOrientationsChanged();
    void powerSaveModeChanged();
    void offsetLimitChanged();
    void viewModeChanged();
    void helpDoneChanged();
    void localeChanged();
    void fontSizeChanged();
    void zoomChanged();
    void autoDownloadOnUpdateChanged();
    void themeChanged();
    void cachingModeChanged();
    void signinTypeChanged();
    void showBroadcastChanged();
    void showOldestFirstChanged();
    void syncReadChanged();
    void doublePaneChanged();
    void clickBehaviorChanged();
    void expandedModeChanged();
    void webviewNavigationChanged();
    void pocketEnabledChanged();
    void pocketTokenChanged();
    void pocketTagsChanged();
    void pocketTagsHistoryChanged();
    void pocketFavoriteChanged();
    void pocketQuickAddChanged();
    void ignoreSslErrorsChanged();
    void imagesDirChanged();

    /*
    501 - Unable create settings dir
    502 - Unable create cache dir
    511 - Password decryption error
    512 - Password encryption error
     */
    void error(int);

public slots:
    void reset();

private:
    QSettings settings;
    static Settings *m_instance;

    explicit Settings(QObject *parent = nullptr);
};

#endif // SETTINGS_H
