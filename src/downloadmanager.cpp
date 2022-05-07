/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "downloadmanager.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QNetworkConfiguration>
#include <QRegExp>
#include <QTimer>
#include <QUrl>

#include "fetcher.h"
#include "utils.h"

DownloadManager *DownloadManager::m_instance = nullptr;

DownloadManager *DownloadManager::instance(QObject *parent) {
    if (DownloadManager::m_instance == nullptr) {
        DownloadManager::m_instance = new DownloadManager{parent};
    }

    return DownloadManager::m_instance;
}

DownloadManager::DownloadManager(QObject *parent) : QObject{parent} {
    connect(&m_adder, &DownloadAdder::addDownload, this,
            &DownloadManager::addDownload);
    connect(&m_adder, &DownloadAdder::addingFinished, this,
            &DownloadManager::addingFinishedHandler);
    connect(&m_cacheDeterminer, &CacheDeterminer::cacheDetermined, this,
            &DownloadManager::cacheSizeDetermined);
    connect(&m_cleaner, &CacheCleaner::finished, this,
            &DownloadManager::cacheCleaningFinished);
    connect(&m_remover, &CacheRemover::finished, this,
            &DownloadManager::cacheRemoverFinished);
    connect(&m_remover, &CacheRemover::progressChanged, this,
            &DownloadManager::cacheRemoverProgressChanged);
    connect(&m_ncm, &QNetworkConfigurationManager::onlineStateChanged, this,
            &DownloadManager::onlineStateChanged);
    connect(&m_manager, &QNetworkAccessManager::finished, this,
            &DownloadManager::downloadFinished);
    connect(&m_manager, &QNetworkAccessManager::networkAccessibleChanged, this,
            &DownloadManager::networkAccessibleChanged);
}

bool DownloadManager::isWLANConnected() const {
    auto activeConfigs = m_ncm.allConfigurations(QNetworkConfiguration::Active);
    auto i = activeConfigs.begin();
    while (i != activeConfigs.end()) {
        if (i->bearerType() == QNetworkConfiguration::BearerWLAN ||
            i->bearerType() == QNetworkConfiguration::BearerEthernet) {
            return true;
        }
        ++i;
    }

    return false;
}

bool DownloadManager::isOnline() const { return m_ncm.isOnline(); }

void DownloadManager::onlineStateChanged(bool isOnline) {
    Q_UNUSED(isOnline)
    emit onlineChanged();
}

void DownloadManager::startDownload() {
    if (Settings::instance()->fetcher->isBusy() || m_queue.isEmpty()) {
        return;
    }

    emit busyChanged();
    addNextDownload();
}

void DownloadManager::removerCancel() { m_remover.cancel(); }

void DownloadManager::cacheSizeDetermined(int size) {
    if (size != m_lastCacheSize) {
        m_lastCacheSize = size;
        m_cacheSizeFreshFlag = true;
        emit cacheSizeChanged();
    }
}

int DownloadManager::getCacheSize() {
    if (!m_cacheSizeFreshFlag) {
        m_cacheDeterminer.start(QThread::IdlePriority);
    } else {
        m_cacheSizeFreshFlag = false;
    }
    return m_lastCacheSize;
}

void DownloadManager::cleanCache() { m_cleaner.start(QThread::IdlePriority); }

void DownloadManager::cacheCleaningFinished() {
    emit cacheSizeChanged();
    emit cacheCleaned();
}

void DownloadManager::cacheRemoverProgressChanged(int current, int total) {
    emit removerProgressChanged(current, total);
}

void DownloadManager::cacheRemoverFinished() {
    emit removerBusyChanged();
    emit cacheSizeChanged();
}

void DownloadManager::removeCache() {
    if (isRemoverBusy()) return;
    DatabaseManager::instance()->removeCacheItems();
    m_remover.start(QThread::LowPriority);
    emit removerBusyChanged();
}

void DownloadManager::networkAccessibleChanged(
    QNetworkAccessManager::NetworkAccessibility accessible) {
    if (isBusy()) {
        switch (accessible) {
            case QNetworkAccessManager::UnknownAccessibility:
                break;
            case QNetworkAccessManager::NotAccessible:
                qWarning() << "Network is not accessible";
                cancel();
                emit networkNotAccessible();
                break;
            case QNetworkAccessManager::Accessible:
                break;
        }
    }
}

