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

#include <QNetworkReply>
#include <QRegExp>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QList>
#include <QStringList>
#include <QDateTime>
#include <math.h>
#else
#include "parser.h"
#endif

#include "oldreaderfetcher.h"
#include "settings.h"
#include "downloadmanager.h"
#include "utils.h"

OldReaderFetcher::OldReaderFetcher(QObject *parent) :
    Fetcher(parent),
    currentJob(Idle)
{
}

void OldReaderFetcher::signIn()
{
    data.clear();

    Settings *s = Settings::instance();

    // Check is already have cookie
    if (s->getCookie() != "") {
        prepareUploadActions();
        return;
    }

    QString password = s->getPassword();
    QString username = s->getUsername();
    int type = s->getSigninType();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QString body;
    QNetworkRequest request;

    switch (type) {
    case 10:
        if (password == "" || username == "") {
            qWarning() << "Username & password do not match!";
            if (busyType == Fetcher::CheckingCredentials)
                emit errorCheckingCredentials(400);
            else
                emit error(400);
            setBusy(false);
            return;
        }

        request.setUrl(QUrl("https://theoldreader.com/accounts/ClientLogin"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
        body = "output=json&client=Kaktus&accountType=HOSTED_OR_GOOGLE&service=reader&Email="+
                QUrl::toPercentEncoding(username)+"&Passwd="+
                QUrl::toPercentEncoding(password);
        currentReply = nam.post(request,body.toUtf8());
        break;
    default:
        qWarning() << "Invalid sign in type!";
        emit error(500);
        setBusy(false);
        return;
    }

#ifndef QT_NO_SSL
    connect(currentReply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    if (busyType == Fetcher::CheckingCredentials)
        connect(currentReply, SIGNAL(finished()), this, SLOT(finishedSignInOnlyCheck()));
    else
        connect(currentReply, SIGNAL(finished()), this, SLOT(finishedSignIn()));

    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

QString OldReaderFetcher::getIdsFromActionString(const QString &actionString)
{
    QString ids;
    QStringList idList = actionString.split("&");
    QStringList::iterator i = idList.begin();
    while (i != idList.end()) {
        ids += QString("i=%1&").arg(*i);
        ++i;
    }
    ids.remove(ids.length()-1,1);
    //qDebug() << "ids:" << ids;
    return ids;
}

void OldReaderFetcher::setAction()
{
    data.clear();

    DatabaseManager::Action action = actionsList.first();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url;
    QString body;
    switch (action.type) {
    case DatabaseManager::SetRead:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        body = QString("a=user/-/state/com.google/read&i=%1").arg(action.id1);
        break;
    case DatabaseManager::UnSetRead:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        body = QString("r=user/-/state/com.google/read&i=%1").arg(action.id1);
        break;
    case DatabaseManager::SetLiked:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        body = QString("a=user/-/state/com.google/like&i=%1").arg(action.id1);
        break;
    case DatabaseManager::UnSetLiked:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        body = QString("r=user/-/state/com.google/like&i=%1").arg(action.id1);
        break;
    case DatabaseManager::SetListRead:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        body = QString("a=user/-/state/com.google/read&%1").arg(getIdsFromActionString(action.id1));
        break;
    case DatabaseManager::SetSaved:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        body = QString("a=user/-/state/com.google/starred&i=%1").arg(action.id1);
        break;
    case DatabaseManager::UnSetSaved:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        body = QString("r=user/-/state/com.google/starred&i=%1").arg(action.id1);
        break;
    case DatabaseManager::SetStreamReadAll:
        url.setUrl("https://theoldreader.com/reader/api/0/mark-all-as-read");
        body = QString("s=%1&ts=%2").arg(action.id1).arg(
                    QString::number(s->db->readLastLastUpdateByStream(action.id1))+"000000"
                    );
        break;
    case DatabaseManager::SetTabReadAll:
        if (action.id1 == "subscriptions") {
            // Adding SetStreamReadAll action for every stream in substriptions folder
            QStringList list = s->db->readStreamIdsByTab("subscriptions");
            QStringList::iterator it = list.begin();
            while (it != list.end()) {
                DatabaseManager::Action action;
                action.type = DatabaseManager::SetStreamReadAll;
                action.id1 = *it;
                actionsList.insert(1,action);
                ++it;
            }

            finishedSetAction();
            return;
        }

        url.setUrl("https://theoldreader.com/reader/api/0/mark-all-as-read");
        body = QString("s=%1&ts=%2").arg(action.id1).arg(
                    QString::number(s->db->readLastLastUpdateByTab(action.id1))+"000000"
                    );
        break;
    case DatabaseManager::SetAllRead:
        url.setUrl("https://theoldreader.com/reader/api/0/mark-all-as-read");
        body = QString("s=user/-/state/com.google/reading-list&ts=%1").arg(
                    QString::number(s->db->readLastLastUpdateByDashboard(s->getDashboardInUse()))+"000000"
                    );
        break;
    case DatabaseManager::SetBroadcast:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        if (action.text == "")
            body = QString("a=user/-/state/com.google/broadcast&i=%1").arg(action.id1);
        else
            body = QString("a=user/-/state/com.google/broadcast&i=%1&annotation=%2").arg(action.id1).arg(action.text);
        break;
    case DatabaseManager::UnSetBroadcast:
        url.setUrl("https://theoldreader.com/reader/api/0/edit-tag");
        body = QString("r=user/-/state/com.google/broadcast&i=%1").arg(action.id1);
        break;
    default:
        // Unknown action -> skiping
        qWarning("Unknown action!");
        finishedSetAction();
        return;
    }

    QNetworkRequest request(url);

    // Headers
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());
    request.setRawHeader("Content-Encoding", "gzip");

    currentReply = nam.post(request,body.toUtf8());

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedSetAction()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void OldReaderFetcher::fetchFriends()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url("https://theoldreader.com/reader/api/0/friend/list?output=json");
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedFriends()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}


void OldReaderFetcher::fetchTabs()
{
    //qDebug() << "fetchTabs";
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url("https://theoldreader.com/reader/api/0/tag/list?output=json");
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedTabs()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void OldReaderFetcher::fetchFeeds()
{
    //qDebug() << "fetchFeeds";
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url("https://theoldreader.com/reader/api/0/subscription/list?output=json");
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedFeeds()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void OldReaderFetcher::fetchStream()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QString st(QUrl::toPercentEncoding("user/-/state/com.google/reading-list"));
    QString xst(QUrl::toPercentEncoding("user/-/state/com.google/read"));
#else
    QString st = "user/-/state/com.google/reading-list";
    QString xst = "user/-/state/com.google/read";
#endif
    int epoch = s->getRetentionDays() > 0 ?
                QDateTime::currentDateTimeUtc().addDays(0-s->getRetentionDays()).toTime_t() :
                0;

    QString surl = QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&s=%2").arg(limitAtOnce).arg(st);
    if (lastContinuation != "")
        surl += QString("&c=%1").arg(lastContinuation);
    if (epoch > 0)
        surl += QString("&ot=%1").arg(epoch);
    if (!s->getSyncRead())
        surl += QString("&xt=%1").arg(xst);

    /*if (lastContinuation == "") {
        if (epoch > 0)
            url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&ot=%2&s=%3")
                       .arg(limitAtOnce).arg(epoch).arg(st));
        else
            url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&s=%2")
                       .arg(limitAtOnce).arg(st));
    } else {
        if (epoch > 0)
            url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&c=%2&ot=%3&s=%4")
                       .arg(limitAtOnce).arg(lastContinuation).arg(epoch).arg(st));
        else
            url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&c=%2&s=%3")
                       .arg(limitAtOnce).arg(lastContinuation).arg(st));
    }*/

    QUrl url(surl);
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());

    //qDebug() << surl;

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedStream()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void OldReaderFetcher::fetchStarredStream()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QString st(QUrl::toPercentEncoding("user/-/state/com.google/starred"));
#else
    QString st = "user/-/state/com.google/starred";
#endif
    if (lastContinuation == "")
        url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&s=%2")
                   .arg(limitAtOnce).arg(st));
    else
        url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&c=%2&s=%3")
                   .arg(limitAtOnce).arg(lastContinuation).arg(st));
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedStarredStream()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void OldReaderFetcher::fetchLikedStream()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QString st(QUrl::toPercentEncoding("user/-/state/com.google/like"));
#else
    QString st = "user/-/state/com.google/liked";
#endif
    if (lastContinuation == "")
        url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&s=%2")
                   .arg(limitAtOnce).arg(st));
    else
        url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&c=%2&s=%3")
                   .arg(limitAtOnce).arg(lastContinuation).arg(st));
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedLikedStream()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void OldReaderFetcher::fetchBroadcastStream()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QString st(QUrl::toPercentEncoding("user/-/state/com.google/broadcast"));
#else
    QString st = "user/-/state/com.google/broadcast";
