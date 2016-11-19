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

#ifndef CACHESERVER_H
#define CACHESERVER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QThread>
#include <QUrl>

#include "qhttpserver/qhttpserver.h"
#include "qhttpserver/qhttprequest.h"
#include "qhttpserver/qhttpresponse.h"

#include "databasemanager.h"
#include "settings.h"

class FilteringWorker : public QThread
{
    Q_OBJECT
public:
    explicit FilteringWorker(QObject *parent = 0);
    QHttpResponse *resp;
    QHttpRequest *req;
    QByteArray data;
    bool error;

protected:
    void run();

public slots:
    void start(QHttpRequest *req, QHttpResponse *resp);

private:
    QString content;
    DatabaseManager::CacheItem item;

    void filter();
    bool filterArticle();
    //void advancedFilter();
    void filterOffline();
    void filterOnline();
    void removeUrls(const QRegExp &rx);
    void resolveRelativeUrls(const QRegExp &rx);
    //bool readFile(const QString &filename);
};

class CacheServer : public QObject
{
    Q_OBJECT
public:
    static bool readFile(const QString &filename, QByteArray &data);
    static QString getFileUrl(const QString &id);
    static QByteArray getDataUrlByUrl(const QString &item);

    explicit CacheServer(QObject *parent = 0);
    ~CacheServer();
    Q_INVOKABLE QString getUrlbyId(const QString &item);
    Q_INVOKABLE QString getUrlbyUrl(const QString &item);
    Q_INVOKABLE QString getCacheUrlbyUrl(const QString &item);

public slots:
    void handle(QHttpRequest *req, QHttpResponse *resp);
    void handleFinish();

signals:
    //void startWorker(QThread::Priority priority);
    void startWorker(QHttpRequest*, QHttpResponse*);

private:
    static const int port = 9999;

    QHttpServer *server;

    //QString hash(const QString &url);
    //void resolveRelativeUrls(QString &content, const QRegExp &rx, const QUrl &baseUrl, int filter);
};

#endif // CACHESERVER_H
