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

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QCryptographicHash>
#include <QTimer>
#include <QUrl>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QNetworkConfiguration>

#include "downloadmanager.h"
//#include "netvibesfetcher.h"
#include "fetcher.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
#include "utils.h"
#endif


DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent)
{
    /*QList<QNetworkConfiguration> activeConfigs = ncm.allConfigurations();
    qDebug() << "activeConfigs" << activeConfigs.length();
    QList<QNetworkConfiguration>::iterator i = activeConfigs.begin();
    while (i != activeConfigs.end()) {
        qDebug() << (*i).bearerTypeName();
        ++i;
    }*/

    lastCacheSize = 0;
    cacheSizeFreshFlag = false;

    connect(&adder, SIGNAL(addDownload(DatabaseManager::CacheItem)), this, SLOT(addDownload(DatabaseManager::CacheItem)));
    connect(&cacheDeterminer, SIGNAL(cacheDetermined(int)), this, SLOT(cacheSizeDetermined(int)));
    connect(&cleaner, SIGNAL(finished()), this, SLOT(cacheCleaningFinished()));
    connect(&remover, SIGNAL(finished()), this, SLOT(cacheRemoverFinished()));
    connect(&remover, SIGNAL(progressChanged(int,int)), this, SLOT(cacheRemoverProgressChanged(int,int)));
#ifdef ONLINE_CHECK
    connect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(onlineStateChanged(bool)));
#endif
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(downloadFinished(QNetworkReply*)));
    connect(&manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
}

DownloadManager::~DownloadManager()
{}

bool DownloadManager::isWLANConnected()
{
    QList<QNetworkConfiguration> activeConfigs = ncm.allConfigurations(QNetworkConfiguration::Active);
    QList<QNetworkConfiguration>::iterator i = activeConfigs.begin();
    while (i != activeConfigs.end()) {
        QNetworkConfiguration c = (*i);
        //qDebug() << c.bearerTypeName() << c.identifier() << c.name();
        if (c.bearerType()==QNetworkConfiguration::BearerWLAN ||
            c.bearerType()==QNetworkConfiguration::BearerEthernet) {
            return true;
        }
        ++i;
    }

    return false;
}

bool DownloadManager::isOnline()
{
#ifdef ONLINE_CHECK
    return ncm.isOnline();
#endif
#ifndef ONLINE_CHECK
    return true;
#endif
}

void DownloadManager::onlineStateChanged(bool isOnline)
{
    Q_UNUSED(isOnline)
    emit onlineChanged();
}

void DownloadManager::startDownload()
{
    Settings *s = Settings::instance();

    //qDebug() << "DownloadManager::startDownload" << s->fetcher->isBusy() << queue.isEmpty();

    if (s->fetcher->isBusy() ||
            queue.isEmpty()) {
        return;
    }

    emit busyChanged();
    addNextDownload();
}

void DownloadManager::removerCancel()
{
    remover.cancel();
}

void DownloadManager::cacheSizeDetermined(int size)
{
    if (size!=lastCacheSize) {
        lastCacheSize = size;
        cacheSizeFreshFlag = true;
        emit cacheSizeChanged();
    }
}

int DownloadManager::getCacheSize()
{
    if (!cacheSizeFreshFlag) {
        cacheDeterminer.start(QThread::IdlePriority);
    } else {
        cacheSizeFreshFlag = false;
    }
    return lastCacheSize;
}

void DownloadManager::cleanCache()
{
    cleaner.start(QThread::IdlePriority);
}

void DownloadManager::cacheCleaningFinished()
{
    emit cacheSizeChanged();
    emit cacheCleaned();
}

void DownloadManager::cacheRemoverProgressChanged(int current, int total)
{
    emit removerProgressChanged(current, total);
}

void DownloadManager::cacheRemoverFinished()
{
    emit removerBusyChanged();
    emit cacheSizeChanged();
}