void DownloadManager::doDownload(DatabaseManager::CacheItem &&item) {
    QNetworkRequest request{QUrl{item.finalUrl}};
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      Settings::instance()->getDmUserAgent());
    request.setRawHeader("Accept", "*/*");
    auto *reply = m_manager.get(request);
    m_replyToCheckerMap.insert(reply, new Checker{reply});
    m_replyToCachedItemMap.insert(reply, item);

    connect(reply, &QNetworkReply::sslErrors, this,
            &DownloadManager::sslErrors);
    connect(reply,
            static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(
                &QNetworkReply::error),
            this, &DownloadManager::handleError);

    m_downloads.append(reply);
}

void DownloadManager::handleError(QNetworkReply::NetworkError code) const {
    if (code == QNetworkReply::OperationCanceledError) {
        return;
    }

    QNetworkReply *reply = dynamic_cast<QNetworkReply *>(sender());
    int httpCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray httpPhrase =
        reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute)
            .toByteArray();
    qWarning() << "Error in DownloadManager!, error code:" << code
               << ", HTTP code:" << httpCode << httpPhrase;
}

void DownloadManager::addNextDownload() {
    if (m_downloads.isEmpty() && m_queue.isEmpty()) {
        emit progress(m_downloadTotal, m_downloadTotal);
        emit ready();
        emit busyChanged();
        emit cacheSizeChanged();
        m_downloadTotal = 0;
        return;
    }

    if (m_downloads.count() < Settings::instance()->getDmConnections() &&
        !m_queue.isEmpty()) {
        doDownload(m_queue.takeFirst());
    }

    int current = m_downloadTotal - (m_downloads.count() + m_queue.count());
    emit progress(current, m_downloadTotal);
}

