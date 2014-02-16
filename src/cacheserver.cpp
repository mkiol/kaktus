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

#include "cacheserver.h"

CacheServer::CacheServer(DatabaseManager *db, QObject *parent) :
    QObject(parent)
{
    this->db = db;

    Settings *s = Settings::instance();
    cacheDir = s->getDmCacheDir();
    port = s->getCsPort();

    server = new QHttpServer;
    connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handle(QHttpRequest*, QHttpResponse*)));
    server->listen(port);
}

CacheServer::~CacheServer()
{
    delete server;
}

void CacheServer::handle(QHttpRequest *req, QHttpResponse *resp)
{
    qDebug() << "handle, url=" << req->url().toString();

    QString entryId = req->url().path();
    if (entryId.at(0) == '/')
        entryId = entryId.right(entryId.length()-1);

    DatabaseManager::CacheItem item = db->readCacheItemFromEntryId(entryId);

    QString filename;
    QByteArray data;

    if (item.id == "") {
        item = db->readCacheItem(entryId);
        filename = entryId;
    } else {
        filename = item.id;
    }

    if (!readFile(filename, data)) {
        resp->setHeader("Content-Length", "0");
        resp->setHeader("Connection", "close");
        resp->writeHead(404);
        resp->end("");
        return;
    }

    if (item.type == "text") {
        // Converting charset
        QTextCodec *tc = QTextCodec::codecForHtml(data);
        QString content = tc->toUnicode(data);

        filter(content);
        data = tc->fromUnicode(content);
    }

    resp->setHeader("Content-Length", QString::number(data.size()));
    resp->setHeader("Content-Type", item.contentType);
    resp->writeHead(200);
    resp->end(data);
}

bool CacheServer::readFile(const QString &filename, QByteArray &data)
{
    QFile file(cacheDir + "/" + filename);

    if (!QFile::exists(cacheDir + "/" + filename)) {
        qWarning() << "File " << filename << "does not exists!";
        file.close();
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open" << filename << "for reading: " << file.errorString();
        file.close();
        return false;
    }

    data.append(file.readAll());
    file.close();

    return true;
}

void CacheServer::filter(QString &content)
{
    QRegExp rxImg("(<img\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    //QRegExp rxCss("<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxCss("(<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxScript("(<script\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxFrame("(<iframe\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxA("(<a\\s[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxUrl("url[\\s]*\\([^\\)]*\\)", Qt::CaseInsensitive);
    QRegExp rxScriptAll("<script[^>]*>((?!<\\/script>).)*<\\/script>", Qt::CaseInsensitive);

    content = content.replace(rxImg,"\\1");
    content = content.replace(rxScript,"\\1");
    content = content.replace(rxScriptAll,"");
    content = content.replace(rxFrame,"\\1");
    content = content.replace(rxA,"\\1");
    content = content.replace(rxUrl,"http://0.0.0.0");
    content = content.replace(rxCss,"\\1");

    // Change CSS link
    /*
    int i = 1, pos = 0;
    QList<QString> caps;
    while ((pos = rxCss.indexIn(content, pos)) != -1) {
        QString cap = rxCss.cap(2); cap = cap.mid(1,cap.length()-2);
        caps.append(cap);
        pos += rxCss.matchedLength();
        ++i;
    }

    QList<QString>::iterator it = caps.begin();
    while (it != caps.end()) {
        DatabaseManager::CacheItem item = db->readCacheItemFromOrigUrl(hash(*it));
        //qDebug() << "filter, *it=" << *it << "item.id=" << item.id;
        if (item.id != "")
            content = content.replace(*it, item.id);
        else
            content = content.replace(*it, "http://0.0.0.0");
        ++it;
    }
    */
}

QString CacheServer::hash(const QString &url)
{
    QByteArray data; data.append(url);
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
}

QString CacheServer::getUrl(const QString &item)
{
    return "http://127.0.0.1:" + QString::number(port) + "/" + item;
}
