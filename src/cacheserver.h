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
    static CacheServer* instance(QObject *parent = nullptr);
    static bool readFile(const QString &filename, QByteArray &data);
    static bool readFile2(const QString &path, QByteArray &data);
    static QString getFileUrl(const QString &id);
    static QByteArray getDataUrlByUrl(const QString &item);
    static bool getPathAndContentTypeByUrl(const QString &url, QString &path,
                                           QString &contentType);

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
    static CacheServer* m_instance;
    static const int port = 9999;
    explicit CacheServer(QObject *parent = nullptr);

    QHttpServer *server;

    //QString hash(const QString &url);
    //void resolveRelativeUrls(QString &content, const QRegExp &rx, const QUrl &baseUrl, int filter);
};

#endif // CACHESERVER_H