void DownloadManager::downloadFinished(QNetworkReply *reply) {
    auto *db = DatabaseManager::instance();

    QUrl url = reply->url();
    QNetworkReply::NetworkError error = reply->error();
    DatabaseManager::CacheItem item = m_replyToCachedItemMap.take(reply);

    delete m_replyToCheckerMap.take(reply);

    if (error) {
        if (item.type == "online-item") {
            // Quick download in online mode
            emit onlineDownloadFailed();
            m_downloads.removeOne(reply);
            reply->deleteLater();
            addNextDownload();
            return;
        }

        if (!item.entryId.isEmpty()) {
            switch (error) {
                case QNetworkReply::OperationCanceledError:
                    if (!checkIfHeadersAreValid(reply))
                        db->updateEntriesCachedFlagByEntry(
                            item.entryId,
                            QDateTime::currentDateTime().toTime_t(), 2);
                    break;
                case QNetworkReply::HostNotFoundError:
                    break;
                case QNetworkReply::AuthenticationRequiredError:
                    db->updateEntriesCachedFlagByEntry(
                        item.entryId, QDateTime::currentDateTime().toTime_t(),
                        5);
                    break;
                case QNetworkReply::ContentNotFoundError:
                case QNetworkReply::ContentOperationNotPermittedError:
                case QNetworkReply::UnknownContentError:
                    db->updateEntriesCachedFlagByEntry(
                        item.entryId, QDateTime::currentDateTime().toTime_t(),
                        6);
                    break;
                default:
                    break;
            }
        }

        // Write Cache item to DB
        if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
            item.contentType =
                reply->header(QNetworkRequest::ContentTypeHeader).toString();
            if (item.type.isEmpty())
                item.type = item.contentType.section('/', 0, 0);
        }

        item.id = Utils::hash(item.finalUrl);
        item.origUrl = Utils::hash(item.origUrl);
        item.baseUrl = item.finalUrl;
        item.finalUrl = Utils::hash(item.finalUrl);
        item.date = QDateTime::currentDateTime().toTime_t();
        item.flag = 0;

        switch (error) {
            case QNetworkReply::OperationCanceledError:
                if (!checkIfHeadersAreValid(reply)) item.flag = 2;
                break;
            case QNetworkReply::HostNotFoundError:
                item.flag = 4;
                break;
            case QNetworkReply::AuthenticationRequiredError:
                item.flag = 5;
                break;
            case QNetworkReply::ContentNotFoundError:
            case QNetworkReply::ContentOperationNotPermittedError:
            case QNetworkReply::UnknownContentError:
                item.flag = 6;
                break;
            default:
                item.flag = 9;
        }

        db->writeCache(item);

    } else {
        // Redirection
        if (reply->attribute(QNetworkRequest::RedirectionTargetAttribute)
                .isValid()) {
            QString newFinalUrl =
                url.resolved(
                       reply
                           ->attribute(
                               QNetworkRequest::RedirectionTargetAttribute)
                           .toUrl())
                    .toString();
            if (item.finalUrl == newFinalUrl ||
                item.redirectUrl == newFinalUrl) {
                // Redirection loop detected -> skiping item
                qWarning() << "Redirection loop detected";
                m_downloads.removeOne(reply);
                reply->deleteLater();
                addNextDownload();
                return;
            }

            item.redirectUrl = item.finalUrl;
            item.finalUrl = newFinalUrl;
            m_downloads.removeOne(reply);
            addDownload(item);
            reply->deleteLater();
            return;
        }

        // Download ok -> save to file
        if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
            item.contentType =
                reply->header(QNetworkRequest::ContentTypeHeader).toString();

            bool onlineItem = false;
            if (item.type == "online-item") {
                // Quick download in online mode
                onlineItem = true;
                item.type.clear();
            }

            if (item.type.isEmpty())
                item.type = item.contentType.section('/', 0, 0);

            if (item.type == "text" || item.type == "image" ||
                item.type == "icon" || item.type == "entry-image") {
                QByteArray content = reply->readAll();

                // Check if tiny image, we do not want it
                if (item.type == "entry-image" &&
                    content.size() < minImageSize) {
                    // qDebug() << "Tiny image found:"<<item.finalUrl;

                    // Write Cache item to DB with flag=10
                    item.id = Utils::hash(item.entryId + item.finalUrl);
                    item.origUrl = Utils::hash(item.origUrl);
                    item.baseUrl = item.finalUrl;
                    item.finalUrl = Utils::hash(item.finalUrl);
                    item.date = QDateTime::currentDateTime().toTime_t();
                    item.flag = 10;
                    db->writeCache(item);

                } else {
                    auto path = saveToDisk(Utils::hash(item.finalUrl), content);
                    if (!path.isEmpty()) {
                        // Write Cache item to DB
                        QString origUrl = item.origUrl;

                        item.id = Utils::hash(item.entryId + item.finalUrl);
                        item.origUrl = Utils::hash(item.origUrl);
                        item.baseUrl = item.finalUrl;
                        item.finalUrl = Utils::hash(item.finalUrl);
                        item.date = QDateTime::currentDateTime().toTime_t();
                        item.flag = 1;
                        db->writeCache(item);

                        if (!item.entryId.isEmpty()) {
                            // Scan for other resouces, only text files
                            db->updateEntriesCachedFlagByEntry(
                                item.entryId,
                                QDateTime::currentDateTime().toTime_t(), 1);
                        }

                        if (onlineItem) {
                            emit onlineDownloadReady(item.entryId,
                                                     item.baseUrl);
                        } else {
                            emit downloadReady(origUrl, path, item.contentType);
                        }
                    } else {
                        if (onlineItem) {
                            emit onlineDownloadFailed();
                        } else {
                            emit downloadFailed(item.origUrl);
                        }

                        qWarning() << "Saving file has failed! Maybe out of "
                                      "disk space?";
                        emit this->error(501);
                    }
                }
            }
        }
    }

    m_downloads.removeOne(reply);
    reply->deleteLater();

    addNextDownload();
}

bool DownloadManager::checkIfHeadersAreValid(QNetworkReply *reply) {
    if (reply->header(QNetworkRequest::ContentLengthHeader).isValid()) {
        if (reply->header(QNetworkRequest::ContentLengthHeader).toInt() >
            Settings::instance()->getDmMaxSize()) {
            return false;
        }
    }

    if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
        auto type = reply->header(QNetworkRequest::ContentTypeHeader)
                        .toString()
                        .section('/', 0, 0);
        if (type != "text" && type != "image") {
            return false;
        }
    }

    return true;
}

void DownloadManager::scanHtml(const QByteArray &content, const QUrl &url) {
    QString contentStr{content};

    static const QRegExp rxCss{
        QStringLiteral("<link\\s[^>]*rel\\s*=(\"stylesheet\"|'stylesheet')[^>]*"
                       "href\\s*=\\s*("
                       "\"[^\"]*\"|'[^']*')"),
        Qt::CaseInsensitive};
    int i = 1, pos = 0;
    while ((pos = rxCss.indexIn(contentStr, pos)) != -1) {
        DatabaseManager::CacheItem item;
        item.origUrl = rxCss.cap(2);
        item.origUrl = item.origUrl.mid(1, item.origUrl.length() - 2);
        item.finalUrl = url.resolved(QUrl(item.origUrl)).toString();

        if (!isUrlinQueue(item.origUrl, item.finalUrl)) {
            addDownload(item);
        } else {
            pos += rxCss.matchedLength();
        }
        ++i;
    }
}

