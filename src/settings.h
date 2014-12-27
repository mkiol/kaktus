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
class NetvibesFetcher;

class Settings: public QObject
{
    Q_OBJECT

    Q_PROPERTY (bool offlineMode READ getOfflineMode WRITE setOfflineMode NOTIFY offlineModeChanged)
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

public:
    static Settings* instance();

    DatabaseManager* db;
    CacheServer* cache;
    DownloadManager* dm;
    NetvibesFetcher* fetcher;

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

    void setDashboardInUse(const QString &value);
    QString getDashboardInUse();

    void setLocale(const QString &value);
    QString getLocale();

    void setLastUpdateDate(int value);
    int getLastUpdateDate();

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

    // ---

    bool getShowOnlyUnread();
    void setShowOnlyUnread(bool value);

    Q_INVOKABLE QString getSettingsDir();

    Q_INVOKABLE void reset();

    QString getDmCacheDir();

    Q_INVOKABLE void setNetvibesUsername(const QString &value);
    Q_INVOKABLE QString getNetvibesUsername();

    Q_INVOKABLE void setNetvibesPassword(const QString &value);
    Q_INVOKABLE QString getNetvibesPassword();

    Q_INVOKABLE void setDmUserAgent(const QString &value);
    Q_INVOKABLE QString getDmUserAgent();

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

signals:
    void offlineModeChanged();
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

    /*
    501 - Unable create settings dir
    502 - Unable create cache dir
    511 - Password decryption error
    512 - Password encryption error
     */
    void error(int);

private:
    QSettings settings;
    static Settings *inst;

    explicit Settings(QObject *parent = 0);
};

#endif // SETTINGS_H
