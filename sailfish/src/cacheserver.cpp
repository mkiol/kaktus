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
#include "utils.h"

FilteringWorker::FilteringWorker(QObject *parent) :
    QThread(parent), error(false)
{
}

void FilteringWorker::start(QHttpRequest *req, QHttpResponse *resp)
{
    sender()->disconnect(this);
    this->req = req;
    this->resp = resp;
    QThread::start(QThread::LowPriority);
}

void FilteringWorker::run()
{
    Settings *s = Settings::instance();

    QString entryId = req->url().path();
    if (entryId.at(0) == '/')
        entryId = entryId.right(entryId.length()-1);
    item = s->db->readCacheByEntry(entryId);
    //qDebug() << "baseUrl3" << item.baseUrl;
    //qDebug() << "finalUrl3" << item.finalUrl;

    QString filename;
    if (item.id == "") {
        item = s->db->readCacheByFinalUrl(entryId);
        //qDebug() << "baseUrl4" << item.baseUrl;
        //qDebug() << "finalUrl4" << item.finalUrl;
        filename = entryId;
    } else {
        filename = item.finalUrl;
    }

    if (!CacheServer::readFile(filename, data)) {
        error = true;
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
        content = tc->toUnicode(data);

        Settings *s = Settings::instance();
        if (s->getOfflineMode()) {
            filterOffline();
        } else {
            filterOnline();
        }
        data = tc->fromUnicode(content);
    }

    resp->setHeader("Content-Type", item.contentType);
    error = false;
}

void FilteringWorker::resolveRelativeUrls(const QRegExp &rx)
{
    int i = 1, pos = 0;
    QStringList caps;
    while ((pos = rx.indexIn(content, pos)) != -1) {
        QString cap = rx.cap(2); cap = cap.mid(1,cap.length()-2);
        if (cap != "" && cap!= "/" && cap.at(0) != QChar('#'))
            caps.append(cap);
        pos += rx.matchedLength();
        ++i;
    }

    QStringList::iterator it = caps.begin();
    while (it != caps.end()) {
        QString cap = *it;
        QUrl capUrl(cap);
        QUrl baseUrl(item.baseUrl);
        // (capUrl.scheme()=="http" || capUrl.scheme()=="https") &&
        //qDebug() << "baseUrl:" << baseUrl << "cap:" << cap;
        if (capUrl.isRelative()) {
            content.replace("'"+cap+"'", "'"+baseUrl.resolved(capUrl).toString()+"'");
            content.replace("\""+cap+"\"", "\""+baseUrl.resolved(capUrl).toString()+"\"");
            //qDebug() << "cap is relative" << baseUrl.resolved(capUrl).toString();
        }
        ++it;
    }
}

void FilteringWorker::removeUrls(const QRegExp &rx)
{
    int i = 1, pos = 0;
    QStringList caps;
    while ((pos = rx.indexIn(content, pos)) != -1) {
        QString cap = rx.cap(2); cap = cap.mid(1,cap.length()-2);
        if (cap != "" && cap!= "/" && cap.at(0) != QChar('#'))
            caps.append(cap);
        pos += rx.matchedLength();
        ++i;
    }

    QStringList::iterator it = caps.begin();
    while (it != caps.end()) {
        QString cap = *it;
        QUrl capUrl(cap);
        if (capUrl.scheme()=="data") {
            content.replace(cap, "");
        }
        ++it;
    }
}

bool FilteringWorker::filterArticle()
{
    QRegExp rx1("<article[^>]*>((?!<\\/article>).)*<\\/article>", Qt::CaseInsensitive);

    int i = 1, pos = 0;
    QStringList articles;
    while ((pos = rx1.indexIn(content, pos)) != -1) {
        if (i>1)
            return false;
        QString cap = rx1.cap(0);
        if (cap != "") {
            //qDebug() << "article!";
            articles.append(cap);
        }
        pos += rx1.matchedLength();
        ++i;
    }

    if (articles.isEmpty()) {
        return false;
    }

    QString newContent = "<html><head>";
    i = 1, pos = 0;
    QRegExp rx2("<meta\\s[^>]*http-equiv=\\s*[^>]*>", Qt::CaseInsensitive);
    while ((pos = rx2.indexIn(content, pos)) != -1) {
        QString cap = rx2.cap(0);
        if (cap != "") {
            //qDebug() << "meta: " << cap;
            newContent += cap;
        }
        pos += rx2.matchedLength();
        ++i;
    }

    newContent += "</head><body>";
    QStringList::iterator it = articles.begin();
    while (it != articles.end()) {
        QString cap = *it;
        newContent += cap;
        ++it;
    }
    newContent += "</body></html>";

    content = newContent;

    return true;
}

