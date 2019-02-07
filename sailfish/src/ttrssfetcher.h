/*
  Copyright (C) 2019 Michal Kosciesza <michal@mkiol.net>

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

#ifndef TTRSSFETCHER_H
#define TTRSSFETCHER_H

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

class TTRssFetcher : public Fetcher
{
    Q_OBJECT

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    typedef void (TTRssFetcher::*ReplyCallback)();
#else
    typedef QString ReplyCallback;
#endif

    typedef void (TTRssFetcher::*ChainCommand)();

    enum Job {
        Idle,
        StoreCategories,
        StoreFeeds,
        StoreStream
    };

    enum SpecialFeed {
        AllArticles = -4,
        Fresh = -3,
        Published = -2,
        Starred = -1
    };

public:
    explicit TTRssFetcher(QObject *parent = 0);
    virtual ~TTRssFetcher();

    Q_INVOKABLE virtual void getConnectUrl(int) {}
    Q_INVOKABLE virtual bool setConnectUrl(const QString &) { return true; }

protected:
    void run();

private Q_SLOTS:
    void finishedSignIn();
    void finishedConfig();
    void finishedCategories();
    void finishedFeeds();
    void finishedStream();
    void finishedStream2();
    void finishedSetAction();

    void callNextCmd();

private:
    virtual void signIn();
    virtual void startFetching();
    virtual void uploadActions();

    void fetchCategories();
    void fetchFeeds();
    void fetchStream();
    void fetchStarredStream();
    void fetchPublishedStream();
    void pruneOld();
    void setAction();

    void startJob(Job job);

    void storeCategories();
    void storeFeeds();
    void storeStream();

    QString mergeEntryIds(const QList<DatabaseManager::Entry>& entries, bool read);

    void getHeadlines(int feedId, bool getContent, bool unreadOnly, int offset, ReplyCallback callback);

    void sendApiCall(const QString& op, ReplyCallback callback);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    void sendApiCall(const QString& op, const QJsonObject& params, ReplyCallback callback);
#else
    void sendApiCall(const QString& op, const QString& params, ReplyCallback callback);
#endif

    bool processResponse();

private:
    static const int streamLimit = 100;

    QString sessionId;
    int apiLevel;
    QString iconsUrl;

    QList<int> processedActionList;
    QList<ChainCommand> commandList;
    ChainCommand currentCommand;
    Job currentJob;
    int lastDate;
    int lastCount;
    int offset;
};

#endif // TTRSSFETCHER_H