#endif
    if (lastContinuation == "")
        url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&s=%2")
                   .arg(limitAtOnce).arg(st));
    else
        url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&c=%2&s=%3")
                   .arg(limitAtOnce).arg(lastContinuation).arg(st));
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedBroadcastStream()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void OldReaderFetcher::fetchUnreadStream()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QString st1(QUrl::toPercentEncoding("user/-/state/com.google/reading-list"));
    QString st2(QUrl::toPercentEncoding("user/-/state/com.google/read"));
#else
    QString st1 = "user/-/state/com.google/reading-list";
    QString st2 = "user/-/state/com.google/read";
#endif
    if (lastContinuation == "")
        url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&s=%2&xt=%3")
                   .arg(limitAtOnce).arg(st1, st2));
    else
        url.setUrl(QString("https://theoldreader.com/reader/api/0/stream/contents?output=json&n=%1&c=%2&s=%3&xt=%4")
                   .arg(limitAtOnce)
                   .arg(lastContinuation).arg(st1, st2));
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("GoogleLogin auth=%1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedUnreadStream()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void OldReaderFetcher::finishedSignInOnlyCheck()
{
    //qDebug() << data;
    Settings *s = Settings::instance();

    if (currentReply->error() &&
        currentReply->error()!=QNetworkReply::OperationCanceledError) {
        int code = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // 403 Forbidden = Invalid username or password
        if (code == 403) {
            emit errorCheckingCredentials(402);
            setBusy(false);
            qWarning() << "Sign in check failed! Invalid username or password.";
            return;
        }

        qWarning() << "Sign in failed!";
        emit errorCheckingCredentials(501);
        setBusy(false);
        return;
    }

    switch (s->getSigninType()) {
    case 10:
        if (parse()) {
            QString auth = jsonObj["Auth"].toString();
            if (auth != "") {
                s->setSignedIn(true);
                s->setCookie(auth);
                emit credentialsValid();
                setBusy(false);
            } else {
                s->setSignedIn(false);
                qWarning() << "Sign in check failed! Can not find Auth param.";
                emit errorCheckingCredentials(501);
                setBusy(false);
            }
        } else {
            s->setSignedIn(false);
            qWarning() << "Sign in check failed! Error while parsing JSON";
            emit errorCheckingCredentials(501);
            setBusy(false);
        }
        break;
    default:
        qWarning() << "Invalid sign in type!";
        emit errorCheckingCredentials(502);
        setBusy(false);
        s->setSignedIn(false);
    }
}

void OldReaderFetcher::finishedSignIn()
{
    //qDebug() << data;
    Settings *s = Settings::instance();

    if (currentReply->error() &&
        currentReply->error()!=QNetworkReply::OperationCanceledError) {
        int code = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        // 403 Forbidden = Invalid username or password
        if (code == 403) {
            emit error(402);
            setBusy(false);
            qWarning() << "Sign in failed! Invalid username or password.";
            return;
        }

        qWarning() << "Sign in failed!";
        emit error(501);
        setBusy(false);
        return;
    }

    switch (s->getSigninType()) {
    case 10:
        if (parse()) {
            QString auth = jsonObj["Auth"].toString();
            if (auth != "") {
                s->setSignedIn(true);
                s->setCookie(auth);
                prepareUploadActions();
            } else {
                s->setSignedIn(false);
                qWarning() << "Sign in failed! Can not find Auth param.";
                emit error(501);
                setBusy(false);
            }
        } else {
            s->setSignedIn(false);
            qWarning() << "Sign in failed! Error while parsing JSON";
            emit error(501);
            setBusy(false);
        }
        break;
    default:
        qWarning() << "Invalid sign in type!";
        emit error(502);
        setBusy(false);
        s->setSignedIn(false);
    }
}

void OldReaderFetcher::finishedTabs()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    Settings *s = Settings::instance();
    s->db->cleanTabs();
    startJob(StoreTabs);
}

