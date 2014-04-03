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

#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QQuickView>
#include <QQmlContext>

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

public:
    static Settings* instance();

    DatabaseManager* db;
    CacheServer* cache;
    DownloadManager* dm;
    NetvibesFetcher* fetcher;
    QQuickView* view;

    //Properties
    void setOfflineMode(bool value);
    bool getOfflineMode();
    bool getShowTabIcons();
    void setShowTabIcons(bool value);
    void setSignedIn(bool value);
    bool getSignedIn();
    bool getShowStarredTab();
    void setShowStarredTab(bool value);

    // General
    Q_INVOKABLE QString getSettingsDir();
    Q_INVOKABLE void setAutoDownloadOnUpdate(bool value);
    Q_INVOKABLE bool getAutoDownloadOnUpdate();

    //UI
    Q_INVOKABLE bool getAutoMarkAsRead();
    Q_INVOKABLE void setAutoMarkAsRead(bool value);

    // Netvibes Fetcher
    Q_INVOKABLE void setNetvibesUsername(const QString &value);
    Q_INVOKABLE QString getNetvibesUsername();
    Q_INVOKABLE void setNetvibesPassword(const QString &value);
    Q_INVOKABLE QString getNetvibesPassword();
    Q_INVOKABLE void setNetvibesDefaultDashboard(const QString &value);
    Q_INVOKABLE QString getNetvibesDefaultDashboard();
    Q_INVOKABLE void setNetvibesFeedLimit(int value);
    Q_INVOKABLE int getNetvibesFeedLimit();
    Q_INVOKABLE void setNetvibesFeedUpdateAtOnce(int value);
    Q_INVOKABLE int getNetvibesFeedUpdateAtOnce();
    Q_INVOKABLE void setNetvibesLastUpdateDate(int value);
    Q_INVOKABLE int getNetvibesLastUpdateDate();

    // Download Manger
    Q_INVOKABLE void setDmConnections(int value);
    Q_INVOKABLE int getDmConnections();
    Q_INVOKABLE void setDmTimeOut(int value);
    Q_INVOKABLE int getDmTimeOut();
    Q_INVOKABLE void setDmMaxSize(int value);
    Q_INVOKABLE int getDmMaxSize();
    //Q_INVOKABLE void setDmCacheDir(const QString &value);
    Q_INVOKABLE QString getDmCacheDir();
    Q_INVOKABLE void setDmUserAgent(const QString &value);
    Q_INVOKABLE QString getDmUserAgent();
    Q_INVOKABLE void setDmMaxCacheRetency(int value);
    Q_INVOKABLE int getDmMaxCacheRetency();
    Q_INVOKABLE void setDmCacheRetencyFeedLimit(int value);
    Q_INVOKABLE int getDmCacheRetencyFeedLimit();

    // Cache Server
    Q_INVOKABLE void setCsPost(int value);
    Q_INVOKABLE int getCsPort();
    Q_INVOKABLE QString getCsTheme();
    Q_INVOKABLE void setCsTheme(const QString &value);

signals:
    void offlineModeChanged();
    void showTabIconsChanged();
    void signedInChanged();
    void showStarredTabChanged();
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
