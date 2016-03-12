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

//#include <QWebPage>
//#include <QWebFrame>
//#include <QWebElement>

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

/*void FilteringWorker::advancedFilter()
{
    QUrl url(item.baseUrl);

    if (url.host().contains("reddit.com")) {
        QWebPage page;
        QWebFrame *frame = page.mainFrame();
        frame->setHtml(content);
        QWebElement el1 = frame->findFirstElement("div[role='banner']");
        if (!el1.isNull())
            el1.removeFromDocument();
        content = frame->toHtml();
        return;
    }

    qDebug() << "-2";

    if (url.host().contains("lightreading.com")) {
        QWebPage page;
        qDebug() << "-1";
        QWebFrame *frame = page.mainFrame();
        frame->setHtml(content);

        qDebug() << "0";
        QWebElement el1 = frame->findFirstElement("article");
        if (!el1.isNull()) {
            qDebug() << "1";
            QWebElement el2 = frame->findFirstElement("div[name='msgqueue']");
            QWebElement body = frame->findFirstElement("body");
            if (!body.isNull()) {
                qDebug() << "2";
                body.removeFromDocument();
                QWebElement head = frame->findFirstElement("head");
                head.appendOutside(el2);
                head.appendOutside(el1);
                content = frame->toHtml();
            }
        }
        return;
    }
}*/

