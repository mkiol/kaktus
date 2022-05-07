/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CACHESERVER_H
#define CACHESERVER_H

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QThread>
#include <QUrl>

#include "databasemanager.h"
#include "qhttpserver/qhttprequest.h"
#include "qhttpserver/qhttpresponse.h"
#include "qhttpserver/qhttpserver.h"
#include "settings.h"
#include "singleton.h"

class FilteringWorker : public QThread {
    Q_OBJECT
public:
    explicit FilteringWorker(QObject *parent = nullptr);
    QHttpResponse *resp = nullptr;
    QHttpRequest *req = nullptr;
    QByteArray data;
    bool error = false;

protected:
    void run();

public slots:
    void start(QHttpRequest *req, QHttpResponse *resp);

private:
    QString content;
    DatabaseManager::CacheItem item;

    void filter();
    bool filterArticle();
    void filterOffline();
    void filterOnline();
    void removeUrls(const QRegExp &rx);
    void resolveRelativeUrls(const QRegExp &rx);
};

class CacheServer : public QObject, public Singleton<CacheServer> {
    Q_OBJECT
public:
    static bool readFile(const QString &filename, QByteArray &data);
    static bool readFile2(const QString &path, QByteArray &data);
    static QString getFileUrl(const QString &id);
    static QByteArray getDataUrlByUrl(const QString &item);
    static bool getPathAndContentTypeByUrl(const QString &url, QString &path,
                                           QString &contentType);
    explicit CacheServer(QObject *parent = nullptr);

    Q_INVOKABLE QString getUrlbyId(const QString &item);
    Q_INVOKABLE QString getUrlbyUrl(const QString &item);
    Q_INVOKABLE QString getCacheUrlbyUrl(const QString &item);
    Q_INVOKABLE QString getPathByUrl(const QString &url);

public slots:
    void handle(QHttpRequest *req, QHttpResponse *resp);
    void handleFinish();

signals:
    void startWorker(QHttpRequest*, QHttpResponse*);

private:
    static const int port = 9999;

    QHttpServer *server = nullptr;
};

#endif // CACHESERVER_H
