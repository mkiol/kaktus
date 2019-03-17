/*
  Copyright (C) 2019 Michal Kosciesza <michal@mkiol.net>
  Copyright (C) 2019 Renaud Casenave-Péré <renaud@casenave-pere.fr>

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
#include <QSslError>
#include <QtCore/qmath.h>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonDocument>
#endif

#include "ttrssfetcher.h"
#include "settings.h"
#include "downloadmanager.h"
#include "utils.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#define FETCHER_SLOT(callback) &TTRssFetcher::callback
#else
#define FETCHER_SLOT(callback) SLOT(callback)
#endif

TTRssFetcher::TTRssFetcher(QObject *parent) :
  Fetcher(parent),
  currentCommand(NULL),
  currentJob(Idle)
{
}

TTRssFetcher::~TTRssFetcher()
{
}

void TTRssFetcher::signIn()
{
    Settings *s = Settings::instance();

    if (sessionId != "") {
        prepareUploadActions();
        return;
    }

    QString url = s->getUrl();
    QString password = s->getPassword();
    QString username = s->getUsername();
    int type = s->getSigninType();

    switch (type) {
    case 30:
    {
        if (password == "" || username == "" || url == "") {
            qWarning() << "TTRss credentials are invalid!";
            if (busyType == Fetcher::CheckingCredentials)
                emit errorCheckingCredentials(400);
            else
                emit error(400);
            setBusy(false);
            return;
        }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        QJsonObject params;
        params["user"] = username;
        params["password"] = password;
#else
        QString params = "\"user\":\"" + username + "\",\"password\":\"" + password.replace("\\", "\\\\").replace("\"", "\\\"") + "\"";
#endif

        sendApiCall("login", params, FETCHER_SLOT(finishedSignIn));
        break;
    }
    default:
        qWarning() << "Invalid sign in type!";
        emit error(500);
        setBusy(false);
        return;
    }
}

void TTRssFetcher::finishedSignIn()
{
    Settings *s = Settings::instance();
    if (!processResponse()) {
        s->setSignedIn(false);
        sessionId = "";
        apiLevel = 0;
        return;
    } else {
        s->setSignedIn(true);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        sessionId = jsonObj["content"].toObject()["session_id"].toString();
        apiLevel = jsonObj["content"].toObject()["api_level"].toInt();
#else
        sessionId = jsonObj["content"].toMap()["session_id"].toString();
        apiLevel = jsonObj["content"].toMap()["api_level"].toInt();
#endif
        if (busyType == Fetcher::CheckingCredentials) {
            emit credentialsValid();
            setBusy(false);
        } else {
            sendApiCall("getConfig", FETCHER_SLOT(finishedConfig));
        }
    }
}

void TTRssFetcher::finishedConfig()
{
    if (!processResponse()) {
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    iconsUrl = jsonObj["content"].toObject()["icons_url"].toString();
#else
    iconsUrl = jsonObj["content"].toMap()["icons_url"].toString();
#endif

    prepareUploadActions();
}

void TTRssFetcher::startFetching()
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    if (!db->makeBackup ()) {
        qWarning() << "Unable to make DB backup!";
        emit error(506);
        setBusy(false);
        return;
    }

    db->cleanDashboards();

    DatabaseManager::Dashboard d;
    d.id = "ttrss";
    d.name = "Default";
    d.title = "Default";
    d.description = "Tiny Tiny Rss default dashboard";
    db->writeDashboard(d);
    s->setDashboardInUse(d.id);

    db->cleanTabs();
    db->cleanStreams();
    db->cleanModules();

    if(busyType == Fetcher::Initiating) {
        db->cleanCache();
        db->cleanEntries();
    }

    commandList.clear();
    commandList.append(&TTRssFetcher::fetchCategories);
    commandList.append(&TTRssFetcher::fetchFeeds);
    commandList.append(&TTRssFetcher::fetchStream);
    commandList.append(&TTRssFetcher::fetchStarredStream);
    commandList.append(&TTRssFetcher::fetchPublishedStream);
    commandList.append(&TTRssFetcher::pruneOld);

    proggressTotal = commandList.size() + s->getRetentionDays();
    proggress = 0;
    lastDate = 0;
    offset = 0;

    callNextCmd();
}

void TTRssFetcher::fetchCategories()
{
    sendApiCall("getCategories", FETCHER_SLOT(finishedCategories));
}

void TTRssFetcher::finishedCategories()
{
    if (!processResponse()) {
        return;
    }

    startJob(StoreCategories);
}

void TTRssFetcher::fetchFeeds()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonObject params;
    params["cat_id"] = -3;
#else
    QString params = "\"cat_id\":-3";
#endif

    sendApiCall("getFeeds", params, FETCHER_SLOT(finishedFeeds));
}

void TTRssFetcher::finishedFeeds()
{
    if (!processResponse()) {
        return;
    }

    startJob(StoreFeeds);
}

void TTRssFetcher::fetchStream()
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    if (offset == 0) {
        db->updateEntriesFlag(1);
    }

    getHeadlines(AllArticles, true, !s->getSyncRead(), offset, FETCHER_SLOT(finishedStream));
}

void TTRssFetcher::finishedStream()
{
    if (!processResponse()) {
        return;
    }

    startJob(StoreStream);
}

void TTRssFetcher::finishedStream2()
{
    Settings *s = Settings::instance();
    if ((s->getRetentionDays() > 0 && lastDate > s->getRetentionDays()) ||
        lastCount < streamLimit) {
        offset = 0;
        lastDate = s->getRetentionDays();
    } else {
        offset += lastCount;
        commandList.prepend(currentCommand);
    }

    callNextCmd();
}

void TTRssFetcher::fetchStarredStream()
{
    getHeadlines(Starred, true, false, offset, FETCHER_SLOT(finishedStream));
}

void TTRssFetcher::fetchPublishedStream()
{
    getHeadlines(Published, true, false, offset, FETCHER_SLOT(finishedStream));
}

void TTRssFetcher::pruneOld()
{
    DatabaseManager::instance()->removeEntriesByFlag(1);
    callNextCmd();
}

void TTRssFetcher::startJob(Job job)
{
    if (isRunning()) {
        qWarning() << "Job is running";
        return;
    }

    disconnect(this, SIGNAL(finished()), 0, 0);
    currentJob = job;

    switch (job) {
    case StoreCategories:
    case StoreFeeds:
        connect(this, SIGNAL(finished()), this, SLOT(callNextCmd()));
        break;
    case StoreStream:
        connect(this, SIGNAL(finished()), this, SLOT(finishedStream2()));
        break;
    default:
        qWarning() << "Unknown Job!";
        emit error(502);
        setBusy(false);
        return;
    }

    start(QThread::LowPriority);
}

void TTRssFetcher::run()
{
    switch (currentJob) {
    case StoreCategories:
        storeCategories();
        break;
    case StoreFeeds:
        storeFeeds();
        break;
    case StoreStream:
        storeStream();
        break;
    default:
        qWarning() << "Unknown Job!";
        break;
    }
}

void TTRssFetcher::storeCategories()
{
    auto db = DatabaseManager::instance();
    QString dashboardId = "ttrss";

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (jsonObj["content"].isArray()) {
        QJsonArray arr = jsonObj["content"].toArray();
        int end = arr.count();
#else
        if (jsonObj["content"].type()==QVariant::List) {
            QVariantList::const_iterator i = jsonObj["content"].toList().constBegin();
            QVariantList::const_iterator end = jsonObj["content"].toList().constEnd();
#endif
        for (int i = 0; i < end; ++i) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = arr.at(i).toObject();
#else
            QVariantMap obj = (*i).toMap();
#endif
            if (obj["id"].toInt() >= 0) {
                DatabaseManager::Tab t;
                t.id = obj["id"].isString() ? obj["id"].toString() : QString::number(obj["id"].toInt());
                t.dashboardId = dashboardId;
                t.title = obj["title"].toString();
                db->writeTab(t);
            }
        }
    } else {
        qWarning() << "No categories found!";
    }
}

void TTRssFetcher::storeFeeds()
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (jsonObj["content"].isArray()) {
        QJsonArray arr = jsonObj["content"].toArray();
        int end = arr.count();
#else
    if (jsonObj["content"].type()==QVariant::List) {
        QVariantList::const_iterator i = jsonObj["content"].toList().constBegin();
        QVariantList::const_iterator end = jsonObj["content"].toList().constEnd();
#endif
        for (int i = 0; i < end; ++i) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = arr.at(i).toObject();
#else
            QVariantMap obj = (*i).toMap();
#endif
            DatabaseManager::Stream st;
            st.id = obj["id"].isString() ? obj["id"].toString() : QString::number(obj["id"].toInt());
            st.title = obj["title"].toString();
            st.link = obj["feed_url"].toString();
            st.query = st.link;
            st.content = "";
            st.type = "";
            st.unread = 0;
            st.saved = 0;
            st.read = 0;
            st.slow = 0;
            st.newestItemAddedAt = obj["last_updated"].toInt();
            st.updateAt = st.newestItemAddedAt;
            st.lastUpdate = QDateTime::currentDateTimeUtc().toTime_t();
            if (obj["has_icon"].toBool()) {
                st.icon = s->getUrl() + "/" + iconsUrl + "/" + st.id + ".ico";
                DatabaseManager::CacheItem item;
                item.origUrl = st.icon;
                item.finalUrl = st.icon;
                item.type = "icon";
                emit addDownload(item);
            }

            db->writeStream(st);

            DatabaseManager::Module m;
            m.id = st.id;
            m.name = st.title;
            m.title = st.title;
            m.status = "";
            m.widgetId = "";
            m.pageId = "";
            m.tabId = obj["id"].isString() ? obj["cat_id"].toString() : QString::number(obj["cat_id"].toInt());
            m.streamList.append(st.id);
            db->writeModule(m);
        }
    } else {
        qWarning() << "No feeds found!";
    }
}

void TTRssFetcher::storeStream()
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();
    auto dm = DownloadManager::instance();

    int count = 0;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (jsonObj["content"].isArray()) {
        QJsonArray arr = jsonObj["content"].toArray();
        int end = arr.count();
        count = end;
#else
    if (jsonObj["content"].type()==QVariant::List) {
        QVariantList::const_iterator i = jsonObj["content"].toList().constBegin();
        QVariantList::const_iterator end = jsonObj["content"].toList().constEnd();
#endif

        for (int i = 0; i < arr.count(); ++i) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = arr.at(i).toObject();
#else
            QVariantMap obj = (*i).toMap();
            ++count;
#endif

            DatabaseManager::Entry e;
            e.id = obj["id"].isString() ? obj["id"].toString() : QString::number(obj["id"].toInt());
            e.streamId = obj["feed_id"].isString() ? obj["feed_id"].toString() : QString::number(obj["feed_id"].toInt());
            e.title = obj["title"].toString();
            e.author = obj["author"].toString();

            if (obj["content"].isString())
                e.content = obj["content"].toString();

            e.link = obj["link"].toString();
            e.read = obj["unread"].toBool() ? 0 : 1;
            e.saved = obj["marked"].toBool() ? 1 : 0;
            e.publishedAt = obj["updated"].toInt();
            e.createdAt = obj["updated"].toInt();
            e.cached = 0;
            e.fresh = 1;
            e.broadcast = obj["published"].toBool() ? 1 : 0;
            e.timestamp = obj["updated"].toInt();

            QRegExp rx("<img\\s[^>]*src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
            if (rx.indexIn(e.content)!=-1) {
                QString imgSrc = rx.cap(1); imgSrc = imgSrc.mid(1,imgSrc.length()-2);
                if (!imgSrc.isEmpty()) {
                    imgSrc.replace("&amp;","&", Qt::CaseInsensitive);
                    if (s->getCachingMode() == 2 || (s->getCachingMode() == 1 && dm->isWLANConnected())) {
                        if (!db->isCacheExistsByFinalUrl(Utils::hash(imgSrc))) {
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

            db->writeEntry(e);
            if (!e.saved && !e.broadcast && s->getRetentionDays() > 0) {
                int date = QDateTime::fromTime_t(e.timestamp).daysTo(QDateTime::currentDateTimeUtc());
                if (date > lastDate)
                    lastDate = date;
            }
        }

        lastCount = count;
    }
}

void TTRssFetcher::uploadActions()
{
    if (!actionsList.isEmpty()) {
        emit uploading();
        setAction();
    }
}

void TTRssFetcher::setAction()
{
    auto db = DatabaseManager::instance();
    DatabaseManager::Action action = actionsList.first();

    QString ids;
    int mode, field;

    switch (action.type)
    {
    case DatabaseManager::SetRead:
    case DatabaseManager::UnSetRead:
    {
        ids = action.id1.replace('&', ',');
        mode = action.type == DatabaseManager::SetRead ? 0 : 1;
        field = 2;
        break;
    }
    case DatabaseManager::SetSaved:
    case DatabaseManager::UnSetSaved:
    {
        ids = action.id1.replace('&', ',');
        mode = action.type == DatabaseManager::SetSaved ? 1 : 0;
        field = 0;
        break;
    }
    case DatabaseManager::SetBroadcast:
    case DatabaseManager::UnSetBroadcast:
    {
        ids = action.id1.replace('&', ',');
        mode = action.type == DatabaseManager::SetBroadcast ? 1 : 0;
        field = 1;
        break;
    }
    case DatabaseManager::SetStreamReadAll:
    case DatabaseManager::UnSetStreamReadAll:
    {
        ids = mergeEntryIds(db->readEntriesByStream(action.id1, 0, db->countEntriesByStream(action.id1)),
                            action.type == DatabaseManager::SetStreamReadAll);
        mode = action.type == DatabaseManager::SetStreamReadAll ? 0 : 1;
        field = 2;
        break;
    }
    case DatabaseManager::SetTabReadAll:
    case DatabaseManager::UnSetTabReadAll:
    {
        QList<QString> streams = db->readStreamIdsByTab(action.id1);
        for (int i = 0; i < streams.count(); ++i) {
            QString streamIds = mergeEntryIds(db->readEntriesByStream(streams[i], 0, db->countEntriesByStream(streams[i])),
                                              action.type == DatabaseManager::SetTabReadAll);
            if (!streamIds.isEmpty()) {
                if (!ids.isEmpty())
                    ids += ",";
                ids += streamIds;
            }
        }

        mode = action.type == DatabaseManager::SetTabReadAll ? 0 : 1;
        field = 2;
        break;
    }
    case DatabaseManager::SetAllRead:
    case DatabaseManager::UnSetAllRead:
    {
        QList<DatabaseManager::Stream> streams = db->readStreamsByDashboard(action.id1);
        for (int i = 0; i < streams.count(); ++i) {
            QString streamIds = mergeEntryIds(db->readEntriesByStream(streams[i].id, 0, db->countEntriesByStream(streams[i].id)),
                                              action.type == DatabaseManager::SetAllRead);
            if (!streamIds.isEmpty()) {
                if (!ids.isEmpty())
                    ids += ",";
                ids += streamIds;
            }
        }

        mode = action.type == DatabaseManager::SetAllRead ? 0 : 1;
        field = 2;
        break;
    }
    case DatabaseManager::SetListRead:
    case DatabaseManager::UnSetListRead:
    {
        ids = action.id1.replace('&', ',');
        mode = action.type == DatabaseManager::SetListRead ? 0 : 1;
        field = 2;
        break;
    }
    case DatabaseManager::SetListSaved:
    case DatabaseManager::UnSetListSaved:
    {
        ids = action.id1.replace('&', ',');
        mode = action.type == DatabaseManager::SetListSaved ? 1 : 0;
        field = 1;
        break;
    }
    default:
        qWarning() << "Unknown action type: " << action.type;
        finishedSetAction();
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonObject params;
    params["article_ids"] = ids;
    params["mode"] = mode;
    params["field"] = field;
#else
    QString params = "\"article_ids\":\"" + ids + "\",\"mode\":" + QString::number(mode) +
        ",\"field\":" + QString::number(field);
#endif

    sendApiCall("updateArticle", params, FETCHER_SLOT(finishedSetAction));
}

void TTRssFetcher::finishedSetAction()
{
    if (!processResponse()) {
        return;
    }

    auto db = DatabaseManager::instance();

    DatabaseManager::Action action = actionsList.takeFirst();
    db->removeActionsByIdAndType(action.id1, action.type);

    emit uploadProgress(uploadProggressTotal - actionsList.size(), uploadProggressTotal);

    if (actionsList.isEmpty()) {
        startFetching();
    } else {
        setAction();
    }
}

void TTRssFetcher::callNextCmd()
{
    if (!commandList.isEmpty()) {
        proggress = proggressTotal - commandList.size() - Settings::instance()->getRetentionDays() + lastDate;
        emit progress(proggress, proggressTotal);
        currentCommand = commandList.takeFirst();
        (this->*currentCommand)();
    } else {
        taskEnd();
    }
}

QString TTRssFetcher::mergeEntryIds(const QList<DatabaseManager::Entry>& entries, bool read)
{
    QString ids;

    for (int i = 0; i < entries.count(); ++i) {
        if (entries[i].read == read) {
            if (!ids.isEmpty())
                ids += ",";
            ids += entries[i].id;
        }
    }

    return ids;
}

void TTRssFetcher::getHeadlines(int feedId, bool getContent, bool unreadOnly, int offset, ReplyCallback callback)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonObject params;
    params["feed_id"] = feedId;
    params["show_content"] = getContent;
    params["view_mode"] = unreadOnly ? "unread" : "all_articles";
    params["include_attachments"] = getContent;
    params["order_by"] = "feed_dates";
    params["skip"] = offset;
    params["limit"] = streamLimit;
#else
    QString params = "\"feed_id\":" + QString::number(feedId) + "," +
        "\"show_content\":" + (getContent ? "true," : "false,") +
        "\"show_excerpt\":false," +
        "\"view_mode\":" + (unreadOnly ? "\"unread\"," : "\"all_articles\",") +
        "\"include_attachments\":" + (getContent ? "true," : "false,") +
        "\"order_by\":\"feed_dates\"," +
        "\"skip\":" + QString::number(offset) + "," +
        "\"limit\":" + QString::number(streamLimit);
#endif

    sendApiCall("getHeadlines", params, callback);
}

void TTRssFetcher::sendApiCall(const QString& op, ReplyCallback callback)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonObject params;
#else
    QString params;
#endif
    sendApiCall(op, params, callback);
}

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void TTRssFetcher::sendApiCall(const QString& op, const QJsonObject& params, ReplyCallback callback)
#else
void TTRssFetcher::sendApiCall(const QString& op, const QString& params, ReplyCallback callback);
#endif
{
    data.clear();

    if (currentReply != NULL) {
        currentReply->disconnect();
        currentReply->deleteLater();
        currentReply = NULL;
    }

    QNetworkRequest request(QUrl(Settings::instance()->getUrl() + "/api/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonObject obj (params);
    if (!sessionId.isEmpty()) {
        obj["sid"] = sessionId;
    }
    obj["op"] = op;

    QString body = QJsonDocument(obj).toJson(QJsonDocument::Compact);
#else
    QString body = "{\"op\":\"" + op + "\"" + (!params.isEmpty() ? params : "") + "}";
#endif

    currentReply = nam.post(request, body.toUtf8());
    connect(currentReply, &QNetworkReply::finished, this, callback);
    connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
#ifndef QT_NO_SSL
    connect(currentReply, &QNetworkReply::sslErrors, this, &Fetcher::sslErrors);
#endif
}

bool TTRssFetcher::processResponse()
{
    auto e = currentReply->error();
    if (e != QNetworkReply::NoError &&
        e != QNetworkReply::OperationCanceledError) {
        qDebug() << "Request error:" << e;
        if (e == QNetworkReply::SslHandshakeFailedError) {
            emit error(700);
        } else {
            emit error(500);
        }
        setBusy(false);
        return false;
    }

    if (!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return false;
    } else if (jsonObj["status"].toInt() != 0) {
        QString err;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        err = jsonObj["content"].toObject()["error"].toString();
#else
        err = jsonObj["content"].toMap()["error"].toString();
#endif
        qWarning() << "Error: " << err;
        if (err == "LOGIN_ERROR") {
            if (busyType == Fetcher::CheckingCredentials) {
                emit errorCheckingCredentials(501);
            } else {
                emit error(402);
            }
        } else if (err == "NOT_LOGGED_IN") {
            emit error(401);
        } else if (err == "API_DISABLED") {
            emit error(404);
        } else {
            emit error(601);
        }
        setBusy(false);
        return false;
    }

    return true;
}
