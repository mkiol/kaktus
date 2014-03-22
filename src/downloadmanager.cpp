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

#include "downloadmanager.h"

DownloadManager::DownloadManager(DatabaseManager *db)
{
    this->db = db; 

    /*QList<QNetworkConfiguration> activeConfigs = ncm.allConfigurations(QNetworkConfiguration::Active);
    QList<QNetworkConfiguration>::iterator i = activeConfigs.begin();
    while (i != activeConfigs.end()) {
        qDebug() << (*i).bearerTypeName();
        ++i;
    }*/
    //connect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(onlineStateChanged(bool)));

    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(downloadFinished(QNetworkReply*)));
    connect(&manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));

}

bool DownloadManager::isOnline()
{
    return ncm.isOnline();
}

void DownloadManager::onlineStateChanged(bool isOnline)
{
    qDebug() << "isOnline:" << isOnline;
}

void DownloadManager::cleanCache()
{
    Settings *s = Settings::instance();
    QString cacheDir = s->getDmCacheDir();
    int limit = s->getDmCacheRetencyFeedLimit();
    int date = QDateTime::currentDateTime().toTime_t() - s->getDmMaxCacheRetency();

    /*QDirIterator i(cacheDir);
    while (i.hasNext()) {
        if (i.fileInfo().isFile()) {
            //qDebug() << "Checking: " << i.filePath();
            if (!db->isCacheItemExistsByFinalUrl(i.fileName())) {
                if (!QFile::remove(i.filePath())) {
                    qWarning() << "Unable to remove file " << i.filePath();
                } else {
                    qDebug() << "Removing" << i.filePath();
                }
            }
        }
        i.next();
    }*/

    QList<QString> list = db->readCacheFinalUrlOlderThan(date, limit);
    //qDebug() << "list.count: " << list.count();
    QList<QString>::iterator i = list.begin();
    while (i!=list.end()) {
        QString filepath = cacheDir + "/" + *i;
        if (QFile::exists(filepath)) {
            if (!QFile::remove(filepath)) {
                qWarning() << "Unable to remove file " << filepath;
            } else {
                qDebug() << "Removing" << filepath;
            }
        }
        ++i;
    }
    db->removeEntriesOlderThan(date, limit);
}

void DownloadManager::removeCache()
{
    Settings *s = Settings::instance();
    QDir cache(s->getDmCacheDir());
    if (!cache.removeRecursively()) {
        qWarning() << "Unable to remove dir " << s->getDmCacheDir();
    }
}

void DownloadManager::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (isBusy()) {
        switch (accessible) {
        case QNetworkAccessManager::UnknownAccessibility:
            break;
        case QNetworkAccessManager::NotAccessible:
            qWarning() << "Network is not accessible!";
            cancel();
            emit networkNotAccessible();
            break;
        case QNetworkAccessManager::Accessible:
            break;
        }
    }
}

