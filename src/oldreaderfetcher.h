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

#ifndef OLDREADERFETCHER_H
#define OLDREADERFETCHER_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonArray>
#else
#include <QVariantList>
#endif

#include "fetcher.h"

class OldReaderFetcher : public Fetcher
{
    Q_OBJECT
public:
    explicit OldReaderFetcher(QObject *parent = 0);

    Q_INVOKABLE void getConnectUrl(int type);
    Q_INVOKABLE bool setConnectUrl(const QString &url);

protected:
    void run();

private Q_SLOTS:
    void finishedSignIn();
    void finishedSignInOnlyCheck();
    void finishedTabs();
    void finishedTabs2();
    void finishedFriends();
    void finishedFriends2();
    void finishedFeeds();
    void finishedFeeds2();
    void finishedStream();
    void finishedStream2();
    void finishedStarredStream();
    void finishedStarredStream2();
    void finishedLikedStream();
    void finishedLikedStream2();
    void finishedBroadcastStream();
    void finishedBroadcastStream2();
    void finishedUnreadStream();
    void finishedUnreadStream2();
    void finishedSetAction();
    void finishedMarkSlow();

private:
    enum Job { Idle, StoreTabs, StoreFriends, StoreFeeds, StoreStream,
               StoreUnreadStream, StoreStarredStream, StoreLikedStream,
               StoreBroadcastStream, MarkSlow };

    static const int limitAtOnce = 400;
    static const int continuationLimit = 100;

    Job currentJob;
    QStringList tabList;
    //QList<DatabaseManager::StreamModuleTab> feedList;
    //QList<DatabaseManager::StreamModuleTab> storedFeedList;

    QString lastContinuation;
    int continuationCount;
    int lastDate;

    void signIn();
    void startFetching();

    void uploadActions();

    //void removeDeletedFeeds();

    void fetchTabs();
    void fetchFriends();
    void fetchFeeds();
    void fetchStream();
    void fetchUnreadStream();
    void fetchStarredStream();
    void fetchLikedStream();
    void fetchBroadcastStream();
    void setAction();

    void startJob(Job job);

    void storeTabs();
    void storeFriends();
    void storeFeeds();
    void storeStream();
    void markSlowFeeds();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    void getFolderFromCategories(const QJsonArray &categories, QString &tabId, QString &tabName);
    void getFromCategories(const QJsonArray &categories, QVariantMap &result);
#else
    void getFolderFromCategories(const QVariantList &categories, QString &tabId, QString &tabName);
    void getFromCategories(const QVariantList &categories, QVariantMap &result);
#endif

    QString getIdsFromActionString(const QString &actionString);
};

#endif // OLDREADERFETCHER_H
