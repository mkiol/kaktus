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

#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QTimer>
#include <QUrl>
#include <QDebug>
#include <QMap>
#include <QRegExp>
#include <QCryptographicHash>
#include <QTimer>
#include <QDirIterator>

#include "databasemanager.h"
#include "settings.h"


class QSslError;

class Checker: public QObject
{
    Q_OBJECT

public:
    Checker(QNetworkReply *reply);
    ~Checker();

public slots:
    void metaDataChanged();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void timeout();

private:
    QNetworkReply *reply;
    int maxSize;
    int maxTime;
};

class DownloadManager: public QObject
{
    Q_OBJECT

public:
    DownloadManager(DatabaseManager* db);
    void addDownload(DatabaseManager::CacheItem item);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE int itemsToDownloadCount();
    Q_INVOKABLE bool isBusy();

signals:
    void busy();
    void ready();
    void networkNotAccessible();
    void canceled();
    /*
    500 - Unknown error
    501 - Save to disk error
     */
    void error(int code);
    void progress(int remaining);

public slots:
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
    void error(QNetworkReply::NetworkError code);
    void networkAccessibleChanged (QNetworkAccessManager::NetworkAccessibility accessible);
    void startFeedDownload();
    void cleanCache();

private:
    QNetworkAccessManager manager;
    QList<DatabaseManager::CacheItem> queue;
    QList<QNetworkReply*> downloads;
    QMap<QNetworkReply*,DatabaseManager::CacheItem> replyToCachedItemMap;
    QMap<QNetworkReply*,Checker*> replyToCheckerMap;
    DatabaseManager *db;
    int connections;
    QString userAgent;
    QString cacheDir;

    void doDownload(DatabaseManager::CacheItem item);
    bool saveToDisk(const QString &filename, const QByteArray &content);
    bool isUrlinQueue(const QString &origUrl, const QString &finalUrl);
    void scanHtml(const QByteArray &content, const QUrl &url);
    void addNextDownload();
    QString hash(const QString &url);
    bool checkIfHeadersAreValid(QNetworkReply *reply);
};


#endif // DOWNLOADMANAGER_H