void DownloadManager::doDownload(DatabaseManager::CacheItem item)
{   
    QNetworkRequest request(QUrl(item.finalUrl));
    Settings *s = Settings::instance();
    request.setHeader(QNetworkRequest::UserAgentHeader, s->getDmUserAgent());
    request.setRawHeader("Accept", "*/*");
    QNetworkReply *reply = manager.get(request);
    replyToCheckerMap.insert(reply, new Checker(reply));
    replyToCachedItemMap.insert(reply, item);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    downloads.append(reply);
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{
    qDebug() << "error: " << code;
}

void DownloadManager::addNextDownload()
{
    if (downloads.isEmpty() && queue.isEmpty()) {
        emit progress(0);
        emit ready();
        return;
    }

    Settings *s = Settings::instance();
    if (downloads.count() < s->getDmConnections() && !queue.isEmpty()) {
        DatabaseManager::CacheItem item = queue.takeFirst();
        doDownload(item);
    }

    emit progress(downloads.count() + queue.count());
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{
    //qDebug() << "Errorcode: " << reply->error() << " HttpStatusCode: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << "Url:" << reply->url();

    QUrl url = reply->url();
    QNetworkReply::NetworkError error = reply->error();
    DatabaseManager::CacheItem item = replyToCachedItemMap.take(reply);
    delete replyToCheckerMap.take(reply);

    if (error) {

        //qDebug() << "DM, Errorcode: " << error << "entryId=" << item.entryId;
        /*qWarning() << "Download of " << url.toEncoded().constData()
                   << " failed: " << reply->errorString();*/

        if (item.entryId != "") {
            switch (error) {
            case QNetworkReply::OperationCanceledError:
                if (!checkIfHeadersAreValid(reply))
                    db->updateEntryCache(item.entryId,QDateTime::currentDateTime().toTime_t(),2);
                break;
            case QNetworkReply::HostNotFoundError:
                //db->updateEntryCache(item.entryId,QDateTime::currentDateTime().toTime_t(),4);
                break;
            case QNetworkReply::AuthenticationRequiredError:
                db->updateEntryCache(item.entryId,QDateTime::currentDateTime().toTime_t(),5);
                break;
            case QNetworkReply::ContentNotFoundError:
                db->updateEntryCache(item.entryId,QDateTime::currentDateTime().toTime_t(),6);
                break;
            default:
                break;
            }
        }

        // Write Cache item to DB
        if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
            item.contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
            item.type = item.contentType.section('/', 0, 0);
        }
        item.id = hash(item.finalUrl);
        item.origUrl = hash(item.origUrl);
        item.finalUrl = hash(item.finalUrl);

        switch (error) {
        case QNetworkReply::OperationCanceledError:
            if (!checkIfHeadersAreValid(reply))
                db->writeCache(item, QDateTime::currentDateTime().toTime_t(),2);
            break;
        case QNetworkReply::HostNotFoundError:
            db->writeCache(item, QDateTime::currentDateTime().toTime_t(),4);
            break;
        case QNetworkReply::AuthenticationRequiredError:
            db->writeCache(item, QDateTime::currentDateTime().toTime_t(),5);
            break;
        case QNetworkReply::ContentNotFoundError:
            db->writeCache(item, QDateTime::currentDateTime().toTime_t(),6);
            break;
        default:
            break;
        }

    } else {

        // Redirection
        if (reply->attribute(QNetworkRequest::RedirectionTargetAttribute).isValid()) {
            item.finalUrl = url.resolved(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl()).toString();
            //qDebug() << "RedirectionTarget: " << url.toString() << "entryId" << item.entryId;
            downloads.removeOne(reply);
            //replyToCachedItemMap.remove(reply);
            addDownload(item);
            reply->deleteLater();
            return;
        }

        // Download ok -> save to file
        if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
            item.contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
            item.type = item.contentType.section('/', 0, 0);

            if (item.type == "text" || item.type == "image") {
                QByteArray content = reply->readAll();
                if (saveToDisk(hash(url.toString()), content)) {

                    // Write Cache item to DB
                    item.id = hash(item.entryId+item.finalUrl);
                    //qDebug() << "hash(item.finalUrl): " << hash(item.finalUrl);
                    item.origUrl = hash(item.origUrl);
                    item.finalUrl = hash(item.finalUrl);
                    db->writeCache(item, QDateTime::currentDateTime().toTime_t());

                    if (item.entryId != "") {
                        // Scan for other resouces, only text files
                        //if (item.type == "text")
                        //    scanHtml(content, url);
                        db->updateEntryCache(item.entryId, QDateTime::currentDateTime().toTime_t());
                    }

                } else {
                    emit this->error(501);
                    qWarning() << "Save to disk failed!";
                }
            }
        }

    }

    downloads.removeOne(reply);
    reply->deleteLater();
    addNextDownload();
}

bool DownloadManager::checkIfHeadersAreValid(QNetworkReply *reply)
{
    // check length
    if (reply->header(QNetworkRequest::ContentLengthHeader).isValid()) {
        int length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
        Settings *s = Settings::instance();
        int maxSize = s->getDmMaxSize();
        if (length > maxSize) {
            //qDebug() << "length > maxSize";
            return false;
        }
    }

    // check content type
    if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
        QString type = reply->header(QNetworkRequest::ContentTypeHeader).toString().section('/', 0, 0);
        if (type != "text" && type != "image") {
            //qDebug() << "type != text | image";
            return false;
        }
    }
    //qDebug() << "headers are valid!";
    return true;
}

