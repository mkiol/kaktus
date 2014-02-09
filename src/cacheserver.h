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
#include <QRegExp>
#include <QCryptographicHash>

#include "qhttpserver/qhttpserver.h"
#include "qhttpserver/qhttprequest.h"
#include "qhttpserver/qhttpresponse.h"

#include "settings.h"
#include "databasemanager.h"

class CacheServer : public QObject
{
    Q_OBJECT
public:
    explicit CacheServer(DatabaseManager* db, QObject *parent = 0);
    ~CacheServer();
    Q_INVOKABLE QString getUrl(const QString &item);

public slots:
    void handle(QHttpRequest *req, QHttpResponse *resp);

private:
    QHttpServer *server;
    QString cacheDir;
    int port;
    DatabaseManager *db;

    bool readFile(const QString &filename, QByteArray &data);
    QString hash(const QString &url);
    void filter(QString &content);
};

#endif // CACHESERVER_H