bool DownloadManager::isUrlinQueue(const QString &origUrl,
                                   const QString &finalUrl) {
    auto i = m_queue.begin();
    while (i != m_queue.end()) {
        if ((*i).origUrl == origUrl || (*i).finalUrl == finalUrl) return true;
        ++i;
    }
    return false;
}

QString DownloadManager::saveToDisk(const QString &filename,
                                    const QByteArray &content) const {
    Settings *s = Settings::instance();
    QDir dir{s->getDmCacheDir()};
    auto path = dir.absoluteFilePath(filename);
    QFile file{path};

    if (file.exists()) {
        if (!file.remove()) {
            qWarning() << "File" << filename << "exists, but unable to delete.";
            return {};
        }
    }

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open" << filename
                   << "for writing. Error string:" << file.errorString();
        return {};
    }

    if (file.write(content) == -1) {
        qWarning() << "Could not write data to" << filename
                   << ". Error string:" << file.errorString();
        return {};
    }

    return path;
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors) {
    for (const QSslError &error : sslErrors)
        qWarning() << "SSL error: " << error.errorString();
    if (Settings::instance()->getIgnoreSslErrors()) {
        qDebug() << "Ignoring SSL errors";
        qobject_cast<QNetworkReply *>(sender())->ignoreSslErrors();
    }
}

void DownloadManager::addDownload(DatabaseManager::CacheItem item) {
    Settings *s = Settings::instance();
    if (item.type == "icon" || (!s->fetcher->isBusy() &&
                                m_downloads.count() < s->getDmConnections())) {
        auto busyEmit = !isBusy() && item.type != "online-item";
        doDownload(std::move(item));
        if (busyEmit) emit busyChanged();
    } else {
        m_queue.append(item);
    }
}

Checker::Checker(QNetworkReply *reply) {
    Settings *s = Settings::instance();
    maxTime = s->getDmTimeOut();
    maxSize = s->getDmMaxSize();

    this->reply = reply;

    connect(reply, &QNetworkReply::metaDataChanged, this,
            &Checker::metaDataChanged);
    QTimer::singleShot(maxTime, this, &Checker::timeout);
}

Checker::~Checker() { disconnect(reply, 0, this, 0); }

void Checker::timeout() {
    reply->close();
}

void Checker::metaDataChanged() {
    if (reply->header(QNetworkRequest::ContentLengthHeader).isValid()) {
        if (reply->header(QNetworkRequest::ContentLengthHeader).toInt() >
            maxSize) {
            reply->close();
            return;
        }
    }
    if (reply->header(QNetworkRequest::ContentTypeHeader).isValid()) {
        auto type = reply->header(QNetworkRequest::ContentTypeHeader)
                        .toString()
                        .section('/', 0, 0);
        if (type != "text" && type != "image") {
            reply->close();
            return;
        }
    }
}

void DownloadManager::startFeedDownload() {
    cleanCache();

    if (!m_ncm.isOnline()) {
        qWarning() << "Network is offline";
    }

    m_adder.start(QThread::LowestPriority);
}

void DownloadManager::cancel() {
    m_queue.clear();

    QList<QNetworkReply *>::iterator i = m_downloads.begin();
    while (i != m_downloads.end()) {
        (*i)->close();
        ++i;
    }

    m_downloads.clear();

    emit canceled();
}

int DownloadManager::itemsToDownloadCount() const {
    return DatabaseManager::instance()->countEntriesNotCached();
}

bool DownloadManager::isBusy() const {
    return !m_downloads.isEmpty() || !m_queue.isEmpty();
}

bool DownloadManager::isRemoverBusy() const { return m_remover.isRunning(); }

void DownloadManager::onlineDownload(const QString &id, const QString &url) {
    auto *db = DatabaseManager::instance();
    DatabaseManager::CacheItem item;

    // Search by entryId
    if (!id.isEmpty()) {
        item = db->readCacheByEntry(id);
        if (item.id.isEmpty()) {
            // No cache item -> downloaing
            item.entryId = id;
            item.origUrl = url;
            item.finalUrl = url;
            item.baseUrl = url;
            item.type = "online-item";
            addDownload(item);
            return;
        }
        // qDebug() << "Item found by entryId! baseUrl=" << item.baseUrl;
        emit onlineDownloadReady(id, "");
    } else {
        // Downloading
        item.entryId = id;
        item.origUrl = url;
        item.finalUrl = url;
        item.baseUrl = url;
        item.type = "online-item";
        addDownload(item);
        return;
    }
}

