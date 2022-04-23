/*
  Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>

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

#ifndef NVFETCHER_H
#define NVFETCHER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVariantMap>
#include <QNetworkRequest>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonArray>
#else
#include <QVariantList>
#endif

#include "fetcher.h"
#include "databasemanager.h"

class NvFetcher : public Fetcher
{
    Q_OBJECT
public:
    explicit NvFetcher(QObject *parent = 0);

    Q_INVOKABLE void getConnectUrl(int type);
    Q_INVOKABLE bool setConnectUrl(const QString &url);

protected:
    void run();

private Q_SLOTS:
    void finishedGetAuthUrl();
    void finishedSignIn();
    void finishedSignInOnlyCheck();
    void finishedDashboards();
    void finishedDashboards2();
    void finishedTabs();
    void finishedTabs2();
    void finishedFeeds();
    void finishedFeeds2();
    void finishedFeedsUpdate();
    void finishedFeedsUpdate2();
    void finishedFeedsReadlater();
    void finishedFeedsReadlater2();
    void finishedSetAction();

private:
    enum Job { Idle, StoreDashboards, StoreTabs, StoreFeeds,
               StoreFeedsInfo, StoreFeedsUpdate, StoreFeedsReadlater
             };

    static const int feedsAtOnce = 5;
    static const int limitFeeds = 25;
    static const int limitFeedsUpdate = 25;
    static const int limitFeedsReadlater = 25;
    static const int feedsUpdateAtOnce = 10;

    Job currentJob;
    QStringList dashboardList;
    QStringList tabList;
    QList<DatabaseManager::StreamModuleTab> streamList;
    QList<DatabaseManager::StreamModuleTab> streamUpdateList;
    QList<DatabaseManager::StreamModuleTab> storedStreamList;
    int publishedBeforeDate = 0;

    void signIn();
    void startFetching();
    void uploadActions();
    void fetchDashboards(const QString &url = "http://www.netvibes.com/privatepage/0");
    void fetchTabs();
    void fetchFeeds();
    void fetchFeedsUpdate();
    void fetchFeedsReadlater();
    void setAction();

    void startJob(Job job);

    void storeDashboards();
    void storeDashboardsByParsingHtml();
    void storeTabs();
    int storeFeeds();

    void setCookie(QNetworkRequest &request, const QString &cookie);
    bool checkCookie(const QString &cookie);
    bool checkError();
    void cleanNewFeeds();
    void cleanRemovedFeeds();
    void removeAction();
};

#endif // NVFETCHER_H
