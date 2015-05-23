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
#else
#include "qjson.h"
#endif

#include "fetcher.h"
#include "settings.h"
#include "databasemanager.h"
#include "downloadmanager.h"

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
    Settings *s = Settings::instance();
    nam.setCookieJar(new FetcherCookieJar(this));
    connect(&nam, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
    connect(this, SIGNAL(addDownload(DatabaseManager::CacheItem)),
            s->dm, SLOT(addDownload(DatabaseManager::CacheItem)));
    connect(this, SIGNAL(busyChanged()), s->dm, SLOT(startDownload()));
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
        qWarning() << "Fetcher is busy!";
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

    //TODO ....

    signIn();
    emit progress(0,100);
    return true;
}

bool Fetcher::update()
{
    if (busy) {
        qWarning() << "Fetcher is busy!";
        return false;
    }

    Settings *s = Settings::instance();
    int streamsCount =s->db->countStreams();
    int entriesCount =s->db->countEntries();
    int tabsCount =s->db->countTabs();

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

    //TODO ....

    signIn();
    emit progress(0,100);
    return true;
}

void Fetcher::cancel()
{
    //disconnect(ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));
    if (busyType == Fetcher::UpdatingWaiting ||
        busyType == Fetcher::InitiatingWaiting ||
        busyType == Fetcher::CheckingCredentialsWaiting) {
        setBusy(false);
    } else {
        if (currentReply != NULL)
            currentReply->close();
        else
            setBusy(false);
    }
}

bool Fetcher::checkCredentials()
{
    if (busy) {
        qWarning() << "Fetcher is busy!";
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

    //TODO ...

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
            qWarning() << "Network is not accessible!";
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
        qWarning() << "Network is Offline!";
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
        qWarning() << "Wrong busy state!";
        setBusy(false);
        return false;
    }

    //TODO .....

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
        qWarning() << "Network error!, error code:" << e << ", HTTP code:" << code << phrase << currentReply->readAll();
    }
}

bool Fetcher::parse()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Json doc is empty!";
        return false;
    }
    jsonObj = doc.object();
#else
    QJson qjson(this);
    bool ok;
    jsonObj = qjson.parse(data, &ok).toMap();
    if (!ok) {
        qWarning() << "An error occurred during parsing Json!";
        return false;
    }
    if (jsonObj.empty()) {
        qWarning() << "Json doc is empty!";
        return false;
    }
#endif
    return true;
}

void Fetcher::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, sslErrors)
        qWarning() << "SSL error: " << error.errorString();
#else
    Q_UNUSED(sslErrors);
#endif
}

void Fetcher::prepareUploadActions()
{
    // upload actions
    Settings *s = Settings::instance();
    actionsList = s->db->readActions();
    if (actionsList.isEmpty()) {
        //qDebug() << "No actions to upload!";
        startFetching();
    } else {
        //qDebug() << actionsList.count() << " actions to upload!";
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

