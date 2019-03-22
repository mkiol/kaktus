/*
  Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>

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

#include <QNetworkRequest>
#include <QDateTime>
#include <QByteArray>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QImageWriter>
#include <QImage>
#include <memory>
#else
#include "parser.h"
#endif

#include "fetcher.h"
#include "settings.h"
#include "databasemanager.h"
#include "downloadmanager.h"
#include "cacheserver.h"
#include "utils.h"

FetcherCookieJar::FetcherCookieJar(QObject *parent) :
    QNetworkCookieJar(parent)
{}

bool FetcherCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> & cookieList, const QUrl & url)
{
    Q_UNUSED(cookieList)
    Q_UNUSED(url)
    return false;
}

Fetcher::Fetcher(QObject *parent) :
    QThread(parent),
    currentReply(NULL),
    busyType(Fetcher::UnknownBusyType),
    busy(false)
{
    auto dm = DownloadManager::instance();

    nam.setCookieJar(new FetcherCookieJar(this));
    connect(&nam, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
    connect(this, SIGNAL(addDownload(DatabaseManager::CacheItem)),
            dm, SLOT(addDownload(DatabaseManager::CacheItem)));
    connect(this, SIGNAL(busyChanged()), dm, SLOT(startDownload()));
}

Fetcher::~Fetcher()
{
    if (currentReply != NULL) {
        currentReply->disconnect(this);
        currentReply->deleteLater();
        currentReply = NULL;
    }
}

bool Fetcher::isBusy()
{
    return busy;
}

Fetcher::BusyType Fetcher::readBusyType()
{
    return busyType;
}

void Fetcher::setBusy(bool busy, Fetcher::BusyType type)
{
    this->busyType = type;
    this->busy = busy;

    if (!busy)
        this->busyType = Fetcher::UnknownBusyType;

    emit busyChanged();
}

bool Fetcher::init()
{
    if (busy) {
        qWarning() << "Fetcher is busy";
        return false;
    }

#ifdef ONLINE_CHECK
    if (!ncm.isOnline()) {
        qDebug() << "Network is Offline. Waiting...";
        setBusy(true, Fetcher::InitiatingWaiting);
        connect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));
        return true;
    }
#endif

    setBusy(true, Fetcher::Initiating);
    emit progress(0,100);
    signIn();
    return true;
}

bool Fetcher::update()
{
    if (busy) {
        qWarning() << "Fetcher is busy";
        return false;
    }

    auto db = DatabaseManager::instance();
    int streamsCount = db->countStreams();
    int entriesCount = db->countEntries();
    int tabsCount = db->countTabs();

#ifdef ONLINE_CHECK
    if (!ncm.isOnline()) {
        qDebug() << "Network is Offline. Waiting...";
        if (streamsCount == 0 || entriesCount == 0 || tabsCount == 0) {
            setBusy(true, Fetcher::InitiatingWaiting);
        } else {
            setBusy(true, Fetcher::UpdatingWaiting);
        }
        connect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));
        return true;
    }
#endif

    if (streamsCount == 0 || entriesCount == 0 || tabsCount == 0) {
        setBusy(true, Fetcher::Initiating);
    } else {
        setBusy(true, Fetcher::Updating);
    }

    emit progress(0,100);
    signIn();
    return true;
}

void Fetcher::cancel()
{
    if (busyType == Fetcher::UpdatingWaiting ||
        busyType == Fetcher::InitiatingWaiting ||
        busyType == Fetcher::CheckingCredentialsWaiting) {
        setBusy(false);
    } else {

        // Restoring backup
        auto db = DatabaseManager::instance();
        if (!db->restoreBackup()) {
            qWarning() << "Unable to restore DB backup";
        }

        if (currentReply != NULL)
            currentReply->close();
        else
            setBusy(false);
    }
}

bool Fetcher::checkCredentials()
{
    if (busy) {
        qWarning() << "Fetcher is busy";
        return false;
    }

#ifdef ONLINE_CHECK
    if (!ncm.isOnline()) {
        //qDebug() << "Network is Offline. Waiting...";
        setBusy(true, Fetcher::CheckingCredentialsWaiting);
        connect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));
        return true;
    }
#endif

    setBusy(true, Fetcher::CheckingCredentials);
    return true;
}

void Fetcher::readyRead()
{
    int statusCode = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << "readyRead, statusCode=" << statusCode;
    if (statusCode >= 200 && statusCode < 300) {
        data += currentReply->readAll();
    }
}

void Fetcher::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (busy) {
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

bool Fetcher::delayedUpdate(bool state)
{
    disconnect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));

#ifdef ONLINE_CHECK
    if (!state) {
        qWarning() << "Network is Offline";
        emit networkNotAccessible();
        setBusy(false);
        return false;
    }
#endif

    switch (busyType) {
    case Fetcher::InitiatingWaiting:
        setBusy(true, Fetcher::Initiating);
        break;
    case Fetcher::UpdatingWaiting:
        setBusy(true, Fetcher::Updating);
        break;
    case Fetcher::CheckingCredentialsWaiting:
        setBusy(true, Fetcher::CheckingCredentials);
        break;
    default:
        qWarning() << "Wrong busy state";
        setBusy(false);
        return false;
    }

    return true;
}

void Fetcher::networkError(QNetworkReply::NetworkError e)
{
    if (e == QNetworkReply::OperationCanceledError) {
        if (currentReply != NULL) {
            currentReply->disconnect(this);
            currentReply->deleteLater();
            currentReply = NULL;
        }
        emit canceled();
        data.clear();
        setBusy(false);
    } else {
        int code = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QByteArray phrase = currentReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
        //emit error(500);
        qWarning() << "Network error!" << "Url:" << currentReply->url().toString() << "Error code:" << e
                   << "HTTP code:" << code << phrase << "Content:" << currentReply->readAll();
    }
}

bool Fetcher::parse()
{
    //qint64 date1 = QDateTime::currentMSecsSinceEpoch();
    //qDebug() << "parse:" << data;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isObject()) {
        jsonObj = doc.object();
        return true;
    }

    if (doc.isArray()) {
        jsonArr = doc.array();
        return true;
    }
#else
    QJson::Parser qjson;
    bool ok;
    QVariant result = qjson.parse(data, &ok);
    if (!ok) {
        qWarning() << "An error occurred during parsing Json";
        return false;
    }

    if (result.type()==QVariant::Map) {
        jsonObj = result.toMap();
        return true;
    }

    if (result.type()==QVariant::List) {
        jsonArr = result.toList();
        return true;
    }
#endif
    //qDebug() << "parse time:" << (QDateTime::currentMSecsSinceEpoch() - date1);
    qWarning() << "Json doc is empty";
    return false;
}

void Fetcher::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    for (const QSslError &error : sslErrors)
        qWarning() << "SSL error: " << error.errorString();
    if (Settings::instance()->getIgnoreSslErrors()) {
        qDebug() << "Ignoring SSL errors";
        auto reply = dynamic_cast<QNetworkReply*>(sender());
        reply->ignoreSslErrors();
    }
#else
    Q_UNUSED(sslErrors);
#endif
}

void Fetcher::mergeActionsIntoList(DatabaseManager::ActionsTypes typeSet,
                                   DatabaseManager::ActionsTypes typeUnset,
                                   DatabaseManager::ActionsTypes typeSetList,
                                   DatabaseManager::ActionsTypes typeUnsetList)
{
    auto db = DatabaseManager::instance();

    QList<DatabaseManager::Action>::iterator it1 = actionsList.begin();
    QList<DatabaseManager::Action>::iterator it2 = it1 + 1;
    if (it2 != actionsList.end()) {
        bool inMerge = false;
        while (it2 != actionsList.end()) {

            DatabaseManager::ActionsTypes type1 = (*it1).type;
            DatabaseManager::ActionsTypes type2 = (*it2).type;
            DatabaseManager::ActionsTypes newType;

            bool typeOk = false;
            if (type1 == typeSet) {
                newType = typeSetList;
                typeOk = true;
            } else if (type1 == typeUnset) {
                newType = typeUnsetList;
                typeOk = true;
            }

            if (type1 == type2 && typeOk) {

                QString newId1 = (*it1).id1 + QString("&%1").arg((*it2).id1);
                QString newId2 = (*it1).id2 + QString("&%1").arg((*it2).id2);
                QString newId3 = (*it1).id3 + QString("&%1").arg((*it2).id3);
                db->updateActionByIdAndType((*it1).id1, type1, newId1, newId2, newId3, type1);
                (*it1).id1 = newId1;
                (*it1).id2 = newId2;
                (*it1).id3 = newId3;
                db->removeActionsByIdAndType((*it2).id1, type2);
                it2 = actionsList.erase(it2);
                inMerge = true;

                if (it2 == actionsList.end()) {
                    db->updateActionByIdAndType((*it1).id1, type1, (*it1).id1, (*it1).id2, (*it1).id3, newType);
                    (*it1).type = newType;
                    inMerge = false;
                }
            } else {
                if (inMerge) {
                    db->updateActionByIdAndType((*it1).id1, type1, (*it1).id1, (*it1).id2, (*it1).id3, newType);
                    (*it1).type = newType;
                    inMerge = false;
                }

                it1 = it2;
                ++it2;
            }
        }
    }
}

void Fetcher::prepareUploadActions()
{
    // upload actions
    auto db = DatabaseManager::instance();
    actionsList = db->readActions();
    if (actionsList.isEmpty()) {
        //qDebug() << "No actions to upload";
        startFetching();
    } else {

        //qDebug() << actionsList.count() << " actions to upload";

        // Actions optimization
        // 1. Merging series of SetRead into SetListRead
        mergeActionsIntoList(DatabaseManager::SetRead, DatabaseManager::UnSetRead,
                             DatabaseManager::SetListRead, DatabaseManager::UnSetListRead);
        // 2. Merging series of SetSaved into SetListSaved
        /*mergeActionsIntoList(DatabaseManager::SetSaved, DatabaseManager::UnSetSaved,
                             DatabaseManager::SetListSaved, DatabaseManager::UnSetListSaved);*/

        //qDebug() << actionsList.count() << " actions to upload after opt";

        this->uploadProggressTotal = actionsList.size();
        uploadActions();
    }
}

