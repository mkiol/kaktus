/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslError>
#include <QThread>
#include <QUrl>

#include "databasemanager.h"
#include "settings.h"

class QSslError;

class CacheDeterminer : public QThread {
    Q_OBJECT
   public:
    CacheDeterminer(QObject *parent = nullptr);

   protected:
    void run();

   signals:
    void cacheDetermined(int size);
};

class DownloadAdder : public QThread {
    Q_OBJECT
   public:
    DownloadAdder(QObject *parent = nullptr);

   protected:
    void run();

   signals:
    void addDownload(DatabaseManager::CacheItem item);
    void addingFinished(int count);
};

class CacheRemover : public QThread {
    Q_OBJECT
   public:
    CacheRemover(QObject *parent = nullptr);
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

class CacheCleaner : public QThread {
    Q_OBJECT

   protected:
    void run();

   private:
    static const int entriesLimit = 100;

    void cleanNv();
    void cleanOr();
};

class Checker : public QObject {
    Q_OBJECT

   public:
    explicit Checker(QNetworkReply *reply);
    ~Checker();

   public slots:
    void metaDataChanged();
    void timeout();

   private:
    QNetworkReply *reply;
    int maxSize;
    int maxTime;
};

class DownloadManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool online READ isOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(int cacheSize READ getCacheSize NOTIFY cacheSizeChanged)
    Q_PROPERTY(bool removerBusy READ isRemoverBusy NOTIFY removerBusyChanged)

   public:
    static DownloadManager *instance(QObject *parent = nullptr);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void removerCancel();
    Q_INVOKABLE int itemsToDownloadCount() const;
    Q_INVOKABLE void startFeedDownload();
    Q_INVOKABLE void cleanCache();
    Q_INVOKABLE bool isWLANConnected() const;
    Q_INVOKABLE void onlineDownload(const QString &id, const QString &url);

    bool isBusy() const;
    bool isOnline() const;
    int getCacheSize();
    bool isRemoverBusy() const;

   signals:
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
    void onlineDownloadReady(const QString &id, const QString &url);
    void onlineDownloadFailed();
    void downloadReady(const QString &url, const QString &path,
                       const QString &contentType);
    void downloadFailed(const QString &url);

   public slots:
    void addDownload(DatabaseManager::CacheItem item);
    void startDownload();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
    void handleError(QNetworkReply::NetworkError code) const;
    void networkAccessibleChanged(
        QNetworkAccessManager::NetworkAccessibility accessible);
    void removeCache();
    void onlineStateChanged(bool isOnline);
    void cacheCleaningFinished();
    void cacheRemoverFinished();
    void cacheRemoverProgressChanged(int current, int total);
    void cacheSizeDetermined(int size);
    void addingFinishedHandler(int count);

   private:
    static DownloadManager *m_instance;
    static const int entriesLimit = 200;
    static const int cacheRetencyFeedLimit = 20;
    static const int maxCacheRetency = 604800;  // 1 week
    static const int minImageSize = 2000;

    QNetworkAccessManager m_manager;
    QList<DatabaseManager::CacheItem> m_queue;
    QList<QNetworkReply *> m_downloads;
    QMap<QNetworkReply *, DatabaseManager::CacheItem> m_replyToCachedItemMap;
    QMap<QNetworkReply *, Checker *> m_replyToCheckerMap;
    QNetworkConfigurationManager m_ncm;
    CacheCleaner m_cleaner;
    CacheRemover m_remover;
    DownloadAdder m_adder;
    CacheDeterminer m_cacheDeterminer;

    int m_lastCacheSize = 0;
    int m_downloadTotal = 0;
    bool m_cacheSizeFreshFlag = false;

    DownloadManager(QObject *parent = nullptr);
    void doDownload(DatabaseManager::CacheItem &&item);
    QString saveToDisk(const QString &filename,
                       const QByteArray &content) const;
    bool isUrlinQueue(const QString &origUrl, const QString &finalUrl);
    void scanHtml(const QByteArray &content, const QUrl &url);
    void addNextDownload();
    static bool checkIfHeadersAreValid(QNetworkReply *reply);
};

#endif  // DOWNLOADMANAGER_H
