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

#ifndef FEEDLYFETCHER_H
#define FEEDLYFETCHER_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonArray>
#else
#include <QVariantList>
#endif

#include "fetcher.h"

class FeedlyFetcher : public Fetcher
{
    Q_OBJECT
public:
    explicit FeedlyFetcher(QObject *parent = 0);

    Q_INVOKABLE void getConnectUrl(int type);
    Q_INVOKABLE bool setConnectUrl(const QString &url);

protected:
    void run();

private Q_SLOTS:
    void finishedRefreshToken();
    void finishedProfile();
    void finishedProfile2();
    void finishedSignIn();
    void finishedSignInOnlyCheck();
    void finishedTabs();
    void finishedTabs2();
    void finishedFeeds();
    void finishedFeeds2();
    void finishedStream();
    void finishedStream2();
    void finishedStarredStream();
    void finishedStarredStream2();
    void finishedMustStream();
    void finishedMustStream2();
    void finishedSetAction();
    void finishedMarkSlow();

private:
    enum Job { Idle, StoreProfile, StoreTabs, StoreFeeds, StoreStream,
               StoreStarredStream, StoreMustStream, MarkSlow };

    static const int limitAtOnce = 200;
    static const int continuationLimit = 100;

    Job currentJob;
    QStringList tabList;
    QString lastContinuation;
    int continuationCount;
    int lastDate;
    bool refreshTokenDone;

    void signIn();
    void startFetching();
    void uploadActions();

    void refreshToken();
    void fetchProfile();
    void fetchTabs();
    void fetchFeeds();
    void fetchStream();
    void fetchUnreadStream();
    void fetchStarredStream();
    void fetchMustStream();
    void setAction();

    void startJob(Job job);

    void storeProfile();
    void storeTabs();
    void storeFeeds();
    void storeStream();
    void markSlowFeeds();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    void getFolderFromCategories(const QJsonArray &categories, QString &tabId, QString &tabName);
    //void getFromTags(const QJsonArray &tags, QVariantMap &result);
    bool getSavedFromTags(const QJsonArray &tags);
#else
    void getFolderFromCategories(const QVariantList &categories, QString &tabId, QString &tabName);
    //void getFromTags(const QVariantList &tags, QVariantMap &result);
    bool getSavedFromTags(const QVariantList &tags);
#endif
    QString getIdsFromActionString(const QString &actionString);
};

#endif // FEEDLYFETCHER_H