void OldReaderFetcher::finishedTabs2()
{
    Settings *s = Settings::instance();

    lastContinuation = "";
    continuationCount = 0;

    if (tabList.isEmpty()) {
        qWarning() << "No Tabs to download!";
        if (busyType == Fetcher::Initiating)
            s->db->cleanEntries();

        // Proggres initiating
        proggressTotal = 2;
        proggress = 1;
        emit progress(proggress, proggressTotal);

        s->db->cleanStreams();
        s->db->cleanModules();
        fetchStarredStream();
        return;
    }

    fetchFriends();
}

void OldReaderFetcher::finishedFriends()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    Settings *s = Settings::instance();
    s->db->cleanStreams();
    s->db->cleanModules();
    startJob(StoreFriends);
}

void OldReaderFetcher::finishedFriends2()
{
    fetchFeeds();
}

void OldReaderFetcher::finishedFeeds()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreFeeds);
}

void OldReaderFetcher::finishedFeeds2()
{
    // Proggres initiating, one step is one day
    Settings *s = Settings::instance();
    proggressTotal = s->getRetentionDays() > 0 ? log(s->getRetentionDays()) + 4 : 5;
    proggress = 1;
    lastDate = 0;
    //qDebug() << "finishedFeeds2" << "proggress" << proggress << "log(s->getRetentionDays())" << log(s->getRetentionDays()) << "proggressTotal" << proggressTotal;
    //emit progress(proggress, proggressTotal);

    /*if (busyType == Fetcher::Updating)
        removeDeletedFeeds();*/

    s->db->updateEntriesFlag(1); // Marking as old

    fetchStream();
}

