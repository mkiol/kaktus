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

#include <QRegExp>
#include <QCryptographicHash>
#include <QTextCodec>
#include <QFile>
#include <QDebug>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QUrlQuery>
#endif

#include "cacheserver.h"


CacheServer::CacheServer(QObject *parent) :
    QObject(parent)
{
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
    //qDebug() << "handle, url=" << req->url().toString();

    ///@todo Need to rewrite this crappy code :-(

    Settings *s = Settings::instance();

    QString entryId = req->url().path();
    if (entryId.at(0) == '/')
        entryId = entryId.right(entryId.length()-1);

    DatabaseManager::CacheItem item = s->db->readCacheByEntry(entryId);

    QString filename;
    QByteArray data;

    if (item.id == "") {
        item = s->db->readCacheByFinalUrl(entryId);
        filename = entryId;
    } else {
        filename = item.finalUrl;
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
        QTextCodec *tc;
        QRegExp rx("charset=(\\S*)", Qt::CaseInsensitive);
        if (rx.indexIn(item.contentType)!=-1)
            tc = QTextCodec::codecForName(rx.cap(1).toUtf8());
        else
            tc = QTextCodec::codecForHtml(data);
        QString content = tc->toUnicode(data);

        QString image = "";
        if(item.entryId!="")
            image = s->db->readEntryImageById(item.entryId);

        filter(content,req->url(),image);
        data = tc->fromUnicode(content);
    }

    //qDebug() << data;

    resp->setHeader("Content-Length", QString::number(data.size()));
    //qDebug() << "Content-Type:"<<item.contentType;
    resp->setHeader("Content-Type", item.contentType);
    resp->writeHead(200);
    resp->end(data);
}

bool CacheServer::readFile(const QString &filename, QByteArray &data)
{
    Settings *s = Settings::instance();
    QString cacheDir = s->getDmCacheDir();

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

void CacheServer::filter(QString &content, const QUrl &query, const QString &image)
{
    //QRegExp rxImg("(<img\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxImgAll("<img[^>]*>", Qt::CaseInsensitive);
    QRegExp rxBody("(<body[^>]*>)", Qt::CaseInsensitive);
    //QRegExp rxImgUrl(image, Qt::CaseInsensitive);
    QRegExp rxLinkAll("<link[^>]*>", Qt::CaseInsensitive);
    QRegExp rxFormAll("<form[^>]*>((?!<\\/form>).)*<\\/form>", Qt::CaseInsensitive);
    QRegExp rxInputAll("<input[^>]*>", Qt::CaseInsensitive);
    //QRegExp rxMetaAll("<meta[^>]*>", Qt::CaseInsensitive);
    QRegExp rxMetaViewport("<meta\\s[^>]*name\\s*=(\"viewport\"|'viewport')[^>]*>", Qt::CaseInsensitive);
    //QRegExp rxCss("(<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxScript("(<script\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxFrame("(<iframe\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxA("(<a\\s[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxUrl("url[\\s]*\\([^\\)]*\\)", Qt::CaseInsensitive);
    QRegExp rxScriptAll("<script[^>]*>((?!<\\/script>).)*<\\/script>", Qt::CaseInsensitive);
    QRegExp rxStyleAll("<style[^>]*>((?!<\\/style>).)*<\\/style>", Qt::CaseInsensitive);
    QRegExp rxStyle("\\s*style\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxClass("\\s*class\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxWidth("\\s*width\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxHeight("\\s*height\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxHeadEnd("</head>", Qt::CaseInsensitive);

    content = content.replace(rxImgAll,"");
    content = content.replace(rxLinkAll,"");
    content = content.replace(rxScript,"\\1");
    content = content.replace(rxScriptAll,"");
    content = content.replace(rxFormAll,"");
    content = content.replace(rxInputAll,"");
    //content = content.replace(rxMetaAll,"");
    content = content.replace(rxMetaViewport,"");
    content = content.replace(rxStyle,"");
    content = content.replace(rxClass,"");
    content = content.replace(rxWidth,"");
    content = content.replace(rxHeight,"");
    content = content.replace(rxStyleAll,"");
    content = content.replace(rxFrame,"\\1");
    content = content.replace(rxA,"\\1");
    content = content.replace(rxUrl,"http://0.0.0.0");
    //content = content.replace(rxCss,"\\1");

    // Applying Theme's style
    Settings *s = Settings::instance();
    QString style, width, fontsize;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QUrlQuery urlQuery(query);
    if (urlQuery.hasQueryItem("width"))
        width = urlQuery.queryItemValue("width");
    if (urlQuery.hasQueryItem("fontsize"))
        fontsize = urlQuery.queryItemValue("fontsize");
#else
    if (query.hasQueryItem("width"))
        width = query.queryItemValue("width");
    if (query.hasQueryItem("fontsize"))
        fontsize= query.queryItemValue("fontsize");
#endif

    /*if (s->getOfflineTheme() == "white") {
        style = QString("<meta name='viewport' content='width=%1'>"
                        "<style>body{background:#FFF;color:#000;font-size:%2;}img{max-width:%3;}</style></head>")
                .arg(width).arg(fontsize).arg(width);
    }

    if (s->getOfflineTheme() == "black") {
        style = QString("<meta name='viewport' content='width=%1'>"
                "<style>body{background:#000;color:#FFF;font-size:%2;}img{max-width:%3;}</style></head>")
                .arg(width).arg(fontsize).arg(width);
    }*/

    if (s->getOfflineTheme() == "white") {
            style = QString("<meta name='viewport' content='device-width'>"
                            "<style>body{background:#FFF;color:#000;font-size:%1;}img{max-width:%2;}</style></head>")
                    .arg(fontsize).arg(width);
        }

        if (s->getOfflineTheme() == "black") {
            style = QString("<meta name='viewport' content='device-width'>"
                            "<style>body{background:#000;color:#FFF;font-size:%1;}img{max-width:%2;}</style></head>")
                    .arg(fontsize).arg(width);
        }

    // Inserting image after <body> tag
    if (image!="")
        content = content.replace(rxBody,QString("\\1<img src=\"%1\"/>").arg(getUrlbyUrl(image)));

    content = content.replace(rxHeadEnd,style);

    //qDebug() << content;

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

QString CacheServer::getUrlbyId(const QString &item)
{
    return "http://127.0.0.1:" + QString::number(port) + "/" + item;
}

QString CacheServer::getUrlbyUrl(const QString &url)
{
    return "http://127.0.0.1:" + QString::number(port) + "/" + hash(url);
}
