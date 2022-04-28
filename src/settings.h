/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include <QObject>
#include <QQmlContext>
#include <QSettings>
#include <QString>
#include <QUrl>
#include <QVariant>

class DatabaseManager;
class DownloadManager;
class CacheServer;
class Fetcher;

class Settings : public QSettings {
    Q_OBJECT

    Q_PROPERTY(bool offlineMode READ getOfflineMode WRITE setOfflineMode NOTIFY
                   offlineModeChanged)
    Q_PROPERTY(bool autoOffline READ getAutoOffline WRITE setAutoOffline NOTIFY
                   autoOfflineChanged)
    Q_PROPERTY(bool readerMode READ getReaderMode WRITE setReaderMode NOTIFY
                   readerModeChanged)
    Q_PROPERTY(bool showTabIcons READ getShowTabIcons WRITE setShowTabIcons
                   NOTIFY showTabIconsChanged)
    Q_PROPERTY(
        bool signedIn READ getSignedIn WRITE setSignedIn NOTIFY signedInChanged)
    Q_PROPERTY(bool showStarredTab READ getShowStarredTab WRITE
                   setShowStarredTab NOTIFY showStarredTabChanged)
    Q_PROPERTY(int filter READ getFilter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString dashboardInUse READ getDashboardInUse WRITE
                   setDashboardInUse NOTIFY dashboardInUseChanged)
    Q_PROPERTY(int lastUpdateDate READ getLastUpdateDate WRITE setLastUpdateDate
                   NOTIFY lastUpdateDateChanged)
    Q_PROPERTY(int allowedOrientations READ getAllowedOrientations WRITE
                   setAllowedOrientations NOTIFY allowedOrientationsChanged)
    Q_PROPERTY(int offsetLimit READ getOffsetLimit WRITE setOffsetLimit NOTIFY
                   offsetLimitChanged)
    Q_PROPERTY(ViewMode viewMode READ getViewMode WRITE setViewMode NOTIFY
                   viewModeChanged)
    Q_PROPERTY(int viewModeNum READ getViewModeNum NOTIFY viewModeChanged)
    Q_PROPERTY(
        bool helpDone READ getHelpDone WRITE setHelpDone NOTIFY helpDoneChanged)
    Q_PROPERTY(
        QString locale READ getLocale WRITE setLocale NOTIFY localeChanged)
    Q_PROPERTY(
        int fontSize READ getFontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(float zoom READ getZoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(bool autoDownloadOnUpdate READ getAutoDownloadOnUpdate WRITE
                   setAutoDownloadOnUpdate NOTIFY autoDownloadOnUpdateChanged)
    Q_PROPERTY(int cachingMode READ getCachingMode WRITE setCachingMode NOTIFY
                   cachingModeChanged)
    Q_PROPERTY(int theme READ getTheme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(int signinType READ getSigninType WRITE setSigninType NOTIFY
                   signinTypeChanged)
    Q_PROPERTY(bool showBroadcast READ getShowBroadcast WRITE setShowBroadcast
                   NOTIFY showBroadcastChanged)
    Q_PROPERTY(bool showOldestFirst READ getShowOldestFirst WRITE
                   setShowOldestFirst NOTIFY showOldestFirstChanged)
    Q_PROPERTY(
        bool syncRead READ getSyncRead WRITE setSyncRead NOTIFY syncReadChanged)
    Q_PROPERTY(bool doublePane READ getDoublePane WRITE setDoublePane NOTIFY
                   doublePaneChanged)
    Q_PROPERTY(int clickBehavior READ getClickBehavior WRITE setClickBehavior
                   NOTIFY clickBehaviorChanged)
    Q_PROPERTY(bool expandedMode READ getExpandedMode WRITE setExpandedMode
                   NOTIFY expandedModeChanged)
    Q_PROPERTY(int webviewNavigation READ getWebviewNavigation WRITE
                   setWebviewNavigation NOTIFY webviewNavigationChanged)
    Q_PROPERTY(bool nightMode READ getNightMode WRITE setNightMode NOTIFY
                   nightModeChanged)
    Q_PROPERTY(bool pocketEnabled READ getPocketEnabled WRITE setPocketEnabled
                   NOTIFY pocketEnabledChanged)
    Q_PROPERTY(bool pocketQuickAdd READ getPocketQuickAdd WRITE
                   setPocketQuickAdd NOTIFY pocketQuickAddChanged)
    Q_PROPERTY(bool pocketFavorite READ getPocketFavorite WRITE
                   setPocketFavorite NOTIFY pocketFavoriteChanged)
    Q_PROPERTY(QString pocketToken READ getPocketToken WRITE setPocketToken
                   NOTIFY pocketTokenChanged)
    Q_PROPERTY(QString pocketTags READ getPocketTags WRITE setPocketTags NOTIFY
                   pocketTagsChanged)
    Q_PROPERTY(QString pocketTagsHistory READ getPocketTagsHistory WRITE
                   setPocketTagsHistory NOTIFY pocketTagsHistoryChanged)
    Q_PROPERTY(bool ignoreSslErrors READ getIgnoreSslErrors WRITE
                   setIgnoreSslErrors NOTIFY ignoreSslErrorsChanged)
    Q_PROPERTY(QString imagesDir READ getImagesDir WRITE setImagesDir NOTIFY
                   imagesDirChanged)

   public:
    enum class ViewMode {
        TabsFeedsEntries = 0,
        TabsEntries = 1,
        FeedsEntries = 2,
        AllEntries = 3,
        SavedEntries = 4,
        SlowEntries = 5,
        LikedEntries = 6,
        BroadcastedEntries = 7
    };
    Q_ENUM(ViewMode)

    static Settings *instance();

    Fetcher *fetcher = nullptr;

    inline void setContext(QQmlContext *context) { m_context = context; }
    inline void setContextProperty(const QString &name, QObject *value) {
        if (m_context) m_context->setContextProperty(name, value);
    }

    void setOfflineMode(bool value);
    bool getOfflineMode() const;

    void setAutoOffline(bool value);
    bool getAutoOffline() const;

    void setReaderMode(bool value);
    bool getReaderMode() const;

    void setNightMode(bool value);
    bool getNightMode() const;

    bool getShowTabIcons() const;
    void setShowTabIcons(bool value);

    void setSignedIn(bool value);
    bool getSignedIn() const;

    void setHelpDone(bool value);
    bool getHelpDone() const;

    bool getShowStarredTab() const;
    void setShowStarredTab(bool value);

    bool getShowBroadcast() const;
    void setShowBroadcast(bool value);

    bool getPocketEnabled() const;
    void setPocketEnabled(bool value);

    bool getPocketQuickAdd() const;
    void setPocketQuickAdd(bool value);

    bool getPocketFavorite() const;
    void setPocketFavorite(bool value);

    void setPocketToken(const QString &value);
    QString getPocketToken();

    void setPocketTags(const QString &value);
    QString getPocketTags() const;

    void setPocketTagsHistory(const QString &value);
    QString getPocketTagsHistory() const;

    void setDashboardInUse(const QString &value);
    QString getDashboardInUse() const;

    void setLocale(const QString &value);
    QString getLocale() const;

    void setLastUpdateDate(int value);
    int getLastUpdateDate() const;

    void setCachingMode(int value);
    int getCachingMode() const;

    void setWebviewNavigation(int value);
    int getWebviewNavigation() const;

    void setAllowedOrientations(int value);
    int getAllowedOrientations() const;

    void setOffsetLimit(int value);
    int getOffsetLimit() const;

    void setImagesDir(const QString &value);
    QString getImagesDir() const;

    void setViewMode(ViewMode mode);
    ViewMode getViewMode() const;
    inline int getViewModeNum() const {
        return static_cast<int>(getViewMode());
    }

    void setReinitDB(bool value);
    bool getReinitDB() const;

    void setFontSize(int value);
    int getFontSize() const;

    void setZoom(float value);
    float getZoom() const;
    Q_INVOKABLE QString zoomViewport() const;
    Q_INVOKABLE QString zoomFontSize() const;

    void setAutoDownloadOnUpdate(bool value);
    bool getAutoDownloadOnUpdate() const;

    void setSyncRead(bool value);
    bool getSyncRead() const;

    void setDoublePane(bool value);
    bool getDoublePane() const;

    void setClickBehavior(int value);
    int getClickBehavior() const;

    void setTheme(int value);
    int getTheme() const;

    void setExpandedMode(bool value);
    bool getExpandedMode() const;

    // Sign in
    //  0 - Netvibes
    //  1 - Netvibes with Twitter
    //  2 - Netvibes with FB
    // 10 - Oldreader
    // 20 - Feedly (not supported)
    // 22 - Feedly with FB (not supported)
    // 30 - Tiny Tiny Rss
    void setSigninType(int);
    int getSigninType() const;

    Q_INVOKABLE QString getSettingsDir();
    QString getDmCacheDir();

    Q_INVOKABLE void setCookie(const QString &value);
    Q_INVOKABLE QString getCookie();

    Q_INVOKABLE void setRefreshCookie(const QString &value);
    Q_INVOKABLE QString getRefreshCookie();

    Q_INVOKABLE void setUrl(const QString &value);
    Q_INVOKABLE QString getUrl() const;

    Q_INVOKABLE void setUsername(const QString &value);
    Q_INVOKABLE QString getUsername() const;

    Q_INVOKABLE void setPassword(const QString &value);
    Q_INVOKABLE QString getPassword();

    void setUserId(const QString &value);
    QString getUserId() const;

    Q_INVOKABLE void setTwitterCookie(const QString &value);
    Q_INVOKABLE QString getTwitterCookie();

    Q_INVOKABLE void setAuthUrl(const QString &value);
    Q_INVOKABLE QString getAuthUrl();

    Q_INVOKABLE void setProvider(const QString &value);
    Q_INVOKABLE QString getProvider() const;

    QString getDmUserAgent() const;

    Q_INVOKABLE void setRetentionDays(int value);
    Q_INVOKABLE int getRetentionDays() const;

    int getFilter() const;
    void setFilter(int value);

    bool getShowOldestFirst() const;
    void setShowOldestFirst(bool value);

    void setDmConnections(int value);
    int getDmConnections() const;

    void setDmTimeOut(int value);
    int getDmTimeOut() const;

    void setDmMaxSize(int value);
    int getDmMaxSize() const;

    void setFeedsAtOnce(int value);
    int getFeedsAtOnce() const;

    void setFeedsUpdateAtOnce(int value);
    int getFeedsUpdateAtOnce() const;

    void setIgnoreSslErrors(bool value);
    bool getIgnoreSslErrors() const;

    Q_INVOKABLE void setHint1Done(bool value);
    Q_INVOKABLE bool getHint1Done() const;

    void setViewModeHistory(QList<ViewMode> history);
    QList<ViewMode> viewModeHistory() const;

    Q_INVOKABLE QString pocketConsumerKey() const;
    Q_INVOKABLE QUrl appIcon() const;

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
    inline static const QString settingsFilename =
        QStringLiteral("settings.conf");
    static constexpr const float maxZoom = 2.0;
    static constexpr const float minZoom = 0.5;
    static Settings *m_instance;
    QQmlContext *m_context = nullptr;
    Settings();
    static QString settingsFilepath();
};

#endif  // SETTINGS_H