void Fetcher::taskEnd()
{
    //qDebug() << "taskEnd";
    emit progress(proggressTotal, proggressTotal);

    if (currentReply != NULL) {
        currentReply->disconnect(this);
        currentReply->deleteLater();
        currentReply = NULL;
    }

    Settings *s = Settings::instance();
    s->setLastUpdateDate(QDateTime::currentDateTimeUtc().toTime_t());

    data.clear();

    emit ready();
    setBusy(false);
}

void Fetcher::copyImage(const QString &path, const QString &contentType)
{
    QString dpath = QDir(Settings::instance()->getImagesDir())
                         .absoluteFilePath("kaktus_" + QFileInfo(path).fileName());

    Utils::addExtension(contentType, dpath);

    if (QFile::exists(dpath)) {
        emit error(801); // image already exists
        return;
    }

    if (contentType == "image/jpeg") {
        qWarning() << "JPEG file, so removing EXIF metadata";
        QImageReader ir(path);
        QImage img = ir.read();
        if (img.isNull()) {
            qWarning() << "Cannot read image:" << path << ir.errorString();
        } else {
            QImageWriter iw(dpath);
            iw.setFormat(ir.format());
            if (iw.write(img)) {
                qDebug() << "Image saved successfully";
                emit imageSaved(QFileInfo(dpath).fileName());
                return;
            } else {
                qWarning() << "Cannot write image:" << dpath << iw.errorString();
            }
        }
    } else if (QFile::copy(path, dpath)) {
        qDebug() << "Image saved successfully";
        emit imageSaved(QFileInfo(dpath).fileName());
        return;
    }

    emit error(800); // image save error
}