void FilteringWorker::filter()
{
    QUrl url(item.baseUrl);
    bool dofilterArticle = true;
    if (url.host().contains("engadget.com")) {
        dofilterArticle = false;
    }

    if (dofilterArticle)
        filterArticle();

    QRegExp rxLinkAll("<link[^>]*>", Qt::CaseInsensitive);
    QRegExp rxScriptAll("<script[^>]*>((?!<\\/script>).)*<\\/script>", Qt::CaseInsensitive);
    QRegExp rxStyleAll("<style[^>]*>((?!<\\/style>).)*<\\/style>", Qt::CaseInsensitive);
    QRegExp rxStyle("\\s*style\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxClass("\\s*class\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    //QRegExp rxId("\\s*id\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxWidth("\\s*width\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxHeight("\\s*height\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxMetaViewport("<meta\\s[^>]*name\\s*=(\"viewport\"|'viewport')[^>]*>", Qt::CaseInsensitive);
    //QRegExp rxMetaAll("<meta[^>]*>", Qt::CaseInsensitive);
    //QRegExp rxFormAll("<form[^>]*>((?!<\\/form>).)*<\\/form>", Qt::CaseInsensitive);
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

    // Applying Theme's style
    QUrl query = req->url();
    Settings *s = Settings::instance();
    QString style, fontsize, highlightColor, secondaryHighlightColor;
    int margin = 0;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QUrlQuery urlQuery(query);
    if (urlQuery.hasQueryItem("fontsize"))
        fontsize = urlQuery.queryItemValue("fontsize");
    if (urlQuery.hasQueryItem("highlightColor"))
        highlightColor = urlQuery.queryItemValue("highlightColor");
    if (urlQuery.hasQueryItem("secondaryHighlightColor"))
        secondaryHighlightColor = urlQuery.queryItemValue("secondaryHighlightColor");
    if (urlQuery.hasQueryItem("margin"))
        margin = urlQuery.queryItemValue("margin").toInt();
#else
    if (query.hasQueryItem("fontsize"))
        fontsize= query.queryItemValue("fontsize");
    if (query.hasQueryItem("highlightColor"))
        highlightColor = query.queryItemValue("highlightColor");
    if (query.hasQueryItem("secondaryHighlightColor"))
        secondaryHighlightColor = query.queryItemValue("secondaryHighlightColor");
    if (query.hasQueryItem("margin"))
        margin = query.queryItemValue("margin").toInt();
#endif

    QString initialScale = "1.0";

    if (s->getOfflineTheme() == "white") {
        style = QString("<meta name='viewport' content='initial-scale=%6'>"
                        "<style>a,h1,h2,h3,div,p,pre,code{word-wrap:break-word}body{margin:%5px;background:#FFF;font-family:sans-serif;font-size:%1;color:#323232;}figure{margin:0;padding:0;}a:link{color:#%2;}a:visited{color:#%4;}a:active{color:#%3;}img{max-width:100%;max-height:device-height;}</style></head>")
                .arg(fontsize).arg(highlightColor).arg(highlightColor).arg(secondaryHighlightColor).arg(margin).arg(initialScale);
    }

    if (s->getOfflineTheme() == "black") {
        style = QString("<meta name='viewport' content='initial-scale=%6'>"
                        "<style>a,h1,h2,h3,div,p,pre,code{word-wrap:break-word}body{margin:%5px;background:#141414;font-family:sans-serif;font-size:%1;color:#FFF;}figure{margin:0;padding:0;}a:link{color:#%2;}a:visited{color:#%4;}a:active{color:#%3;}img{max-width:100%;max-height:device-height;}</style></head>")
                .arg(fontsize).arg(highlightColor).arg(highlightColor).arg(secondaryHighlightColor).arg(margin).arg(initialScale);
        //qDebug() << style;
    }

    QRegExp rxHeadEnd("</head>", Qt::CaseInsensitive);
    content.replace(rxHeadEnd,style);
}

void FilteringWorker::filterOnline()
{
    filter();
    //advancedFilter();

    //QRegExp rxCss("<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxImg("(<img\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    resolveRelativeUrls(rxImg);
    QRegExp rxA("(<a\\s[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    resolveRelativeUrls(rxA);
}

void FilteringWorker::filterOffline()
{
    filter();

    //QRegExp rxMetaAll("<meta[^>]*>", Qt::CaseInsensitive);
    //QRegExp rxA("(<a\\s[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxUrl("url[\\s]*\\([^\\)]*\\)", Qt::CaseInsensitive);
    QRegExp rxImgAll("<img[^>]*>", Qt::CaseInsensitive);
    QRegExp rxFrame("(<iframe\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    QRegExp rxBody("(<body[^>]*>)", Qt::CaseInsensitive);

    //content.replace(rxA,"\\1href='#'");
    //removeUrls(content, rxA);
    content.replace(rxUrl,"http://0.0.0.0");
    content.replace(rxImgAll,"");
    content.replace(rxFrame,"\\1");

    // Inserting image after <body> tag
    Settings *s = Settings::instance();
    QString image = "";
    if(item.entryId!="")
        image = s->db->readEntryImageById(item.entryId);
    if (image!="") {
        content.replace(rxBody,QString("\\1<img src=\"%1\"/>").arg(s->cache->getUrlbyUrl(image)));
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
    //qDebug() << "handle, url=" << req->url().toString();
    if (req->url().path() == "/test") {
        resp->setHeader("Content-Type", "text/html");
        resp->writeHead(200);
        resp->end("<html><body><h1>It works!</h1></body></html>");
        return;
    }

    FilteringWorker *worker = new FilteringWorker();
    QObject::connect(this, SIGNAL(startWorker(QHttpRequest*,QHttpResponse*)), worker, SLOT(start(QHttpRequest*,QHttpResponse*)));
    QObject::connect(worker, SIGNAL(finished()), this, SLOT(handleFinish()));
    emit startWorker(req, resp);
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


/*#define HOST_FILTER        0x01  // => 0001
#define REMOVE_FILTER        0x02  // => 0010

void CacheServer::resolveRelativeUrls(QString &content, const QRegExp &rx, const QUrl &baseUrl, int filter)
{
    QStringList list = baseUrl.host().split(".");
    QString baseHost = list.at(list.length()-2);

    int i = 1, pos = 0;
    QStringList caps;
    while ((pos = rx.indexIn(content, pos)) != -1) {
        QString cap = rx.cap(2); cap = cap.mid(1,cap.length()-2);
        caps.append(cap);
        pos += rx.matchedLength();
        ++i;
    }

    //qDebug() << filter << (filter & REMOVE_FILTER) ;

    QStringList::iterator it = caps.begin();
    while (it != caps.end()) {
        QString cap = *it;
        QUrl capUrl(cap);

        if ((filter & REMOVE_FILTER) != 0) {
            content.replace(cap, "");
            qDebug() << "removed cap=" << cap;
        } else if (capUrl.isRelative()) {
            content.replace(cap, baseUrl.resolved(capUrl).toString());
        } else {
            qDebug() << "filter, host=" << capUrl.host() << "base host=" << baseUrl.host();
            if ((filter & HOST_FILTER) != 0) {
                // Host filter => Removing URL if host not match base URL's host
                list = capUrl.host().split(".");
                if (list.length()>1) {
                    QString capHost = list.at(list.length()-2);
                    qDebug() << "filter, capHost=" << capHost << "baseHost=" << baseHost;
                    if (capHost!=baseHost) {
                        qDebug() << "replace";
                        content.replace(cap, "");
                    }
                }
            }
        }

        ++it;
    }
}*/

/*QString CacheServer::hash(const QString &url)
{
    QByteArray data; data.append(url);
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
}*/

QString CacheServer::getUrlbyId(const QString &item)
{
    //return "http://127.0.0.1:" + QString::number(port) + "/" + item;
    return "http://localhost:" + QString::number(port) + "/" + item;
}

QString CacheServer::getUrlbyUrl(const QString &url)
{
    //qDebug() << "getUrlbyUrl, url=" << url << "hash=" << Utils::hash(url);

    // If url is "image://" will not be hashed
    if (url.isEmpty() || url.startsWith("image://")) {
        return url;
    }

    //return "http://127.0.0.1:" + QString::number(port) + "/" + Utils::hash(url);
    return "http://localhost:" + QString::number(port) + "/" + Utils::hash(url);
}

QString CacheServer::getCacheUrlbyUrl(const QString &url)
{
    // If url is "image://" will not be hashed
    if (url.isEmpty() || url.startsWith("image://")) {
        return url;
    }

    return "cache://" + Utils::hash(url);
}