void CacheCleaner::run() {
    Settings *s = Settings::instance();
    if (s->getSigninType() < 10)
        cleanNv();
    else
        cleanOr();
}

void CacheCleaner::cleanOr() {
    Settings *s = Settings::instance();

    if (s->getRetentionDays() < 1) {
        return;
    }

    QDir cacheDir(s->getDmCacheDir());
    QDateTime date =
        QDateTime::currentDateTime().addDays(0 - s->getRetentionDays());

    if (cacheDir.exists()) {
        QFileInfoList infoList =
            cacheDir.entryInfoList(QDir::Files, QDir::Time);
        foreach (const QFileInfo &info, infoList) {
            if (info.created() < date) {
                if (QFile::remove(info.absoluteFilePath())) {
                    qDebug()
                        << "Cache cleaner:" << info.fileName() << "deleted";
                } else {
                    qWarning() << "Cache cleaner:" << info.fileName()
                               << " is old but can not be deleted";
                }
            } else {
                return;
            }
            QThread::msleep(5);
        }
    }
}

void CacheCleaner::cleanNv() {
    auto *db = DatabaseManager::instance();

    auto streamList = db->readStreamIds();
    auto ii = streamList.begin();
    while (ii != streamList.end()) {
        auto cacheList = db->readCacheFinalUrlsByStream(*ii, entriesLimit);
        auto iii = cacheList.begin();
        while (iii != cacheList.end()) {
            auto filepath = Settings::instance()->getDmCacheDir() + "/" + *iii;
            if (QFile::exists(filepath)) {
                if (!QFile::remove(filepath)) {
                    qWarning() << "Unable to remove file " << filepath;
                }
            }
            ++iii;
            QThread::msleep(10);
        }

        db->removeEntriesByStream(*ii, entriesLimit);

        ++ii;
    }
}

CacheRemover::CacheRemover(QObject *parent) : QThread(parent) {
    total = 100;
    current = 0;
    doCancel = false;
}

/*
 * Copyright (c) 2009 John Schember <john@nachtimwald.com>
 * http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
 */
bool CacheRemover::removeDir(const QString &dirName) {
    bool result = true;
    QDir dir{dirName};

    emit progressChanged(0, total);

    if (dir.exists()) {
        auto infoList =
            dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                  QDir::Hidden | QDir::AllDirs | QDir::Files,
                              QDir::DirsFirst);
        total = infoList.count();
        foreach (const QFileInfo &info, infoList) {
            if (doCancel) return result;
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
                ++current;
                if (current % 10 == 0) emit progressChanged(++current, total);
            }

            if (!result) return result;
        }
        result = dir.rmdir(dirName);
    }

    emit progressChanged(total, total);

    return result;
}

void CacheRemover::run() {
    current = 0;
    total = 100;
    doCancel = false;
    Settings *s = Settings::instance();
    if (!removeDir(s->getDmCacheDir())) {
        qWarning() << "Unable to remove " << s->getDmCacheDir();
    }

    Utils::resetWebViewStatic();
}

void CacheRemover::cancel() { doCancel = true; }

DownloadAdder::DownloadAdder(QObject *parent) : QThread(parent) {}

void DownloadAdder::run() {
    auto list = DatabaseManager::instance()->readNotCachedEntries();
    if (list.isEmpty()) {
        qWarning() << "No feeds to download";
        return;
    }

    auto i = list.begin();
    while (i != list.end()) {
        if (!i.key().isEmpty() && !i.value().isEmpty()) {
            DatabaseManager::CacheItem item;
            item.entryId = i.key();
            item.origUrl = i.value();
            item.finalUrl = i.value();
            emit addDownload(item);
        }
        ++i;
    }

    emit addingFinished(list.size());
}

CacheDeterminer::CacheDeterminer(QObject *parent) : QThread(parent) {}

void CacheDeterminer::run() {
    int size = 0;
    QDirIterator i{Settings::instance()->getDmCacheDir()};
    while (i.hasNext()) {
        if (i.fileInfo().isFile()) size += i.fileInfo().size();
        i.next();
    }
    emit cacheDetermined(size);
}

void DownloadManager::addingFinishedHandler(int count) {
    Q_UNUSED(count)

    this->m_downloadTotal = (m_downloads.count() + m_queue.count());
}
