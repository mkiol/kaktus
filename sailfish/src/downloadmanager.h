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

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QUrl>
#include <QMap>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QThread>

#include "databasemanager.h"
#include "settings.h"


class QSslError;

class CacheDeterminer : public QThread
{
    Q_OBJECT
public:
    CacheDeterminer(QObject * parent = 0);

protected:
    void run();

signals:
    void cacheDetermined(int size);
};

class DownloadAdder : public QThread
{
    Q_OBJECT
public:
    DownloadAdder(QObject * parent = 0);

protected:
    void run();

signals:
    void addDownload(DatabaseManager::CacheItem item);
    void addingFinished(int count);
};

class CacheRemover : public QThread
{
    Q_OBJECT
public:
    CacheRemover(QObject * parent = 0);
    void cancel();

protected:
    void run();

signals:
    void progressChanged(int current, int total);

private:
    bool removeDir(const QString &dirName);
    int total;
    int current;
    bool doCancel;
};

class CacheCleaner : public QThread
{
    Q_OBJECT

protected:
    void run();

private:
    static const int entriesLimit = 100;

    void cleanNv();
    void cleanOr();
};

class Checker: public QObject
{
    Q_OBJECT

public:
    Checker(QNetworkReply *reply);
    ~Checker();

public slots:
    void metaDataChanged();
    void timeout();

private:
    QNetworkReply *reply;
    int maxSize;
    int maxTime;
};

class DownloadManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY (bool online READ isOnline NOTIFY onlineChanged)
    Q_PROPERTY (bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY (int cacheSize READ getCacheSize NOTIFY cacheSizeChanged)
    Q_PROPERTY (bool removerBusy READ isRemoverBusy NOTIFY removerBusyChanged)

public:
    DownloadManager(QObject *parent = 0);
    ~DownloadManager();

    Q_INVOKABLE void cancel();
    Q_INVOKABLE void removerCancel();
    Q_INVOKABLE int itemsToDownloadCount();
    Q_INVOKABLE void startFeedDownload();
    Q_INVOKABLE void cleanCache();
    Q_INVOKABLE bool isWLANConnected();
    Q_INVOKABLE void onlineDownload(const QString& id, const QString& url);

    bool isBusy();
    bool isOnline();
    int getCacheSize();
    bool isRemoverBusy();

Q_SIGNALS:
    void cacheCleaned();
    void cacheSizeChanged();
    void busyChanged();
    void ready();
    void networkNotAccessible();
    void onlineChanged();
    void canceled();
    void removerBusyChanged();
    void removerProgressChanged(int current, int total);
    /*
    500 - Unknown error
    501 - Save to disk error
     */
    void error(int code);
    void progress(int current, int total);
    void onlineDownloadReady(QString id, QString url);
    void onlineDownloadFailed();

public Q_SLOTS:
    void addDownload(DatabaseManager::CacheItem item);
    void startDownload();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
    void error(QNetworkReply::NetworkError code);
    void networkAccessibleChanged (QNetworkAccessManager::NetworkAccessibility accessible);
    void removeCache();
    void onlineStateChanged(bool isOnline);
    void cacheCleaningFinished();
    void cacheRemoverFinished();
    void cacheRemoverProgressChanged(int current, int total);
    void cacheSizeDetermined(int size);
    void addingFinishedHandler(int count);

private:
    static const int entriesLimit = 200;
    static const int cacheRetencyFeedLimit = 20;
    static const int maxCacheRetency = 604800; // 1 week
    static const int minImageSize = 2000;

    QNetworkAccessManager manager;
    QList<DatabaseManager::CacheItem> queue;
    QList<QNetworkReply*> downloads;
    QMap<QNetworkReply*,DatabaseManager::CacheItem> replyToCachedItemMap;
    QMap<QNetworkReply*,Checker*> replyToCheckerMap;
    QNetworkConfigurationManager ncm;
    CacheCleaner cleaner;
    CacheRemover remover;
    DownloadAdder adder;
    CacheDeterminer cacheDeterminer;

    int lastCacheSize;
    int downloadTotal;
    bool cacheSizeFreshFlag;

    void doDownload(DatabaseManager::CacheItem item);
    bool saveToDisk(const QString &filename, const QByteArray &content);
    bool isUrlinQueue(const QString &origUrl, const QString &finalUrl);
    void scanHtml(const QByteArray &content, const QUrl &url);
    void addNextDownload();
    QString hash(const QString &url);
    bool checkIfHeadersAreValid(QNetworkReply *reply);
};


#endif // DOWNLOADMANAGER_H