void DownloadManager::removeCache()
{
    if (isRemoverBusy())
        return;

    Settings *s = Settings::instance();
    s->db->removeCacheItems();
    remover.start(QThread::LowPriority);
    emit removerBusyChanged();
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
    //qDebug() << "item.finalUrl:" << item.finalUrl;
    QNetworkRequest request(QUrl(item.finalUrl));
    Settings *s = Settings::instance();
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    request.setHeader(QNetworkRequest::UserAgentHeader, s->getDmUserAgent());
#else
    request.setRawHeader("User-Agent", s->getDmUserAgent().toLatin1());
#endif

    request.setRawHeader("Accept", "*/*");
    QNetworkReply *reply = manager.get(request);
    replyToCheckerMap.insert(reply, new Checker(reply));
    replyToCachedItemMap.insert(reply, item);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    downloads.append(reply);
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{
    if (code == QNetworkReply::OperationCanceledError) {
        return;
    }

    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
    int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray httpPhrase = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
    //qWarning() << "Error in DownloadManager!, error code:" << code << ", HTTP code:" << httpCode << httpPhrase << reply->readAll();
    qWarning() << "Error in DownloadManager!, error code:" << code << ", HTTP code:" << httpCode << httpPhrase;
}

void DownloadManager::addNextDownload()
{
    if (downloads.isEmpty() && queue.isEmpty()) {
        emit progress(0);
        emit ready();
        emit busyChanged();
        emit cacheSizeChanged();
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
    /*qDebug() << "Errorcode: " << reply->error() <<
    "HttpStatusCode: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() <<
    "Url:" << reply->url();*/

    Settings *s = Settings::instance();

    QUrl url = reply->url();
    QNetworkReply::NetworkError error = reply->error();
    DatabaseManager::CacheItem item = replyToCachedItemMap.take(reply);
    //qDebug() << "baseUrl" << item.baseUrl;
    delete replyToCheckerMap.take(reply);

    if (error) {

        /*qDebug() << "DM, Errorcode: " << error << "entryId=" << item.entryId;
        qWarning() << "Download of " << url.toEncoded().constData()
                   << " failed: " << reply->errorString();*/

        if (item.type == "online-item") {
            // Quick download in online mode
            emit onlineDownloadFailed();
            downloads.removeOne(reply);
            reply->deleteLater();
            addNextDownload();
            return;
        }

        if (item.entryId!="") {
            switch (error) {
            case QNetworkReply::OperationCanceledError:
                if (!checkIfHeadersAreValid(reply))
                    s->db->updateEntriesCachedFlagByEntry(item.entryId,QDateTime::currentDateTime().toTime_t(),2);
                break;
            case QNetworkReply::HostNotFoundError:
                //s->db->updateEntriesCachedFlagByEntry(item.entryId,QDateTime::currentDateTime().toTime_t(),4);
                break;
            case QNetworkReply::AuthenticationRequiredError:
                s->db->updateEntriesCachedFlagByEntry(item.entryId,QDateTime::currentDateTime().toTime_t(),5);
                break;
            case QNetworkReply::ContentNotFoundError:
                s->db->updateEntriesCachedFlagByEntry(item.entryId,QDateTime::currentDateTime().toTime_t(),6);
                break;
            default:
                break;
            }
        }

        // Write Cache item to DB
        if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
            item.contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
            if (item.type == "")
                item.type = item.contentType.section('/', 0, 0);
        }

        item.id = hash(item.finalUrl);
        item.origUrl = hash(item.origUrl);
        item.baseUrl = item.finalUrl;
        item.finalUrl = hash(item.finalUrl);
        item.date = QDateTime::currentDateTime().toTime_t();
        item.flag = 0;

        switch (error) {
        case QNetworkReply::OperationCanceledError:
            if (!checkIfHeadersAreValid(reply))
                item.flag = 2;
            break;
        case QNetworkReply::HostNotFoundError:
            item.flag = 4;
            break;
        case QNetworkReply::AuthenticationRequiredError:
            item.flag = 5;
            break;
        case QNetworkReply::ContentNotFoundError:
            item.flag = 6;
            break;
        default:
            item.flag = 9;
        }

        s->db->writeCache(item);

    } else {

        // Redirection
        if (reply->attribute(QNetworkRequest::RedirectionTargetAttribute).isValid()) {
            item.finalUrl = url.resolved(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl()).toString();
            //qDebug() << "RedirectionTarget: " << url.toString() << "entryId" << item.entryId;
            downloads.removeOne(reply);
            //qDebug() << "item.finalUrl:" << item.finalUrl;
            addDownload(item);
            reply->deleteLater();
            return;
        }

        // Download ok -> save to file
        if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
            item.contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

            bool onlineItem = false;
            if (item.type == "online-item") {
                // Quick download in online mode
                onlineItem = true;
                item.type = "";
            }

            if (item.type == "")
                item.type = item.contentType.section('/', 0, 0);

            if (item.type == "text" ||
                    item.type == "image" ||
                    item.type == "icon" ||
                    item.type == "entry-image") {

                QByteArray content = reply->readAll();

                // Check if tiny image, we do not want it
                if (item.type == "entry-image" && content.size()<minImageSize) {
                    //qDebug() << "Tiny image found:"<<item.finalUrl;

                    // Write Cache item to DB with flag=10
                    item.id = hash(item.entryId+item.finalUrl);
                    item.origUrl = hash(item.origUrl);
                    item.baseUrl = item.finalUrl;
                    item.finalUrl = hash(item.finalUrl);
                    item.date = QDateTime::currentDateTime().toTime_t();
                    item.flag = 10;
                    s->db->writeCache(item);

                } else {
                    //qDebug() << "url" << url.toString() << "finalUrl" << item.finalUrl;
                    //qDebug() << "hash" << hash(url.toString()) << "finalUrl" << hash(item.finalUrl);
                    //if (saveToDisk(hash(url.toString()), content)) {
                    if (saveToDisk(hash(item.finalUrl), content)) {
                        // Write Cache item to DB
                        //qDebug() << "Write Cache item to DB" << item.type << item.finalUrl;
                        item.id = hash(item.entryId+item.finalUrl);
                        //qDebug() << "hash(item.finalUrl): " << hash(item.finalUrl);
                        item.origUrl = hash(item.origUrl);
                        item.baseUrl = item.finalUrl;
                        //qDebug() << "baseUrl2" << item.baseUrl;
                        item.finalUrl = hash(item.finalUrl);
                        item.date = QDateTime::currentDateTime().toTime_t();
                        item.flag = 1;
                        s->db->writeCache(item);

                        if (item.entryId!="") {
                            // Scan for other resouces, only text files
                            //if (item.type == "text")
                            //    scanHtml(content, url);
                            s->db->updateEntriesCachedFlagByEntry(item.entryId,QDateTime::currentDateTime().toTime_t(),1);
                        }

                        if (onlineItem) {
                            emit this->onlineDownloadReady(item.entryId,item.baseUrl);
                            //qDebug() << "emit onlineDownloadReady, item.entryId" << item.entryId << "item.baseUrl" << item.baseUrl << "filename" << hash(url.toString()) << "hash(item.finalUrl)" << hash(item.finalUrl) << "finalurl" << item.finalUrl;
                        }

                    } else {
                        if (onlineItem)
                            emit this->onlineDownloadFailed();
                        emit this->error(501);
                        qWarning() << "Save to disk failed!";
                    }
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
    Settings *s = Settings::instance();
    bool busy = isBusy();

    // Starting icon downloading immediately,
    // other files when fetcher is not busy
    /*qDebug() << ">>>>>>>>>> addDownload";
    qDebug() << "item:" << item.finalUrl << item.type;
    qDebug() << "downloads.count():" <<  downloads.count();
    qDebug() << "queue.size():" <<  queue.size();
    qDebug() << "s->fetcher->isBusy():" <<  s->fetcher->isBusy();
    qDebug() << "busy:" <<  busy;*/

    if (
            item.type=="icon" ||
            (!s->fetcher->isBusy() &&
            downloads.count() < s->getDmConnections())
    ) {
        //qDebug() << "doDownload";
        doDownload(item);
        if (!busy && item.type!="online-item")
            emit busyChanged();
        //emit cacheSizeChanged();
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
    cleanCache();

    //qDebug() << "DownloadManager::startFeedDownload()";
    if (!ncm.isOnline()) {
        qWarning() << "Network is Offline!";
        //emit networkNotAccessible();
        //return false;
    }

    adder.start(QThread::LowestPriority);
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
    Settings *s = Settings::instance();
    return s->db->countEntriesNotCached();
}

bool DownloadManager::isBusy()
{
    if (downloads.isEmpty() && queue.isEmpty()) {
        return false;
    }

    return true;
}

bool DownloadManager::isRemoverBusy()
{
    return remover.isRunning();
}

void DownloadManager::onlineDownload(const QString& id, const QString& url)
{
    Settings *s = Settings::instance();
    DatabaseManager::CacheItem item;

    // Search by entryId
    if (id != "") {
        item = s->db->readCacheByEntry(id);
        if (item.id == "") {
            //qDebug() << "Search by id not found";
            // No cache item -> downloaing
            //qDebug() << "No cache item -> downloaing";
            item.entryId = id;
            item.origUrl = url;
            item.finalUrl = url;
            item.baseUrl = url;
            item.type = "online-item";
            emit addDownload(item);
            return;
        }
        //qDebug() << "Item found by entryId! baseUrl=" << item.baseUrl;
        emit onlineDownloadReady(id, "");

    } else {

        // Downloading
        item.entryId = id;
        item.origUrl = url;
        item.finalUrl = url;
        item.baseUrl = url;
        item.type = "online-item";
        emit addDownload(item);
        return;

        // Search by origUrl
        /*item = s->db->readCacheByOrigUrl(url);
        if (item.id == "") {
            qDebug() << "Search by origUrl not found";
            // No cache item -> downloaing
            //qDebug() << "No cache item -> downloaing";
            item.entryId = id;
            item.origUrl = url;
            item.finalUrl = url;
            item.baseUrl = url;
            item.type = "online-item";
            emit addDownload(item);
            return;
        }
        qDebug() << "Item found by origUrl! baseUrl=" << item.finalUrl;
        emit onlineDownloadReady("", item.baseUrl);*/
    }
}

void CacheCleaner::run() {

    Settings *s = Settings::instance();
    QString cacheDir = s->getDmCacheDir();

    QList<QString> streamList = s->db->readStreamIds();
    QList<QString>::iterator ii = streamList.begin();
    while (ii!=streamList.end()) {
        QList<QString> cacheList = s->db->readCacheFinalUrlsByStream(*ii, entriesLimit);
        QList<QString>::iterator iii = cacheList.begin();
        while (iii!=cacheList.end()) {
            QString filepath = cacheDir + "/" + *iii;
            if (QFile::exists(filepath)) {
                if (!QFile::remove(filepath)) {
                    qWarning() << "Unable to remove file " << filepath;
                }
            }
            ++iii;
            QThread::msleep(10);
        }

        s->db->removeEntriesByStream(*ii, entriesLimit);

        ++ii;
    }
}

CacheRemover::CacheRemover(QObject *parent) : QThread(parent)
{
    total = 100;
    current = 0;
    doCancel = false;
}

/*
 * Copyright (c) 2009 John Schember <john@nachtimwald.com>
 * http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
 */
bool CacheRemover::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    emit progressChanged(0,total);

    //qDebug() << "dirName" << dirName;
    if (dir.exists(dirName)) {
        QFileInfoList infoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
        total = infoList.count();
        //qDebug() << "total" << total;
        Q_FOREACH(QFileInfo info, infoList) {
            if (doCancel)
                return result;

            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
                ++current;
                if (current % 10 == 0)
                    emit progressChanged(++current,total);
            }

            if (!result) {
                return result;
            }
            //qDebug() << "File" << info.absoluteFilePath() << "removed. Result:" << result;
            //QThread::msleep(5);
        }
        result = dir.rmdir(dirName);
    }

    emit progressChanged(total,total);

    return result;
}

void CacheRemover::run()
{
    current=0; total = 100; doCancel = false;
    Settings *s = Settings::instance();
    if (!removeDir(s->getDmCacheDir())) {
        qWarning() << "Unable to remove " << s->getDmCacheDir();
    }

    // Remove QtWebKit cache files
    QString cacheDir = s->getSettingsDir();
    cacheDir.append(QDir::separator()).append(".QtWebKit");
    if (!removeDir(cacheDir)) {
        qWarning() << "Unable to remove " << cacheDir;
    }

    //emit ready();
}

void CacheRemover::cancel()
{
    doCancel = true;
}

DownloadAdder::DownloadAdder(QObject *parent) : QThread(parent)
{}

void DownloadAdder::run()
{
    Settings *s = Settings::instance();
    QMap<QString,QString> list = s->db->readNotCachedEntries();
    //qDebug() << "startFeedDownload, list.count=" << list.count();

    if (list.isEmpty()) {
        qWarning() << "No feeds to download!";
        return;
    }

    QMap<QString,QString>::iterator i = list.begin();
    while (i != list.end()) {
        if (i.key() != "" && i.value() != "") {
            DatabaseManager::CacheItem item;
            item.entryId = i.key();
            item.origUrl = i.value();
            item.finalUrl = i.value();
            //qDebug() << "adding to download" << item.finalUrl;
            emit addDownload(item);
        }
        ++i;
    }
}

CacheDeterminer::CacheDeterminer(QObject *parent) : QThread(parent)
{}

void CacheDeterminer::run()
{
    int size = 0;
    Settings *s = Settings::instance();
    QDirIterator i(s->getDmCacheDir());
    while (i.hasNext()) {
        if (i.fileInfo().isFile())
            size += i.fileInfo().size();
        i.next();
    }
    emit cacheDetermined(size);
}
