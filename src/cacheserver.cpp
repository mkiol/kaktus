/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QRegExp>
#include <QTextCodec>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QUrlQuery>
#endif

#include "cacheserver.h"
#include "utils.h"

FilteringWorker::FilteringWorker(QObject *parent) : QThread{parent} {}

void FilteringWorker::start(QHttpRequest *req, QHttpResponse *resp) {
    sender()->disconnect(this);
    this->req = req;
    this->resp = resp;
    QThread::start(QThread::LowPriority);
}

void FilteringWorker::run() {
    auto *db = DatabaseManager::instance();

    QString entryId = req->url().path();
    if (entryId.at(0) == '/') entryId = entryId.right(entryId.length() - 1);
    item = db->readCacheByEntry(entryId);

    QString filename;
    if (item.id.isEmpty()) {
        item = db->readCacheByFinalUrl(entryId);
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
        if (rx.indexIn(item.contentType) != -1)
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

void FilteringWorker::resolveRelativeUrls(const QRegExp &rx) {
    int i = 1, pos = 0;
    QStringList caps;
    while ((pos = rx.indexIn(content, pos)) != -1) {
        QString cap = rx.cap(2);
        cap = cap.mid(1, cap.length() - 2);
        if (cap != "" && cap != "/" && cap.at(0) != QChar('#'))
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
        // qDebug() << "baseUrl:" << baseUrl << "cap:" << cap;
        if (capUrl.isRelative()) {
            content.replace("'" + cap + "'",
                            "'" + baseUrl.resolved(capUrl).toString() + "'");
            content.replace("\"" + cap + "\"",
                            "\"" + baseUrl.resolved(capUrl).toString() + "\"");
            // qDebug() << "cap is relative" <<
            // baseUrl.resolved(capUrl).toString();
        }
        ++it;
    }
}

void FilteringWorker::removeUrls(const QRegExp &rx) {
    int i = 1, pos = 0;
    QStringList caps;
    while ((pos = rx.indexIn(content, pos)) != -1) {
        QString cap = rx.cap(2);
        cap = cap.mid(1, cap.length() - 2);
        if (cap != "" && cap != "/" && cap.at(0) != QChar('#'))
            caps.append(cap);
        pos += rx.matchedLength();
        ++i;
    }

    QStringList::iterator it = caps.begin();
    while (it != caps.end()) {
        QString cap = *it;
        QUrl capUrl(cap);
        if (capUrl.scheme() == "data") {
            content.replace(cap, "");
        }
        ++it;
    }
}

bool FilteringWorker::filterArticle() {
    QRegExp rx1("<article[^>]*>((?!<\\/article>).)*<\\/article>",
                Qt::CaseInsensitive);

    int i = 1, pos = 0;
    QStringList articles;
    while ((pos = rx1.indexIn(content, pos)) != -1) {
        if (i > 1) return false;
        QString cap = rx1.cap(0);
        if (cap != "") {
            // qDebug() << "article";
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
            // qDebug() << "meta: " << cap;
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

void FilteringWorker::filter() {
    QRegExp rxLinkAll("<link[^>]*>", Qt::CaseInsensitive);
    QRegExp rxScriptAll("<script[^>]*>((?!<\\/script>).)*<\\/script>",
                        Qt::CaseInsensitive);
    QRegExp rxStyleAll("<style[^>]*>((?!<\\/style>).)*<\\/style>",
                       Qt::CaseInsensitive);
    QRegExp rxStyle("\\s*style\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                    Qt::CaseInsensitive);
    QRegExp rxClass("\\s*class\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                    Qt::CaseInsensitive);
    QRegExp rxWidth("\\s*width\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                    Qt::CaseInsensitive);
    QRegExp rxHeight("\\s*height\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                     Qt::CaseInsensitive);
    QRegExp rxMetaViewport(
        "<meta\\s[^>]*name\\s*=(\"viewport\"|'viewport')[^>]*>",
        Qt::CaseInsensitive);
    QRegExp rxInputAll("<input[^>]*>", Qt::CaseInsensitive);
    QRegExp rxTextareaAll("<textarea[^>]*>((?!<\\/textarea>).)*<\\/textarea>",
                          Qt::CaseInsensitive);
    QRegExp rxObjectAll("<object[^>]*>((?!<\\/object>).)*<\\/object>",
                        Qt::CaseInsensitive);
    QRegExp rxButtonAll("<button[^>]*>((?!<\\/button>).)*<\\/button>",
                        Qt::CaseInsensitive);
    QRegExp rxNoscriptAll("<noscript[^>]*>((?!<\\/noscript>).)*<\\/noscript>",
                          Qt::CaseInsensitive);
    QRegExp rxSelectAll("<select[^>]*>((?!<\\/select>).)*<\\/select>",
                        Qt::CaseInsensitive);
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

void FilteringWorker::filterOnline() {
    filter();

    // QRegExp
    // rxCss("<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*href\\s*=\\s*(\"[^\"]*\"|'[^']*')",
    // Qt::CaseInsensitive); QRegExp
    // rxImg("(<img\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')",
    // Qt::CaseInsensitive); resolveRelativeUrls(rxImg); QRegExp
    // rxA("(<a\\s[^>]*)href\\s*=\\s*(\"[^\"]*\"|'[^']*')",
    // Qt::CaseInsensitive); resolveRelativeUrls(rxA);
}

void FilteringWorker::filterOffline() {
    filter();

    QRegExp rxUrl("url[\\s]*\\([^\\)]*\\)", Qt::CaseInsensitive);
    QRegExp rxImgAll("<img[^>]*>", Qt::CaseInsensitive);
    QRegExp rxFrame("(<iframe\\s[^>]*)src\\s*=\\s*(\"[^\"]*\"|'[^']*')",
                    Qt::CaseInsensitive);
    QRegExp rxBody("(<body[^>]*>)", Qt::CaseInsensitive);

    content.replace(rxUrl, "http://0.0.0.0");
    content.replace(rxImgAll, "");
    content.remove("</img>", Qt::CaseInsensitive);
    content.replace(rxFrame, "\\1");

    // Inserting image after <body> tag
    auto db = DatabaseManager::instance();
    QString image;
    if (!item.entryId.isEmpty()) image = db->readEntryImageById(item.entryId);
    if (!image.isEmpty()) {
        image = QString(CacheServer::getDataUrlByUrl(image));
        if (!image.isEmpty()) {
            content.replace(
                rxBody,
                QString("\\1<img id='_kaktus_img' src='%1'/>").arg(image));
        }
    }
}

CacheServer::CacheServer(QObject *parent) : QObject(parent) {
    server = new QHttpServer;

    QObject::connect(server,
                     SIGNAL(newRequest(QHttpRequest *, QHttpResponse *)), this,
                     SLOT(handle(QHttpRequest *, QHttpResponse *)));

    if (!server->listen(port)) {
        qWarning() << "Cache server at localhost failed to start on" << port
                   << "port";
    }
}

bool CacheServer::readFile(const QString &filename, QByteArray &data) {
    Settings *s = Settings::instance();
    QString cacheDir = s->getDmCacheDir();

    QFile file(cacheDir + "/" + filename);

    if (!QFile::exists(cacheDir + "/" + filename)) {
        qWarning() << "File " << filename << "does not exists";
        file.close();
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open" << filename
                   << "for reading: " << file.errorString();
        file.close();
        return false;
    }

    data.append(file.readAll());
    file.close();

    return true;
}

bool CacheServer::readFile2(const QString &path, QByteArray &data) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open" << path
                   << "for reading: " << file.errorString();
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

QString CacheServer::getFileUrl(const QString &id) {
    // qDebug() << "getFileUrl, id=" << id;

    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    DatabaseManager::CacheItem item = db->readCacheByEntry(id);

    QString filename;
    if (item.id.isEmpty()) {
        // item = db->readCacheByFinalUrl(id);
        filename = id;
    } else {
        filename = item.finalUrl;
    }

    QString path = s->getDmCacheDir() + "/" + filename;

    if (!QFile::exists(path)) {
        qWarning() << "File " << path << "does not exists";
        return QString();
    }

    return path;
}

void CacheServer::handle(QHttpRequest *req, QHttpResponse *resp) {
    QStringList parts = req->url().path().split('/');

    if (parts.length() > 1) {
        if (parts[1] == "test") {
            resp->setHeader("Content-Type", "text/html");
            resp->writeHead(200);
            resp->end("<html><body><h1>It works!</h1></body></html>");
            return;
        }

        FilteringWorker *worker = new FilteringWorker();
        QObject::connect(this,
                         SIGNAL(startWorker(QHttpRequest *, QHttpResponse *)),
                         worker, SLOT(start(QHttpRequest *, QHttpResponse *)));
        QObject::connect(worker, SIGNAL(finished()), this,
                         SLOT(handleFinish()));
        emit startWorker(req, resp);

    } else {
        resp->writeHead(404);
        resp->end();
        return;
    }
}

void CacheServer::handleFinish() {
    FilteringWorker *worker = qobject_cast<FilteringWorker *>(sender());

    if (worker->error) {
        qDebug() << "handleFinish error:" << worker->req->url();
        worker->resp->setHeader("Content-Length", "0");
        worker->resp->setHeader("Connection", "close");
        worker->resp->writeHead(404);
        worker->resp->end();
        return;
    }

    worker->resp->writeHead(200);
    worker->resp->end(worker->data);
    delete (worker);
}

QString CacheServer::getUrlbyId(const QString &item) {
    return "http://localhost:" + QString::number(port) + "/" + item;
}

QString CacheServer::getUrlbyUrl(const QString &url) {
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
    QString path = QDir(s->getDmCacheDir()).absoluteFilePath(filename);

    return QFile::exists(path)
               ? "http://localhost:" + QString::number(port) + "/" + filename
               : "";
}

QString CacheServer::getPathByUrl(const QString &url) {
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
    QString path = QDir(s->getDmCacheDir()).absoluteFilePath(filename);

    return QFile::exists(path) ? path : "";
}

bool CacheServer::getPathAndContentTypeByUrl(const QString &url, QString &path,
                                             QString &contentType) {
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    QString filename = Utils::hash(url);
    path = QDir(s->getDmCacheDir()).absoluteFilePath(filename);
    if (QFile::exists(path)) {
        contentType = db->readCacheByOrigUrl(filename).contentType;
        if (contentType.isEmpty()) return false;
    } else {
        return false;
    }

    return true;
}

QByteArray CacheServer::getDataUrlByUrl(const QString &url) {
    auto db = DatabaseManager::instance();

    QString entryId = Utils::hash(url);
    DatabaseManager::CacheItem item = db->readCacheByEntry(entryId);

    QString filename;
    if (item.id.isEmpty()) {
        item = db->readCacheByFinalUrl(entryId);
        filename = entryId;
    } else {
        filename = item.finalUrl;
    }

    QByteArray data;
    if (!CacheServer::readFile(filename, data)) {
        return QByteArray();
    }

    QStringList ct = item.contentType.split(';');
    return QString("data:" + ct[0] + ";base64,").toUtf8() + data.toBase64();
}

QString CacheServer::getCacheUrlbyUrl(const QString &url) {
    // If url is "image://" will not be hashed
    if (url.isEmpty() || url.startsWith("image://")) {
        return url;
    }

    return "cache://" + Utils::hash(url);
}
