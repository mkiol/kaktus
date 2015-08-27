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
#include <QUrl>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QStringList>
#include <QDateTime>
#include <math.h>
#else
#include "parser.h"
#endif

#include "feedlyfetcher.h"
#include "settings.h"
#include "downloadmanager.h"
#include "utils.h"
#include "../feedly.h"

FeedlyFetcher::FeedlyFetcher(QObject *parent) :
    Fetcher(parent),
    currentJob(Idle),
    refreshTokenDone(false)
{
}

void FeedlyFetcher::refreshToken()
{
    data.clear();

    Settings *s = Settings::instance();

    // Check is already have cookie
    if (s->getRefreshCookie() == "") {
        qWarning() << "Refresh token is missing!";
        setBusy(false);
        emit error(500);
        return;
    }

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QString body;
    QNetworkRequest request;

    request.setUrl(QUrl(QString("%1/v3/auth/token").arg(feedly_server)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    body = QString("refresh_token=%1&client_id=%2&client_secret=%3&grant_type=refresh_token")
            .arg(s->getRefreshCookie(), feedly_client_id, feedly_client_secret);
    currentReply = nam.post(request,body.toUtf8());

    //qDebug() << "body:" << body;

#ifndef QT_NO_SSL
    connect(currentReply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedRefreshToken()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void FeedlyFetcher::signIn()
{
    data.clear();

    Settings *s = Settings::instance();

    // Check is already have cookie

    //s->setCookie("");
    //s->setUserId("");
    //prepareUploadActions();

    if (s->getCookie() != "" && s->getRefreshCookie() != "") {
        if (refreshTokenDone)
            prepareUploadActions();
        else
            refreshToken();
        return;
    }

    int type = s->getSigninType();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QString body;
    QNetworkRequest request;

    switch (type) {
    case 20:
        if (s->getAuthUrl() == "") {
            qWarning() << "Not authorized!";
            emit error(400);
            setBusy(false);
            return;
        }

        request.setUrl(QUrl(QString("%1/v3/auth/token").arg(feedly_server)));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
        body = QString("code=%1&client_id=%2&client_secret=%3&redirect_uri=urn:ietf:wg:oauth:2.0:oob&grant_type=authorization_code")
                .arg(s->getAuthUrl(), feedly_client_id, feedly_client_secret);
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
    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedSignIn()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

QString FeedlyFetcher::getIdsFromActionString(const QString &actionString)
{
    QString ids;
    QStringList idList = actionString.split("&");
    QStringList::iterator i = idList.begin();
    while (i != idList.end()) {
        ids += QString("\"%1\",").arg(*i);
        ++i;
    }
    ids.remove(ids.length()-1,1);
    //qDebug() << "ids:" << ids;
    return ids;
}

void FeedlyFetcher::setAction()
{
    data.clear();

    DatabaseManager::Action action = actionsList.first();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url(QString("%1/v3/markers").arg(feedly_server));
    QString body, latestId;
    QStringList list;
    QStringList::iterator it;
    switch (action.type) {
    case DatabaseManager::SetRead:
        body = QString("{\"action\":\"markAsRead\",\"type\":\"entries\",\"entryIds\":[\"%1\"]}").arg(action.id1);
        break;
    case DatabaseManager::UnSetRead:
        body = QString("{\"action\":\"keepUnread\",\"type\":\"entries\",\"entryIds\":[\"%1\"]}").arg(action.id1);
        break;
    case DatabaseManager::SetListRead:
        body = QString("{\"action\":\"markAsRead\",\"type\":\"entries\",\"entryIds\":[%1]}").arg(getIdsFromActionString(action.id1));
        break;
    case DatabaseManager::UnSetListRead:
        body = QString("{\"action\":\"keepUnread\",\"type\":\"entries\",\"entryIds\":[%1]}").arg(getIdsFromActionString(action.id1));
        break;
    case DatabaseManager::SetSaved:
        body = QString("{\"action\":\"markAsSaved\",\"type\":\"entries\",\"entryIds\":[\"%1\"]}").arg(action.id1);
        break;
    case DatabaseManager::UnSetSaved:
        body = QString("{\"action\":\"markAsUnsaved\",\"type\":\"entries\",\"entryIds\":[\"%1\"]}").arg(action.id1);
        break;
    case DatabaseManager::SetStreamReadAll:
        body = QString("{\"action\":\"markAsRead\",\"type\":\"feeds\",\"lastReadEntryId\":\"%1\",\"feedIds\":[\"%2\"]}")
                .arg(s->db->readLatestEntryIdByStream(action.id1))
                .arg(action.id1);
        break;
    case DatabaseManager::SetTabReadAll:
        latestId = s->db->readLatestEntryIdByTab(action.id1);
        if (action.id1 == "global.uncategorized") {
            action.id1 = QString("user/%1/category/global.uncategorized").arg(s->getUserId());
        }
        body = QString("{\"action\":\"markAsRead\",\"type\":\"categories\",\"lastReadEntryId\":\"%1\",\"categoryIds\":[\"%2\"]}")
                .arg(latestId)
                .arg(action.id1);
        break;
    case DatabaseManager::SetAllRead:
        body = QString("{\"action\":\"markAsRead\",\"type\":\"categories\",\"lastReadEntryId\":\"%1\",\"categoryIds\":[")
                .arg(s->db->readLatestEntryIdByDashboard(action.id1));

        list = s->db->readTabIdsByDashboard(action.id1);
        it = list.begin();
        while (it != list.end()) {
            if (*it == "global.uncategorized") {
                *it = QString("user/%1/category/global.uncategorized").arg(s->getUserId());
            }
            body += QString("\"%1\",").arg(*it);
            ++it;
        }

        body.remove(body.length()-1,1);
        body += "]}";
        break;
    default:
        // Unknown action -> skiping
        qWarning("Unknown action!");
        finishedSetAction();
        return;
    }

    qDebug() << "body:" << body;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization",QString("OAuth %1").arg(s->getCookie()).toLatin1());

    currentReply = nam.post(request,body.toUtf8());

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedSetAction()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void FeedlyFetcher::fetchTabs()
{
    //qDebug() << "fetchTabs";
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url(QString("%1/v3/categories").arg(feedly_server));
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("OAuth %1").arg(s->getCookie()).toLatin1());

    //qDebug() << s->getCookie();
    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedTabs()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void FeedlyFetcher::fetchProfile()
{
    //qDebug() << "fetchProfile";
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url(QString("%1/v3/profile").arg(feedly_server));
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("OAuth %1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedProfile()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void FeedlyFetcher::fetchFeeds()
{
    //qDebug() << "fetchFeeds";
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url(QString("%1/v3/subscriptions").arg(feedly_server));
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("OAuth %1").arg(s->getCookie()).toLatin1());

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedFeeds()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void FeedlyFetcher::fetchStream()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    //QString st(QUrl::toPercentEncoding(""));
    QString st = QString("user/%1/category/global.all").arg(s->getUserId());;

    int epoch = s->getRetentionDays() > 0 ?
                QDateTime::currentDateTimeUtc().addDays(0-s->getRetentionDays()).toTime_t() :
                0;
    QString epochMsc = QString::number(epoch).append("000");

    QString surl = QString("%1/v3/streams/contents?streamId=%2&count=%3").arg(feedly_server).arg(st).arg(limitAtOnce);
    if (lastContinuation != "")
        surl += QString("&continuation=%1").arg(lastContinuation);
    if (epoch > 0)
        surl += QString("&newerThan=%1").arg(epochMsc);
    surl += QString("&unreadOnly=%1").arg(s->getSyncRead() ? "false" : "true");

    QUrl url(surl);
    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("OAuth %1").arg(s->getCookie()).toLatin1());

    //qDebug() << url.toString();

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedStream()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void FeedlyFetcher::fetchStarredStream()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url;
    QString st = QString("user/%1/tag/global.saved").arg(s->getUserId());

    if (lastContinuation == "") {
        url.setUrl(QString("%1/v3/streams/contents?streamId=%2&count=%3")
                   .arg(feedly_server).arg(st).arg(limitAtOnce));

    } else {
        url.setUrl(QString("%1/v3/streams/contents?streamId=%2&continuation=%3&count=%4")
                   .arg(feedly_server).arg(st).arg(lastContinuation).arg(limitAtOnce));
    }

    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("OAuth %1").arg(s->getCookie()).toLatin1());

    //qDebug() << url.toString();

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedStarredStream()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void FeedlyFetcher::fetchMustStream()
{
    data.clear();

    Settings *s = Settings::instance();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QUrl url;
    QString st = QString("user/%1/category/global.must").arg(s->getUserId());

    if (lastContinuation == "") {
        url.setUrl(QString("%1/v3/streams/contents?streamId=%2&count=%3")
                   .arg(feedly_server).arg(st).arg(limitAtOnce));

    } else {
        url.setUrl(QString("%1/v3/streams/contents?streamId=%2&continuation=%3&count=%4")
                   .arg(feedly_server).arg(st).arg(lastContinuation).arg(limitAtOnce));
    }

    QNetworkRequest request(url);

    request.setRawHeader("Authorization",QString("OAuth %1").arg(s->getCookie()).toLatin1());

    //qDebug() << url.toString();

    currentReply = nam.get(request);

    connect(currentReply, SIGNAL(finished()), this, SLOT(finishedMustStream()));
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void FeedlyFetcher::finishedSignInOnlyCheck()
{
    qWarning() << "Not implemented!";
}

void FeedlyFetcher::finishedRefreshToken()
{
    //qDebug() << data;
    Settings *s = Settings::instance();

    if (currentReply->error() &&
        currentReply->error()!=QNetworkReply::OperationCanceledError) {
        qWarning() << "Refresh token failed!";
        emit error(505);
        setBusy(false);
        return;
    }

    if (parse()) {
        QString auth = jsonObj["access_token"].toString();
        QString uid = jsonObj["id"].toString();

        if (uid != "" && auth != "") {
            s->setCookie(auth);
            s->setUserId(uid);
            refreshTokenDone = true;
            prepareUploadActions();
        } else {
            qWarning() << "Refresh token failed! Can not find access_token!";
            qWarning() << "Response:" << jsonObj;
            emit error(505);
            setBusy(false);
        }
    } else {
        qWarning() << "Refresh token failed! Error while parsing JSON";
        qWarning() << "Response:" << data;
        emit error(505);
        setBusy(false);
    }
}

void FeedlyFetcher::finishedSignIn()
{
    //qDebug() << data;
    Settings *s = Settings::instance();

    if (currentReply->error() &&
        currentReply->error()!=QNetworkReply::OperationCanceledError) {
        //int code = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qWarning() << "Sign in failed!";
        emit error(501);
        setBusy(false);
        return;
    }

    switch (s->getSigninType()) {
    case 20:
        if (parse()) {
            QString auth = jsonObj["access_token"].toString();
            QString rauth = jsonObj["refresh_token"].toString();
            QString uid = jsonObj["id"].toString();
            if (uid != "" && auth != "" && rauth != "") {
                s->setSignedIn(true);
                s->setCookie(auth);
                s->setRefreshCookie(rauth);
                s->setUserId(uid);

                QString provider = jsonObj["provider"].toString();
                //qDebug() << "Signin provider:" << provider;
                s->setProvider(provider);

                refreshTokenDone = true;
                prepareUploadActions();

            } else {
                s->setSignedIn(false);
                qWarning() << "Sign in failed! Can not id or find access_token or refresh_token!";
                qWarning() << "Response:" << jsonObj;
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

void FeedlyFetcher::finishedTabs()
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

void FeedlyFetcher::finishedTabs2()
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

    fetchFeeds();
}

void FeedlyFetcher::finishedProfile()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreProfile);
}

void FeedlyFetcher::finishedProfile2()
{
    Settings *s = Settings::instance();
    if (s->getUserId() == "") {
        qWarning() << "User ID is empty!";
        emit error(503);
        setBusy(false);
        return;
    }

    //prepareUploadActions();
}

void FeedlyFetcher::finishedFeeds()
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
    startJob(StoreFeeds);
}

void FeedlyFetcher::finishedFeeds2()
{
    // Proggres initiating, one step is one day
    Settings *s = Settings::instance();
    //proggressTotal = s->getRetentionDays() > 0 ? log(s->getRetentionDays()) + 1 : 2;
    proggressTotal = s->getRetentionDays() > 0 ? s->getRetentionDays() + 1 : 2;
    proggress = 1;
    lastDate = 0;
    emit progress(proggress, proggressTotal);

    /*if (busyType == Fetcher::Updating)
        removeDeletedFeeds();*/

    s->db->updateEntriesFlag(1); // Marking as old

    fetchStream();
}

void FeedlyFetcher::finishedStream()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreStream);
}

void FeedlyFetcher::finishedStream2()
{
    Settings *s = Settings::instance();
    if (s->getRetentionDays() > 0) {
        if (lastDate > s->getRetentionDays()) {
            lastDate = s->getRetentionDays();
        }
    }

    double _proggresDelta = lastDate;
    //if (lastDate > 0)
    //    _proggresDelta = log(lastDate);
    emit progress(proggress + _proggresDelta, proggressTotal);

    if (lastContinuation == "" ||
        continuationCount > continuationLimit) {

        this->proggress += s->getRetentionDays() > 0 ? _proggresDelta : 1;

        lastContinuation = "";
        continuationCount = 0;
        lastDate = 0;

        fetchStarredStream();
        return;
    }

    fetchStream();
}

void FeedlyFetcher::finishedStarredStream()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreStarredStream);
}

void FeedlyFetcher::finishedStarredStream2()
{
    if (lastContinuation == "" ||
        continuationCount > continuationLimit) {

        ++proggress;
        emit progress(proggress, proggressTotal);

        //startJob(MarkSlow);
        //fetchMustStream();
        taskEnd();
        return;
    }

    fetchStarredStream();
}

void FeedlyFetcher::finishedMustStream()
{
    //qDebug() << data;
    if (currentReply->error()) {
        emit error(500);
        setBusy(false);
        return;
    }

    startJob(StoreMustStream);
}

void FeedlyFetcher::finishedMustStream2()
{
    if (lastContinuation == "" ||
        continuationCount > continuationLimit) {

        ++proggress;
        emit progress(proggress, proggressTotal);

        //startJob(MarkSlow);
        taskEnd();
        return;
    }

    fetchMustStream();
}

void FeedlyFetcher::finishedSetAction()
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

void FeedlyFetcher::finishedMarkSlow()
{
    // Deleting old entries
    Settings *s = Settings::instance();
    s->db->removeEntriesByFlag(1);

    taskEnd();
    return;
}

bool FeedlyFetcher::setConnectUrl(const QString &url)
{
    //qDebug() << url << QUrl(url).host();

    QUrl _url(url);

    if (_url.host() == "localhost") {

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        QUrlQuery query(_url);
        if (query.hasQueryItem("error")) {
            qWarning() << "Error in signin! Error string:" << query.queryItemValue("error");
#else
        if (_url.hasQueryItem("error")) {
            qWarning() << "Error in signin! Error string:" << _url.queryItemValue("error");
#endif
            emit error(402);
            return false;
        }
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        if (query.hasQueryItem("code")) {
            QString code = query.queryItemValue("code");
#else
        if (_url.hasQueryItem("code")) {
            QString code = _url.queryItemValue("code");
#endif
            if (code != "") {
                Settings *s = Settings::instance();
                s->setAuthUrl(code);
                return true;
            }
        }
    }

    return false;
}

void FeedlyFetcher::getConnectUrl(int type)
{
    Q_UNUSED(type)

    if (busy) {
        qWarning() << "Fetcher is busy!";
        return;
    }

    QString url = QString("%1/v3/auth/auth?response_type=code&clientId=sandbox&redirect_uri=http%3A%2F%2Flocalhost&scope=https://cloud.feedly.com/subscriptions").arg(feedly_server);
    emit newAuthUrl(url,20);
}

void FeedlyFetcher::startFetching()
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

    // Feedly API doesnt have Dashboards
    // Manually adding dummy Dashboard
    DatabaseManager::Dashboard d;
    d.id = "feedly";
    d.name = "Default";
    d.title = "Default";
    d.description = "Feedly dafault dashboard";
    s->db->writeDashboard(d);
    s->setDashboardInUse(d.id);

    fetchTabs();
}

void FeedlyFetcher::startJob(Job job)
{
    if (isRunning()) {
        qWarning() << "Job is running";
        return;
    }

    disconnect(this, SIGNAL(finished()), 0, 0);
    currentJob = job;

    if (parse()) {
        //TODO: handling API errors
    } else {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    switch (job) {
    case StoreProfile:
        connect(this, SIGNAL(finished()), this, SLOT(finishedProfile2()));
        break;
    case StoreTabs:
        connect(this, SIGNAL(finished()), this, SLOT(finishedTabs2()));
        break;
    case StoreFeeds:
        connect(this, SIGNAL(finished()), this, SLOT(finishedFeeds2()));
        break;
    case StoreStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedStream2()));
        break;
    case StoreStarredStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedStarredStream2()));
        break;
    case StoreMustStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedMustStream2()));
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

void FeedlyFetcher::run()
{
    switch (currentJob) {
    case StoreProfile:
        storeProfile();
    case StoreTabs:
        storeTabs();
        break;
    case StoreFeeds:
        storeFeeds();
        break;
    case StoreStream:
    case StoreStarredStream:
    case StoreMustStream:
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

void FeedlyFetcher::storeTabs()
{
    Settings *s = Settings::instance();
    QString dashboardId = "feedly";

    // Adding Uncategorized folder
    DatabaseManager::Tab t;
    t.id = "global.uncategorized";
    t.dashboardId = dashboardId;
    t.title = "Uncategorized";
    s->db->writeTab(t);
    tabList.append(t.id);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (!jsonArr.isEmpty()) {
        QJsonArray::const_iterator i = jsonArr.constBegin();
        QJsonArray::const_iterator end = jsonArr.constEnd();
#else
    qDebug() << jsonArr;
    if (!jsonArr.empty()) {
        QVariantList::const_iterator i = jsonArr.constBegin();
        QVariantList::const_iterator end = jsonArr.constEnd();
#endif
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = (*i).toObject();
#else
            QVariantMap obj = (*i).toMap();
#endif
            // Checking if folder (category)
            QStringList id = obj["id"].toString().split('/');
            QString name = obj["label"].toString();
            if (id.at(2)=="category" && name != "") {
                // Tab
                DatabaseManager::Tab t;
                t.id = obj["id"].toString();
                t.dashboardId = dashboardId;
                t.title = name;
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
void FeedlyFetcher::getFolderFromCategories(const QJsonArray &categories, QString &tabId, QString &tabName)
{
    QJsonArray::const_iterator i = categories.constBegin();
    QJsonArray::const_iterator end = categories.constEnd();
#else
void FeedlyFetcher::getFolderFromCategories(const QVariantList &categories, QString &tabId, QString &tabName)
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
        tabId = obj["id"].toString();
        tabName = obj["label"].toString();

        if (tabName != "" && tabId != "") {
            // Subscription can be only in one folder
            // Always true?
            return;
        }

        ++i;
    }

    tabId = "";
    tabName = "";
}

/*#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void FeedlyFetcher::getFromTags(const QJsonArray &tags, QVariantMap &result)
{
    QJsonArray::const_iterator i = tags.constBegin();
    QJsonArray::const_iterator end = tags.constEnd();
#else
void FeedlyFetcher::getFromTags(const QVariantList &tags, QVariantMap &result)
{
    QVariantList::const_iterator i = tags.constBegin();
    QVariantList::const_iterator end = tags.constEnd();
#endif
    bool read = false;
    bool starred = false;

    while (i != end) {
        QString fullId = (*i).toObject()["id"].toString();
        QStringList idList = fullId.split('/');
        //qDebug() << fullId << idList.count();
        if (idList.count()>3) {
            QString id = idList.at(3);
            if (id == "global.read") {
                read = true;
            } else if (id == "global.saved") {
                starred = true;
            }
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

    //qDebug() << result;
}*/

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
bool FeedlyFetcher::getSavedFromTags(const QJsonArray &tags)
{
    QJsonArray::const_iterator i = tags.constBegin();
    QJsonArray::const_iterator end = tags.constEnd();

    while (i != end) {
        QString fullId = (*i).toObject()["id"].toString();
#else
bool FeedlyFetcher::getSavedFromTags(const QVariantList &tags)
{
    QVariantList::const_iterator i = tags.constBegin();
    QVariantList::const_iterator end = tags.constEnd();

    while (i != end) {
        QString fullId = (*i).toMap()["id"].toString();
#endif
        QStringList idList = fullId.split('/');
        //qDebug() << fullId << idList.count();
        if (idList.count()>3) {
            if (idList.at(3) == "global.saved")
                return true;
        }
        ++i;
    }
    return false;
}

void FeedlyFetcher::storeProfile()
{
    Settings *s = Settings::instance();
    s->setUserId(jsonObj["id"].toString());
}

void FeedlyFetcher::storeFeeds()
{
    Settings *s = Settings::instance();

    bool subscriptionsFolderFeed = false;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (!jsonArr.isEmpty()) {
        QJsonArray::const_iterator i = jsonArr.constBegin();
        QJsonArray::const_iterator end = jsonArr.constEnd();
#else
    if (!jsonArr.empty()) {
        QVariantList::const_iterator i = jsonArr.constBegin();
        QVariantList::const_iterator end = jsonArr.constEnd();
#endif
        while (i != end) {
            QString tabId, tabName;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            // Checking tab (folder)
            QJsonObject obj = (*i).toObject();
            if (obj["categories"].isArray()) {
                getFolderFromCategories(obj["categories"].toArray(), tabId, tabName);
                //qDebug() << "tabId" << tabId << "tabName" << tabName;
            }
#else
            // Checking tab (folder)
            QVariantMap obj = (*i).toMap();
            if (obj["categories"].type()==QVariant::List) {
                getFolderFromCategories(obj["categories"].toList(), tabId, tabName);
            }
#endif
            QStringList id = obj["id"].toString().split('/');
            if (tabId == "" && !id.isEmpty() && id.at(0) == "feed") {
                // Feed without label -> Uncategorized folder
                tabId = "global.uncategorized";
                subscriptionsFolderFeed = true;
            }

            if (!id.isEmpty() && id.at(0) == "feed") {
                // Stream
                DatabaseManager::Stream st;
                st.id = obj["id"].toString();
                st.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
                st.link = obj["website"].toString();
                st.query = "";
                st.content = "";
                st.type = "";
                st.unread = 0;
                st.read = 0;
                st.slow = 0;
                st.newestItemAddedAt = (int) obj["updated"].toDouble();
                st.updateAt = (int) obj["updated"].toDouble();
                st.lastUpdate = QDateTime::currentDateTimeUtc().toTime_t();
                if (obj["iconUrl"].toString() != "") {
                    st.icon = obj["iconUrl"].toString();
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
        // Removing Uncategorized folder
        s->db->removeTabById("global.uncategorized");
    }
}

void FeedlyFetcher::storeStream()
{
    Settings *s = Settings::instance();

    double updated = 0;
    int retentionDays = s->getRetentionDays();

    //qDebug() << "getRetentionDays" << retentionDays;
    //qint64 date1 = QDateTime::currentMSecsSinceEpoch();
    //int items = 0;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (jsonObj["updated"].isDouble()) {
        QString updatedString = QString::number(jsonObj["updated"].toDouble(),'f',0);
        updatedString.chop(3); // converting Msec to sec
        updated = updatedString.toDouble();
    } else {
        qWarning() << "No updated param in stream!";
    }
    if (jsonObj["items"].isArray()) {
        QJsonArray::const_iterator i = jsonObj["items"].toArray().constBegin();
        QJsonArray::const_iterator end = jsonObj["items"].toArray().constEnd();
#else
    if (jsonObj["updated"].type()==QVariant::ULongLong) {
        QString updatedString = QString::number(jsonObj["updated"].toDouble(),'f',0);
        updatedString.chop(3); // converting Msec to sec
        updated = updatedString.toDouble();
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
            bool saved = false;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            /*if (obj["enclosure"].isArray())
                qDebug() << "enclosure:" << obj["enclosure"].toArray();*/
            if (obj["summary"].isObject())
                e.content = obj["summary"].toObject()["content"].toString();
            if (obj["canonical"].isArray() && !obj["canonical"].toArray().isEmpty() && obj["canonical"].toArray()[0].isObject())
                e.link = obj["canonical"].toArray()[0].toObject()["href"].toString();
            if (obj["tags"].isArray())
                saved = getSavedFromTags(obj["tags"].toArray());
            /*if (obj["annotations"].isArray() && !obj["annotations"].toArray().isEmpty())
                e.annotations = obj["annotations"].toArray()[0].toString();*/
#else
            if (obj["summary"].type() == QVariant::Map)
                e.content = obj["summary"].toMap()["content"].toString();
            if (obj["canonical"].type() == QVariant::List && !obj["canonical"].toList().isEmpty() && obj["canonical"].toList()[0].type() == QVariant::Map)
                e.link = obj["canonical"].toList()[0].toMap()["href"].toString();
            if (obj["tags"].type() == QVariant::List)
                saved = getSavedFromTags(obj["tags"].toList());
            /*if (obj["annotations"].type() == QVariant::List && !obj["annotations"].toList().isEmpty())
                e.annotations = obj["annotations"].toList()[0].toString();*/
#endif
            e.read = obj["unread"].toBool() ? 0 : 1;
            //e.read = tags.value("read").toInt();
            //e.saved = tags.value("starred").toInt();
            e.saved = saved;
            e.liked = 0;
            e.freshOR = 0;
            e.cached = 0;
            e.broadcast = 0;
            e.fresh = 1;

            /*QString publishedAt = QString::number(obj["published"].toDouble(),'f',0);
            publishedAt.chop(3); // converting Msec to sec
            e.publishedAt = publishedAt.toDouble();*/

            QString crawlTime = QString::number(obj["crawled"].toDouble(),'f',0);
            crawlTime.chop(3); // converting Msec to sec
            e.crawlTime = crawlTime.toDouble();
            e.createdAt = e.crawlTime;
            e.timestamp = e.crawlTime;
            e.publishedAt = e.crawlTime;

            // Downloading image file
            // Getting img url from Json
            QString imgSrc = "";
            /*if (obj["visual"].isObject() && obj["visual"].toObject()["url"].toString() != "")
                imgSrc = obj["visual"].toObject()["url"].toString();*/
            if (imgSrc == "") {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                if (obj["thumbnail"].isArray() && !obj["thumbnail"].toArray().isEmpty() && obj["thumbnail"].toArray().first().isObject())
                    imgSrc = obj["thumbnail"].toArray().first().toObject()["url"].toString();
#else
                if (obj["thumbnail"].type()==QVariant::List && !obj["thumbnail"].toList().empty() && obj["thumbnail"].toList().first().type()==QVariant::Map)
                    imgSrc = obj["thumbnail"].toList().first().toMap()["url"].toString();
#endif
            }
            if (imgSrc == "") {
                // Checking if content contains image
                QRegExp rx("<img\\s[^>]*src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
                if (rx.indexIn(e.content)!=-1) {
                    imgSrc = rx.cap(1); imgSrc = imgSrc.mid(1,imgSrc.length()-2);
                }
            }
            if (imgSrc != "") {
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

            s->db->writeEntry(e);

            // Progress, only for StoreStream
            //++items;
            if (currentJob == StoreStream && retentionDays > 0) {
                int newLastDate = QDateTime::fromTime_t(e.crawlTime).daysTo(QDateTime::currentDateTimeUtc());
                //qDebug() << "crawlTime" << e.crawlTime << QDateTime::fromTime_t(e.crawlTime).toString();
                //qDebug() << "newLastDate" << newLastDate;
                if (newLastDate > retentionDays) {
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

/*void FeedlyFetcher::removeDeletedFeeds()
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

void FeedlyFetcher::uploadActions()
{
    if (!actionsList.isEmpty()) {
        emit uploading();
        //qDebug() << "Uploading actions...";
        setAction();
    }
}

void FeedlyFetcher::markSlowFeeds()
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
