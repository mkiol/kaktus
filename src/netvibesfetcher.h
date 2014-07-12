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

#ifndef NETVIBESFETCHER_H
#define NETVIBESFETCHER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QNetworkConfigurationManager>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonObject>
#else
#include <QVariantMap>
#endif

#include "databasemanager.h"
#include "downloadmanager.h"

class NetvibesFetcher: public QObject
{
    Q_OBJECT
    Q_ENUMS(BusyType)
    Q_PROPERTY (bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY (BusyType busyType READ readBusyType NOTIFY busyChanged)

public:
    enum BusyType {
        Unknown = 0,
        Initiating = 1,
        Updating = 2,
        CheckingCredentials = 3,
        InitiatingWaiting = 11,
        UpdatingWaiting = 21,
        CheckingCredentialsWaiting = 31
    };

    explicit NetvibesFetcher(QObject *parent = 0);

    Q_INVOKABLE bool init();
    Q_INVOKABLE bool update();
    Q_INVOKABLE bool checkCredentials();
    Q_INVOKABLE void cancel();

    BusyType readBusyType();
    bool isBusy();

signals:
    void quit();
    void busyChanged();
    void progress(int current, int total);
    void networkNotAccessible();
    void uploading();
    void checkingCredentials();

    /*
    200 - Fether is busy
    400 - Email/password is not defined
    401 - SignIn failed
    402 - SignIn user/password do no match
    500 - Network error
    501 - SignIn resposne is null
    600 - Error while parsing XML
    601 - Unknown XML response
     */
    void credentialsValid();
    void errorCheckingCredentials(int code);
    void error(int code);
    void canceled();
    void ready();

public slots:
    void finishedSignIn();
    void finishedSignInOnlyCheck();
    void finishedDashboards();
    void finishedTabs();
    void finishedFeeds();
    void finishedFeedsInfo();
    void finishedFeedsUpdate();
    void finishedFeedsReadlater();

    void finishedSet();

    void readyRead();
    void networkError(QNetworkReply::NetworkError);
    void networkAccessibleChanged (QNetworkAccessManager::NetworkAccessibility accessible);

    bool delayedUpdate(bool state);

private:
    static const int feedsAtOnce = 5;
    static const int limitFeeds = 50;
    static const int limitFeedsReadlater = 50;
    static const int feedsUpdateAtOnce = 10;

    QNetworkAccessManager _manager;
    QNetworkReply* _currentReply;
    QByteArray _data;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonObject _jsonObj;
#else
    QVariantMap _jsonObj;
#endif

    bool _busy;
    BusyType _busyType;
    QStringList _dashboardList;
    //QStringList _feedList;
    QMap<QString,QString> _feedList;
    QMap<QString,int> _feedUpdateList;
    QMap<QString,QString> _feedTabList;
    QStringList _tabList;
    QList<DatabaseManager::Action> actionsList;
    int _total;
    QByteArray _cookie;
    QNetworkConfigurationManager ncm;
    int offset;

    bool parse();

    QString hash(const QString &url);

    void storeTabs(const QString &dashboardId);
    void storeFeeds();
    void storeDashboards();
    void storeEntries();
    bool storeEntriesMerged(); // returns true if has more
    void signIn();
    void fetchDashboards();
    void fetchTabs(const QString &dashboardId);
    void fetchFeeds();
    void fetchFeedsInfo(const QString &tabId);
    void fetchFeedsUpdate();
    void fetchFeedsReadlater();

    void uploadActions();
    void set();

    void cleanNewFeeds();
    void cleanRemovedFeeds();
    void taskEnd();
    void downloadFeeds();

    void setBusy(bool busy, BusyType type = Unknown);
};

#endif // NETVIBESFETCHER_H