void OldReaderFetcher::finishedStream()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreStream);
}

void OldReaderFetcher::finishedStream2()
{
    Settings *s = Settings::instance();
    if (s->getRetentionDays() > 0) {
        if (lastDate > s->getRetentionDays())
            lastDate = s->getRetentionDays();
        //qDebug() << "finishedStream2" << "proggress" << proggress << "log(lastDate)" << log(lastDate) << "proggressTotal" << proggressTotal;
        emit progress(proggress + log(lastDate), proggressTotal);
    }

    if (lastContinuation == "" ||
        continuationCount > continuationLimit) {

        proggress += s->getRetentionDays() > 0 ? log(lastDate) : 1;
        //qDebug() << "finishedStream2" << "proggress" << proggress;


        lastContinuation = "";
        continuationCount = 0;
        lastDate = 0;

        fetchStarredStream();
        return;
    }

    fetchStream();
}

void OldReaderFetcher::finishedStarredStream()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreStarredStream);
}

void OldReaderFetcher::finishedStarredStream2()
{
    if (lastContinuation == "" ||
        continuationCount > continuationLimit) {

        ++proggress;
        //qDebug() << "finishedStarredStream2" << "proggress" << proggress;
        emit progress(proggress, proggressTotal);

        fetchLikedStream();
        return;
    }

    fetchStarredStream();
}

void OldReaderFetcher::finishedLikedStream()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreLikedStream);
}

void OldReaderFetcher::finishedLikedStream2()
{
    if (lastContinuation == "" ||
        continuationCount > continuationLimit) {

        ++proggress;
        //qDebug() << "finishedLikedStream2" << "proggress" << proggress;
        emit progress(proggress, proggressTotal);

        fetchBroadcastStream();
        return;
    }

    fetchLikedStream();
}

void OldReaderFetcher::finishedBroadcastStream()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreBroadcastStream);
}

void OldReaderFetcher::finishedBroadcastStream2()
{
    if (lastContinuation == "" ||
        continuationCount > continuationLimit) {

        ++proggress;
        //qDebug() << "finishedBroadcastStream2" << "proggress" << proggress;
        emit progress(proggress, proggressTotal);

        //startJob(MarkSlow);
        finishedMarkSlow();
        return;
    }

    fetchBroadcastStream();
}

void OldReaderFetcher::finishedUnreadStream()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreUnreadStream);
}

void OldReaderFetcher::finishedUnreadStream2()
{
    if (lastContinuation == "" ||
        continuationCount > continuationLimit) {
        taskEnd();
        return;
    }

    fetchUnreadStream();
}

void OldReaderFetcher::finishedSetAction()
{
    //qDebug() << data;
    if (currentReply != NULL && currentReply->error()) {
        int code = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (code == 404) {
            // Probably item already deleted -> skiping
            qWarning() << "Action request returns 404!";
        } else {
            emit error(500);
            setBusy(false);
            return;
        }
    }

    Settings *s = Settings::instance();

    // Deleting action
    DatabaseManager::Action action = actionsList.takeFirst();
    s->db->removeActionsByIdAndType(action.id1, action.type);

    if (actionsList.isEmpty()) {
        s->db->cleanDashboards();
        startFetching();
        return;
    }

    setAction();
}

void OldReaderFetcher::finishedMarkSlow()
{
    // Deleting old entries
    Settings *s = Settings::instance();
    s->db->removeEntriesByFlag(1);

    taskEnd();
    return;
}

bool OldReaderFetcher::setConnectUrl(const QString &url)
{
    Q_UNUSED(url);
    return false;
}

void OldReaderFetcher::getConnectUrl(int type)
{
    Q_UNUSED(type)
    if (busy) {
        qWarning() << "Fetcher is busy!";
        return;
    }

    //TODO ...
}

void OldReaderFetcher::startFetching()
{
    Settings *s = Settings::instance();

    // Create DB structure
    s->db->cleanDashboards();
    if(busyType == Fetcher::Initiating) {
        s->db->cleanCache();
        s->db->cleanEntries();
    }

    if (busyType == Fetcher::Updating) {
        s->db->updateEntriesFreshFlag(0); // Set current entries as not fresh
    }

    // Old Reader API doesnt have Dashboards
    // Manually adding dummy Dashboard
    DatabaseManager::Dashboard d;
    d.id = "oldreader";
    d.name = "Default";
    d.title = "Default";
    d.description = "Old Reader default dashboard";
    s->db->writeDashboard(d);
    s->setDashboardInUse(d.id);

    fetchTabs();
}