void Fetcher::saveImage(const QString &url)
{
    //qDebug() << "saveImage url:" << url;

    QString path, contentType;
    if (CacheServer::getPathAndContentTypeByUrl(url, path, contentType)) {
        qDebug() << "Image already in cache";
        copyImage(path, contentType);
    } else {
        qDebug() << "Image not cached, so downloading";
        DatabaseManager::CacheItem item;
        item.origUrl = url;
        item.finalUrl = url;
        item.type = "entry-image";
        emit addDownload(item);

        auto dm = DownloadManager::instance();
        auto conn1 = std::make_shared<QMetaObject::Connection>();
        auto conn2 = std::make_shared<QMetaObject::Connection>();
        *conn1 = connect(dm, &DownloadManager::downloadReady,
                        [this, conn1, conn2](const QString &url,
                         const QString &path, const QString &contentType) {
            //qDebug() << "Download finished:" << url << path << contentType;
            Q_UNUSED(url);
            disconnect(*conn1); disconnect(*conn2);
            copyImage(path, contentType);
        });
        *conn2 = connect(dm, &DownloadManager::downloadFailed,
                        [this, conn1, conn2](const QString &url) {
            qWarning() << "Image download error:" << url;
            disconnect(*conn1); disconnect(*conn2);
            emit error(800); // image save error
        });
    }
}