void DownloadManager::scanHtml(const QByteArray &content, const QUrl &url)
{
    QString contentStr(content);

    QRegExp rxCss("<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*href\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
    int i = 1, pos = 0;
    while ((pos = rxCss.indexIn(contentStr, pos)) != -1) {

        DatabaseManager::CacheItem item;
        item.origUrl = rxCss.cap(2); item.origUrl = item.origUrl.mid(1,item.origUrl.length()-2);
        item.finalUrl = url.resolved(QUrl(item.origUrl)).toString();

        if (!isUrlinQueue(item.origUrl, item.finalUrl))
            addDownload(item);
        else
            //qDebug() << "scanHtml, Url is in the queue!";

        pos += rxCss.matchedLength();
        ++i;
    }
}

bool DownloadManager::isUrlinQueue(const QString &origUrl, const QString &finalUrl)
{
    QList<DatabaseManager::CacheItem>::iterator i = queue.begin();
    while (i != queue.end()) {
        if ((*i).origUrl == origUrl || (*i).finalUrl == finalUrl)
            return true;
        ++i;
    }
    return false;
}

bool DownloadManager::saveToDisk(const QString &filename, const QByteArray &content)
{
    Settings *s = Settings::instance();
    QFile file(s->getDmCacheDir() + "/" + filename);

    if (file.exists()) {
        //qWarning() << "File exists, deleting file" << filename;
        file.remove();
    }

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open" << filename << "for writing: " << file.errorString();
        return false;
    }

    file.write(content);
    file.close();

    return true;
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, sslErrors)
        qWarning() << "SSL error: " << error.errorString();
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::addDownload(DatabaseManager::CacheItem item)
{
    if (downloads.isEmpty() && queue.isEmpty()) {
        emit busy();
    }

    Settings *s = Settings::instance();
    if (downloads.count() < s->getDmConnections()) {
        doDownload(item);
    } else {
        queue.append(item);
    }
}

Checker::Checker(QNetworkReply *reply)
{
    Settings *s = Settings::instance();
    maxTime = s->getDmTimeOut();
    maxSize = s->getDmMaxSize();

    this->reply = reply;

    connect(reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    //connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    QTimer::singleShot(maxTime, this, SLOT(timeout()));
}

Checker::~Checker()
{
    disconnect(reply, 0, this, 0);
}

void Checker::timeout()
{
    //qDebug() << "timeout";
    reply->close();
}

void Checker::metaDataChanged()
{
    if (reply->header(QNetworkRequest::ContentLengthHeader).isValid()) {
        int length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
            if (length > maxSize) {
                //qDebug() << "metaDataChanged, length=" << length;
                reply->close();
                return;
            }
            /*if (length == 0) {
                qDebug() << "length=" << length;
                _reply->close();
                return;
            }*/
    }
    if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
        QString type = reply->header(QNetworkRequest::ContentTypeHeader).toString().section('/', 0, 0);
        if (type != "text" && type != "image") {
            //qDebug() << "metaDataChanged, type=" << type;
            reply->close();
            return;
        }
    }
}

void DownloadManager::startFeedDownload()
{
    /*bool busy = !downloads.isEmpty() || !queue.isEmpty();
    if (busy) {
        qWarning() << "Download Manager is busy!";
        return;
    }*/

    QTimer::singleShot(0, this, SLOT(cleanCache()));

    if (!ncm.isOnline()) {
        qWarning() << "Network is Offline!";
        emit networkNotAccessible();
        return;
    }

    QMap<QString,QString> list = db->readNotCachedEntries();
    //qDebug() << "startFeedDownload, list.count=" << list.count();

    if (list.count() == 0) {
        emit ready();
        return;
    }

    //replyToCachedItemMap.clear();
    //replyToCheckerMap.clear();

    QMap<QString,QString>::iterator i = list.begin();
    while (i != list.end()) {

        DatabaseManager::CacheItem item;
        item.entryId = i.key();
        item.origUrl = i.value();
        item.finalUrl = i.value();

        addDownload(item);
        ++i;
    }
}

QString DownloadManager::hash(const QString &url)
{
    return QString(QCryptographicHash::hash(url.toLatin1(), QCryptographicHash::Md5).toHex());
}

void DownloadManager::cancel()
{
    queue.clear();

    QList<QNetworkReply*>::iterator i = downloads.begin();
    while (i!=downloads.end()) {
        (*i)->close();
        ++i;
    }

    downloads.clear();

    emit canceled();
}

int DownloadManager::itemsToDownloadCount()
{
    return db->readNotCachedEntriesCount();
}

bool DownloadManager::isBusy()
{
    if (downloads.isEmpty() && queue.isEmpty()) {
        return false;
    }

    return true;
}