void OldReaderFetcher::startJob(Job job)
{
    if (isRunning()) {
        qWarning() << "Job is running";
        return;
    }

    disconnect(this, SIGNAL(finished()), 0, 0);
    currentJob = job;

    if (parse()) {
        /*qWarning() << "Cookie expires!";
        Settings *s = Settings::instance();
        s->setCookie("");
        setBusy(false);

        // If credentials other than OldReader, prompting for re-auth
        if (s->getSigninType()>10) {
            emit error(403);
            return;
        }

        update();
        return;*/
    } else {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    switch (job) {
    case StoreTabs:
        connect(this, SIGNAL(finished()), this, SLOT(finishedTabs2()));
        break;
    case StoreFriends:
        connect(this, SIGNAL(finished()), this, SLOT(finishedFriends2()));
        break;
    case StoreFeeds:
        connect(this, SIGNAL(finished()), this, SLOT(finishedFeeds2()));
        break;
    case StoreStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedStream2()));
        break;
    case StoreUnreadStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedUnreadStream2()));
        break;
    case StoreStarredStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedStarredStream2()));
        break;
    case StoreLikedStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedLikedStream2()));
        break;
    case StoreBroadcastStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedBroadcastStream2()));
        break;
    case MarkSlow:
        connect(this, SIGNAL(finished()), this, SLOT(finishedMarkSlow()));
        break;
    default:
        qWarning() << "Unknown Job!";
        emit error(502);
        setBusy(false);
        return;
    }

    start(QThread::LowPriority);
}

void OldReaderFetcher::run()
{
    switch (currentJob) {
    case StoreTabs:
        storeTabs();
        break;
    case StoreFriends:
        storeFriends();
        break;
    case StoreFeeds:
        storeFeeds();
        break;
    case StoreStream:
    case StoreUnreadStream:
    case StoreStarredStream:
    case StoreLikedStream:
    case StoreBroadcastStream:
        storeStream();
        break;
    case MarkSlow:
        markSlowFeeds();
        break;
    default:
        qWarning() << "Unknown Job!";
        break;
    }
}