void FilteringWorker::filter()
{
    QRegExp rxLinkAll("<link[^>]*>", Qt::CaseInsensitive);
    QRegExp rxScriptAll("<script[^>]*>((?!<\\/script>).)*<\\/script>", Qt::CaseInsensitive);
    QRegExp rxStyleAll("<style[^>]*>((?!<\\/style>).)*<\\/style>", Qt::CaseInsensitive);
    QRegExp rxStyle("\\s*style\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxClass("\\s*class\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxWidth("\\s*width\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxHeight("\\s*height\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxMetaViewport("<meta\\s[^>]*name\\s*=(\"viewport\"|'viewport')[^>]*>", Qt::CaseInsensitive);
    QRegExp rxInputAll("<input[^>]*>", Qt::CaseInsensitive);
    QRegExp rxTextareaAll("<textarea[^>]*>((?!<\\/textarea>).)*<\\/textarea>", Qt::CaseInsensitive);
    QRegExp rxObjectAll("<object[^>]*>((?!<\\/object>).)*<\\/object>", Qt::CaseInsensitive);
    QRegExp rxButtonAll("<button[^>]*>((?!<\\/button>).)*<\\/button>", Qt::CaseInsensitive);
    QRegExp rxNoscriptAll("<noscript[^>]*>((?!<\\/noscript>).)*<\\/noscript>", Qt::CaseInsensitive);
    QRegExp rxSelectAll("<select[^>]*>((?!<\\/select>).)*<\\/select>", Qt::CaseInsensitive);
    QRegExp rxNavAll("<nav[^>]*>((?!<\\/nav>).)*<\\/nav>", Qt::CaseInsensitive);

    content.remove(rxLinkAll);
    content.remove(rxScriptAll);
    content.remove(rxStyle);
    content.remove(rxClass);
    content.remove(rxWidth);
    content.remove(rxHeight);
    content.remove(rxStyleAll);
    content.remove(rxMetaViewport);
    content.remove(rxInputAll);
    content.remove(rxTextareaAll);
    content.remove(rxObjectAll);
    content.remove(rxButtonAll);
    content.remove(rxNoscriptAll);
    content.remove(rxSelectAll);
    content.remove(rxNavAll);
}

void FilteringWorker::filterOnline()
{
    filter();

    //QRegExp rxCss("<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    //QRegExp rxImg("(<img\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    //resolveRelativeUrls(rxImg);
    //QRegExp rxA("(<a\\s[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    //resolveRelativeUrls(rxA);
}

void FilteringWorker::filterOffline()
{
    filter();

    QRegExp rxUrl("url[\\s]*\\([^\\)]*\\)", Qt::CaseInsensitive);
    QRegExp rxImgAll("<img[^>]*>", Qt::CaseInsensitive);
    QRegExp rxFrame("(<iframe\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxBody("(<body[^>]*>)", Qt::CaseInsensitive);

    content.replace(rxUrl,"http://0.0.0.0");
    content.replace(rxImgAll,""); content.remove("</img>", Qt::CaseInsensitive);
    content.replace(rxFrame,"\\1");

    // Inserting image after <body> tag
    Settings *s = Settings::instance();
    QString image = "";
    if(item.entryId!="")
        image = s->db->readEntryImageById(item.entryId);
    if (!image.isEmpty()) {
        image = QString(CacheServer::getDataUrlByUrl(image));
        if (!image.isEmpty()) {
            content.replace(rxBody,QString("\\1<img id='_kaktus_img' src='%1'/>")
                            .arg(image));
        }
    }
}

/*bool FilteringWorker::readFile(const QString &filename)
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
}*/

// --------------

CacheServer::CacheServer(QObject *parent) :
    QObject(parent)
{
    server = new QHttpServer;

    QObject::connect(server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(handle(QHttpRequest*, QHttpResponse*)));

    if (!server->listen(port)) {
        qWarning() << "Cache server at localhost failed to start on" << this->port << "port!";
    }
}

CacheServer::~CacheServer()
{
    delete server;
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

bool CacheServer::readFile2(const QString &path, QByteArray &data)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open" << path << "for reading: " << file.errorString();
        file.close();
        return false;
    }

    data.append(file.readAll());
    file.close();

    return true;
}

/*QByteArray CacheServer::getData(const QString &id)
{
    qDebug() << "getData, id=" << id;

    Settings *s = Settings::instance();

    DatabaseManager::CacheItem item = s->db->readCacheByEntry(id);

    QString filename;
    if (item.id == "") {
        item = s->db->readCacheByFinalUrl(id);
        filename = id;
    } else {
        filename = item.finalUrl;
    }

    QByteArray data;

    if (!CacheServer::readFile(filename, data)) {
        return data;
    }

    return data;
}*/

QString CacheServer::getFileUrl(const QString &id)
{
    //qDebug() << "getFileUrl, id=" << id;

    Settings *s = Settings::instance();

    DatabaseManager::CacheItem item = s->db->readCacheByEntry(id);

    QString filename;
    if (item.id == "") {
        item = s->db->readCacheByFinalUrl(id);
        filename = id;
    } else {
        filename = item.finalUrl;
    }

    QString path = s->getDmCacheDir() + "/" + filename;

    if (!QFile::exists(path)) {
        qWarning() << "File " << path << "does not exists!";
        return "";
    }

    return path;
}

void CacheServer::handle(QHttpRequest *req, QHttpResponse *resp)
{
    //qDebug() << "handle, url:" << req->url().toString();

    QStringList parts = req->url().path().split('/');
    //qDebug() << "handle, parts.length():" << parts.length();
    if (parts.length() > 1) {
        if (parts[1] == "test") {
            resp->setHeader("Content-Type", "text/html");
            resp->writeHead(200);
            resp->end("<html><body><h1>It works!</h1></body></html>");
            return;
        }

        /*if (parts[1] == "assets" && parts.length() > 2) {
            //qDebug() << "handle, path=" << req->url().path();
            QString path = "app/native" + req->url().path();
            if (QFile::exists(path)) {
                QByteArray content;
                QStringList extParts = parts[parts.length()-1].split('.');
                QString ext = extParts.length() > 0 ? extParts[extParts.length()-1] : "";

                if (ext == "js") {
                    resp->setHeader("Content-Type", "application/javascript");
                    resp->setHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
                    resp->setHeader("Pragma", "no-cache");
                    resp->writeHead(200);
                    CacheServer::readFile2(path, content);
                    resp->end(content);
                    return;
                }
            }
            resp->writeHead(404);
            resp->end("");
            return;
        }*/

        /*if (parts[1] == "rssdata" && parts.length() > 2) {
            qDebug() << "handle, path=" << req->url().path();
            Settings *s = Settings::instance();
            QString id = parts[2];
            QString content = s->db->readEntryContentById(id);
            QString style = "";//img {max-width: 100% !important; max-height: device-height !important;}";

            content = "<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>" +
                      (style.isEmpty() ? "" : "<style>" + style + "</style>") +
                      "</head><body>" + content + "</body></html>";

            content = "<!DOCTYPE html>\n<html><head><meta content='width=device-width, initial-scale=1.0' name='viewport'>" +
                      (style.isEmpty() ? "" : "<style>" + style + "</style>") +
                      "</head><body>" + content + "</body></html>";

            resp->setHeader("Content-Type", "text/html");
            resp->setHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
            resp->setHeader("Pragma", "no-cache");
            resp->writeHead(200);
            resp->end(content.toUtf8());
            return;
        }*/

        FilteringWorker *worker = new FilteringWorker();
        QObject::connect(this, SIGNAL(startWorker(QHttpRequest*,QHttpResponse*)), worker, SLOT(start(QHttpRequest*,QHttpResponse*)));
        QObject::connect(worker, SIGNAL(finished()), this, SLOT(handleFinish()));
        emit startWorker(req, resp);

    } else {
        resp->writeHead(404);
        resp->end("");
        return;
    }
}

void CacheServer::handleFinish()
{
    FilteringWorker *worker = qobject_cast<FilteringWorker*>(sender());

    if (worker->error)    {
        qDebug() << "handleFinish error:" << worker->req->url();
        worker->resp->setHeader("Content-Length", "0");
        worker->resp->setHeader("Connection", "close");
        worker->resp->writeHead(404);
        worker->resp->end("");
        return;
    }

    //worker->resp->setHeader("Content-Type", item.contentType);
    worker->resp->writeHead(200);
    worker->resp->end(worker->data);
    delete(worker);
}

QString CacheServer::getUrlbyId(const QString &item)
{
    return "http://localhost:" + QString::number(port) + "/" + item;
}

QString CacheServer::getUrlbyUrl(const QString &url)
{
    //qDebug() << "getUrlbyUrl, url=" << url << "hash=" << Utils::hash(url);

    if (url.isEmpty()) {
        return url;
    }

    // If url is "image://" will not be hashed
    if (url.startsWith("image://")) {
        return url;
    }

    // If url is "http://localhost" will not be hashed
    if (url.startsWith("http://localhost")) {
        return url;
    }

    QString filename = Utils::hash(url);
    Settings *s = Settings::instance();
    if (!QFile::exists(s->getDmCacheDir() + "/" + filename)) {
        qWarning() << "File " << filename << "does not exists!";
        return "";
    }

    return "http://localhost:" + QString::number(port) + "/" + filename;
}

QByteArray CacheServer::getDataUrlByUrl(const QString &url)
{
    Settings *s = Settings::instance();

    QString entryId = Utils::hash(url);
    DatabaseManager::CacheItem item = s->db->readCacheByEntry(entryId);

    QString filename;
    if (item.id == "") {
        item = s->db->readCacheByFinalUrl(entryId);
        filename = entryId;
    } else {
        filename = item.finalUrl;
    }

    QByteArray data;
    if (!CacheServer::readFile(filename, data)) {
        return QByteArray();
    }

    QStringList ct = item.contentType.split(';');
    return QString("data:"+ct[0]+";base64,").toUtf8() + data.toBase64();
}

QString CacheServer::getCacheUrlbyUrl(const QString &url)
{
    // If url is "image://" will not be hashed
    if (url.isEmpty() || url.startsWith("image://")) {
        return url;
    }

    return "cache://" + Utils::hash(url);
}


