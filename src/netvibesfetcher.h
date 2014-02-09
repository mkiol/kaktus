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

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QUrl>
#include <QDebug>
#include <QByteArray>
#include <QModelIndex>
#include <QDateTime>
#include <QRegExp>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "databasemanager.h"
#include "downloadmanager.h"


class NetvibesFetcher: public QObject
{
    Q_OBJECT

public:
    explicit NetvibesFetcher(DatabaseManager* db, QObject *parent = 0);
    Q_INVOKABLE void init();
    Q_INVOKABLE void update();
    Q_INVOKABLE void checkCredentials();
    Q_INVOKABLE void updateFeeds();
    Q_INVOKABLE void updateTab(const QString &tabId);

signals:
    void quit();
    void busy();
    void progress(int current, int total);
    void ready();
    void initiating();
    void updating();
    void uploading();
    void checkingCredentials();
    void credentialsValid();

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
    void error(int code);
    void errorCheckingCredentials(int code);

public slots:
    void finishedSignIn();
    void finishedSignInOnlyCheck();
    void finishedDashboards();
    void finishedTabs();
    void finishedFeeds();
    void finishedFeedsInfo();
    void finishedFeedsUpdate();

    void finishedSet();

    void readyRead();
    void networkError(QNetworkReply::NetworkError);

private:
    static const int feedsAtOnce = 5;
    QNetworkAccessManager _manager;
    QNetworkReply* _currentReply;
    QByteArray _data;
    QJsonObject _jsonObj;
    bool _busy;
    bool _busyType; // true = init, false = update
    QStringList _dashboardList;
    QStringList _feedList;
    QMap<QString,int> _feedUpdateList;
    QMap<QString,QString> _feedTabList;
    QStringList _tabList;
    QList<DatabaseManager::Action> actionsList;
    int _total;
    QByteArray _cookie;
    DatabaseManager* _db;

    bool parse();

    void storeTabs(const QString &dashboardId);
    void storeFeeds();
    void storeDashboards();
    void storeEntries();
    void signIn(bool onlyCheck = false);
    void fetchDashboards();
    void fetchTabs(const QString &dashboardId);
    void fetchFeeds();
    void fetchFeedsInfo(const QString &tabId);
    void fetchFeedsUpdate();

    void uploadActions();
    void set(const QString &entryId, DatabaseManager::ActionsTypes type);

    void cleanNewFeeds();
    void cleanRemovedFeeds();
    void taskEnd();
    void downloadFeeds();
};

#endif // NETVIBESFETCHER_H