void OldReaderFetcher::storeTabs()
{
    Settings *s = Settings::instance();
    QString dashboardId = "oldreader";

    // Adding Subscriptions folder
    DatabaseManager::Tab t;
    t.id = "subscriptions";
    t.dashboardId = dashboardId;
    t.title = "Subscriptions";
    s->db->writeTab(t);
    tabList.append(t.id);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (jsonObj["tags"].isArray()) {
        QJsonArray::const_iterator i = jsonObj["tags"].toArray().constBegin();
        QJsonArray::const_iterator end = jsonObj["tags"].toArray().constEnd();
#else
    if (jsonObj["tags"].type()==QVariant::List) {
        QVariantList::const_iterator i = jsonObj["tags"].toList().constBegin();
        QVariantList::const_iterator end = jsonObj["tags"].toList().constEnd();
#endif
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = (*i).toObject();
#else
            QVariantMap obj = (*i).toMap();
#endif
            // Checking if folder (label)
            QStringList id = obj["id"].toString().split('/');
            //qDebug() << id;
            if (id.at(2)=="label") {
                // Tab
                DatabaseManager::Tab t;
                t.id = obj["id"].toString();
                t.dashboardId = dashboardId;
                t.title = id.at(3);
                s->db->writeTab(t);
                tabList.append(t.id);
            }

            ++i;
        }

    }  else {
        qWarning() << "No \"tabs\" element found!";
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void OldReaderFetcher::getFolderFromCategories(const QJsonArray &categories, QString &tabId, QString &tabName)
{
    QJsonArray::const_iterator i = categories.constBegin();
    QJsonArray::const_iterator end = categories.constEnd();
#else
void OldReaderFetcher::getFolderFromCategories(const QVariantList &categories, QString &tabId, QString &tabName)
{
    QVariantList::const_iterator i = categories.constBegin();
    QVariantList::const_iterator end = categories.constEnd();
#endif
    while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        QJsonObject obj = (*i).toObject();
#else
        QVariantMap obj = (*i).toMap();
#endif
        QStringList id = obj["id"].toString().split('/');
        //qDebug() << id;
        if (id.at(2)=="label") {
            // Subscription can be only in one folder
            // Always true?
            tabId = obj["id"].toString();
            tabName = id.at(3);
            return;
        }

        ++i;
    }

    tabId = "";
    tabName = "";
}

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void OldReaderFetcher::getFromCategories(const QJsonArray &categories, QVariantMap &result)
{
    QJsonArray::const_iterator i = categories.constBegin();
    QJsonArray::const_iterator end = categories.constEnd();
#else
void OldReaderFetcher::getFromCategories(const QVariantList &categories, QVariantMap &result)
{
    QVariantList::const_iterator i = categories.constBegin();
    QVariantList::const_iterator end = categories.constEnd();
#endif
    bool read = false;
    bool starred = false;
    bool fresh = false;
    bool liked = false;
    bool broadcast = false;
    while (i != end) {
        QStringList id = (*i).toString().split('/');
        //qDebug() << (*i).toString();
        if (id.at(2)=="label") {
            result.insert("tabId",QVariant((*i).toString()));
            result.insert("tabName",QVariant(id.at(3)));
        } else if ((*i).toString() == "user/-/state/com.google/read") {
            read = true;
        } else if ((*i).toString() == "user/-/state/com.google/starred") {
            starred = true;
        } else if ((*i).toString() == "user/-/state/com.google/like") {
            liked = true;
        } else if ((*i).toString() == "user/-/state/com.google/fresh") {
            fresh = true;
        } else if ((*i).toString() == "user/-/state/com.google/broadcast") {
            broadcast = true;
        }

        ++i;
    }

    if (read)
        result.insert("read",QVariant(1));
    else
        result.insert("read",QVariant(0));
    if (starred)
        result.insert("starred",QVariant(1));
    else
        result.insert("starred",QVariant(0));
    if (liked)
        result.insert("liked",QVariant(1));
    else
        result.insert("liked",QVariant(0));
    if (fresh)
        result.insert("fresh",QVariant(1));
    else
        result.insert("fresh",QVariant(0));
    if (broadcast)
        result.insert("broadcast",QVariant(1));
    else
        result.insert("broadcast",QVariant(0));
}

void OldReaderFetcher::storeFriends()
{
    tabList.clear();

    Settings *s = Settings::instance();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (jsonObj["friends"].isArray()) {
        QJsonArray::const_iterator i = jsonObj["friends"].toArray().constBegin();
        QJsonArray::const_iterator end = jsonObj["friends"].toArray().constEnd();
#else
    if (jsonObj["friends"].type()==QVariant::List) {
        QVariantList::const_iterator i = jsonObj["friends"].toList().constBegin();
        QVariantList::const_iterator end = jsonObj["friends"].toList().constEnd();
#endif
        bool addTab = false;
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = (*i).toObject();
#else
            QVariantMap obj = (*i).toMap();
#endif
            addTab = true;

            // Stream
            DatabaseManager::Stream st;
            st.id = obj["stream"].toString();
            st.title = obj["displayName"].toString().remove(QRegExp("<[^>]*>"));
            st.content = "";
            st.type = "";
            st.unread = 0;
            st.read = 0;
            st.slow = 0;
            st.lastUpdate = QDateTime::currentDateTimeUtc().toTime_t();
            if (obj["iconUrl"].toString() != "") {
                st.icon = "http:"+obj["iconUrl"].toString();
                // Downloading fav icon file
                DatabaseManager::CacheItem item;
                item.origUrl = st.icon;
                item.finalUrl = st.icon;
                item.type = "icon";
                emit addDownload(item);
            }
            s->db->writeStream(st);

            // Module
            DatabaseManager::Module m;
            m.id = st.id;
            m.name = st.title;
            m.title = st.title;
            m.tabId = "friends";
            m.streamList.append(st.id);
            s->db->writeModule(m);

            /*DatabaseManager::StreamModuleTab smt;
            smt.streamId = st.id;
            smt.moduleId = st.id;
            smt.tabId = m.tabId;
            feedList.append(smt);*/

            ++i;
        }

        if (addTab) {
            // Adding Friends folder
            DatabaseManager::Tab t;
            t.id = "friends";
            t.dashboardId = "oldreader";
            t.title = "Following";
            s->db->writeTab(t);
            tabList.append(t.id);
        }

    }  else {
        qWarning() << "No \"friends\" element found!";
    }
}

void OldReaderFetcher::storeFeeds()
{
    Settings *s = Settings::instance();

    bool subscriptionsFolderFeed = false;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (jsonObj["subscriptions"].isArray()) {
        QJsonArray::const_iterator i = jsonObj["subscriptions"].toArray().constBegin();
        QJsonArray::const_iterator end = jsonObj["subscriptions"].toArray().constEnd();
#else
    if (jsonObj["subscriptions"].type()==QVariant::List) {
        QVariantList::const_iterator i = jsonObj["subscriptions"].toList().constBegin();
        QVariantList::const_iterator end = jsonObj["subscriptions"].toList().constEnd();
#endif
        while (i != end) {
            QString tabId, tabName;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)

            // Checking tab (folder)
            QJsonObject obj = (*i).toObject();
            if (obj["categories"].isArray()) {
                getFolderFromCategories(obj["categories"].toArray(), tabId, tabName);

            }
#else
            QVariantMap obj = (*i).toMap();
            if (obj["categories"].type()==QVariant::List) {
                getFolderFromCategories(obj["categories"].toList(), tabId, tabName);
            }
#endif
            QStringList id = obj["id"].toString().split('/');
            if (tabId == "" && !id.isEmpty() && id.at(0) == "feed") {
                // Feed without label -> Subscriptions folder
                tabId = "subscriptions";
                subscriptionsFolderFeed = true;
            }

            if (!id.isEmpty() && id.at(0) == "feed") {
                // Stream
                DatabaseManager::Stream st;
                st.id = obj["id"].toString();
                st.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
                st.link = obj["htmlUrl"].toString();
                st.query = obj["url"].toString();
                st.content = "";
                st.type = "";
                st.unread = 0;
                st.read = 0;
                st.slow = 0;
                st.newestItemAddedAt = (int) obj["firstitemmsec"].toDouble();
                st.updateAt = (int) obj["firstitemmsec"].toDouble();
                st.lastUpdate = QDateTime::currentDateTimeUtc().toTime_t();
                if (obj["iconUrl"].toString() != "") {
                    st.icon = "http:"+obj["iconUrl"].toString();
                    // Downloading fav icon file
                    DatabaseManager::CacheItem item;
                    item.origUrl = st.icon;
                    item.finalUrl = st.icon;
                    item.type = "icon";
                    emit addDownload(item);
                }
                s->db->writeStream(st);

                // Module
                DatabaseManager::Module m;
                m.id = st.id;
                m.name = st.title;
                m.title = st.title;
                m.status = "";
                m.widgetId = "";
                m.pageId = "";
                m.tabId = tabId;
                m.streamList.append(st.id);
                s->db->writeModule(m);

                /*DatabaseManager::StreamModuleTab smt;
                smt.streamId = st.id;
                smt.moduleId = st.id;
                smt.tabId = tabId;
                feedList.append(smt);*/
            }

            ++i;
        }

    }  else {
        qWarning() << "No \"tabs\" element found!";
    }

    if (!subscriptionsFolderFeed) {
        // Removing Subscriptions folder
        s->db->removeTabById("subscriptions");
    }
}

void OldReaderFetcher::storeStream()
{
    Settings *s = Settings::instance();

    double updated = 0;
    int retentionDays = s->getRetentionDays();

    //qDebug() << "getRetentionDays" << retentionDays;
    //qint64 date1 = QDateTime::currentMSecsSinceEpoch();
    //int items = 0;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (jsonObj["updated"].isDouble()) {
        updated = jsonObj["updated"].toDouble();
    } else {
        qWarning() << "No updated param in stream!";
    }
    if (jsonObj["items"].isArray()) {
        QJsonArray::const_iterator i = jsonObj["items"].toArray().constBegin();
        QJsonArray::const_iterator end = jsonObj["items"].toArray().constEnd();
#else
    //qDebug() << jsonObj["updated"].type();
    if (jsonObj["updated"].type()==QVariant::ULongLong) {
        updated = jsonObj["updated"].toDouble();
    } else {
        qWarning() << "No updated param in stream!";
    }
    if (jsonObj["items"].type()==QVariant::List) {
        QVariantList::const_iterator i = jsonObj["items"].toList().constBegin();
        QVariantList::const_iterator end = jsonObj["items"].toList().constEnd();
#endif
        //qDebug() << "Updated:" << updated;
        while (i != end) {
            QString feedId;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = (*i).toObject();
            if (obj["origin"].isObject()) {
                feedId = obj["origin"].toObject()["streamId"].toString();
            }
#else
            QVariantMap obj = (*i).toMap();
            if (obj["origin"].type() == QVariant::Map) {
                feedId = obj["origin"].toMap()["streamId"].toString();
            }
#endif
            DatabaseManager::Entry e;
            e.id = obj["id"].toString();
            e.streamId = feedId;
            e.title = obj["title"].toString();
            e.author = obj["author"].toString();
            QVariantMap categories;

            //qDebug() << e.id << e.title;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            if (obj["summary"].isObject())
                e.content = obj["summary"].toObject()["content"].toString();
            if (obj["canonical"].isArray() && !obj["canonical"].toArray().isEmpty() && obj["canonical"].toArray()[0].isObject())
                e.link = obj["canonical"].toArray()[0].toObject()["href"].toString();
            if (obj["categories"].isArray())
                getFromCategories(obj["categories"].toArray(), categories);
            if (obj["annotations"].isArray() && !obj["annotations"].toArray().isEmpty())
                e.annotations = obj["annotations"].toArray()[0].toString();
#else
            if (obj["summary"].type() == QVariant::Map)
                e.content = obj["summary"].toMap()["content"].toString();
            if (obj["canonical"].type() == QVariant::List && !obj["canonical"].toList().isEmpty() && obj["canonical"].toList()[0].type() == QVariant::Map)
                e.link = obj["canonical"].toList()[0].toMap()["href"].toString();
            if (obj["categories"].type() == QVariant::List)
                getFromCategories(obj["categories"].toList(), categories);
            if (obj["annotations"].type() == QVariant::List && !obj["annotations"].toList().isEmpty()) {
                e.annotations = obj["annotations"].toList()[0].toString();
            }
#endif
            e.read = categories.value("read").toInt();
            e.saved = categories.value("starred").toInt();
            e.liked = categories.value("liked").toInt();
            e.freshOR = categories.value("fresh").toInt();
            e.cached = 0;
            e.broadcast = categories.value("broadcast").toInt();
            e.publishedAt = obj["published"].toDouble();
            e.createdAt = obj["updated"].toDouble();
            e.fresh = 1;

            QString crawlTime = obj["crawlTimeMsec"].toString();
            crawlTime.chop(3); // converting Msec to sec
            e.crawlTime = crawlTime.toDouble();
            QString timestamp = obj["timestampUsec"].toString();
            timestamp.chop(6); // converting Usec to sec
            e.timestamp = timestamp.toDouble();

            /*qDebug() << ">>>>>>>>>>>>>>>";
            qDebug() << e.title << e.streamId;
            qDebug() << "crawlTime" << e.crawlTime;
            qDebug() << "timestampUsec" << e.timestamp;
            qDebug() << "publishedAt"<< e.publishedAt;
            qDebug() << "createdAt" << e.createdAt;
            qDebug() << "<<<<<<<<<<<<<<<";*/

            // Downloading image file
            // Checking if content contains image
            QRegExp rx("<img\\s[^>]*src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
            if (rx.indexIn(e.content)!=-1) {
                QString imgSrc = rx.cap(1); imgSrc = imgSrc.mid(1,imgSrc.length()-2);
                if (imgSrc!="") {
                    if (s->getCachingMode() == 2 || (s->getCachingMode() == 1 && s->dm->isWLANConnected())) {
                        if (!s->db->isCacheExistsByFinalUrl(Utils::hash(imgSrc))) {
                            DatabaseManager::CacheItem item;
                            item.origUrl = imgSrc;
                            item.finalUrl = imgSrc;
                            item.type = "entry-image";
                            emit addDownload(item);
                        }
                    }
                    e.image = imgSrc;
                }
            }

            s->db->writeEntry(e);

            // Progress, only for StoreStream
            //++items;
            if (currentJob == StoreStream && retentionDays > 0) {
                int newLastDate = QDateTime::fromTime_t(e.crawlTime).daysTo(QDateTime::currentDateTimeUtc());
                //qDebug() << "newLastDate" << newLastDate;
                if (newLastDate > retentionDays) {
                    //qDebug() << "newLastDate > retentionDays";
                    lastDate = retentionDays;
                    lastContinuation = "";
                    ++continuationCount;

                    //qDebug() << "db write time:" << (QDateTime::currentMSecsSinceEpoch() - date1) << "items:" << items;
                    return;
                } else {
                    lastDate = newLastDate;
                }
            }

            ++i;
        }
    }

    //qDebug() << "db write time:" << (QDateTime::currentMSecsSinceEpoch() - date1) << "items:" << items;

    QString continuation = jsonObj["continuation"].toString();
    lastContinuation = continuation;
    ++continuationCount;
}

/*void OldReaderFetcher::removeDeletedFeeds()
{
    // Removing all existing feeds form feedList
    QList<DatabaseManager::StreamModuleTab>::iterator si = storedFeedList.begin();

    while (si != storedFeedList.end()) {
        bool newFeed = true;
        QList<DatabaseManager::StreamModuleTab>::iterator ui = feedList.begin();
        while (ui != feedList.end()) {
            if ((*ui).streamId == (*si).streamId) {
                newFeed = false;
                break;
            }
            ++ui;
        }

        if (newFeed) {
            qDebug() << "Removing feed:" << (*si).streamId;
            Settings *s = Settings::instance();
            s->db->removeStreamsByStream((*si).streamId);
        }

        ++si;
    }
}*/

void OldReaderFetcher::uploadActions()
{
    if (!actionsList.isEmpty()) {
        emit uploading();
        //qDebug() << "Uploading actions...";
        setAction();
    }
}

void OldReaderFetcher::markSlowFeeds()
{
    // A feed is considered "slow" when it publishes
    // less than 5 articles in a month.

    Settings *s = Settings::instance();
    QStringList list = s->db->readStreamIds();
    QStringList::iterator it = list.begin();
    while (it != list.end()) {
        int n = s->db->countEntriesNewerThanByStream(*it, QDateTime::currentDateTime().addDays(-30));
        if (n<5) {
            // Slow detected
            s->db->updateStreamSlowFlagById(*it, 1);
        }
        ++it;
    }
}
