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

#include <QAbstractListModel>
#include <QBuffer>
#include <QUrl>
#include <QDebug>
#include <QModelIndex>
#include <QDateTime>
#include <QRegExp>
#include <QNetworkConfiguration>
#include <QtCore/qmath.h>
#include <QCryptographicHash>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#else
#include "qjson.h"
#endif

#include "netvibesfetcher.h"

NetvibesFetcher::NetvibesFetcher(QObject *parent) :
    QThread(parent)
{
    _currentReply = NULL;
    _busy = false;
    _busyType = Unknown;

    Settings *s = Settings::instance();

    connect(&_manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));

    connect(this, SIGNAL(addDownload(DatabaseManager::CacheItem)), s->dm, SLOT(addDownload(DatabaseManager::CacheItem)));
}

bool NetvibesFetcher::delayedUpdate(bool state)
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

    switch (_busyType) {
    case InitiatingWaiting:
        setBusy(true, Initiating);
        break;
    case UpdatingWaiting:
        setBusy(true, Updating);
        break;
    case CheckingCredentialsWaiting:
        setBusy(true, CheckingCredentials);
        break;
    default:
        qWarning() << "Wrong busy state!";
        setBusy(false);
        return false;
    }

    _streamList.clear();
    //_moduleList.clear();
    //_feedTabList.clear();
    actionsList.clear();
    signIn();

    return true;
}

bool NetvibesFetcher::init()
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        return false;
    }

#ifdef ONLINE_CHECK
    if (!ncm.isOnline()) {
        qDebug() << "Network is Offline. Waiting...";
        //emit networkNotAccessible();
        setBusy(true, InitiatingWaiting);
        connect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));
        return true;
    }
#endif

    setBusy(true, Initiating);
    _streamList.clear();
    //_moduleList.clear();
    //_feedTabList.clear();

    signIn();
    return true;
}

bool NetvibesFetcher::isBusy()
{
    return _busy;
}

NetvibesFetcher::BusyType NetvibesFetcher::readBusyType()
{
    return _busyType;
}

void NetvibesFetcher::setBusy(bool busy, BusyType type)
{
    _busyType = type;
    _busy = busy;

    if (!busy)
        _busyType = Unknown;

    emit busyChanged();
}

void NetvibesFetcher::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (_busy) {
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

bool NetvibesFetcher::update()
{
    if (_busy) {
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
            setBusy(true, InitiatingWaiting);
        } else {
            setBusy(true, UpdatingWaiting);
        }
        connect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));
        return true;
    }
#endif

    if (streamsCount == 0 || entriesCount == 0 || tabsCount == 0) {
        setBusy(true, Initiating);
    } else {
        setBusy(true, Updating);
    }

    _streamList.clear();
    //_moduleList.clear();
    actionsList.clear();

    signIn();
    emit progress(0,100);
    return true;
}

bool NetvibesFetcher::checkCredentials()
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        return false;
    }

#ifdef ONLINE_CHECK
    if (!ncm.isOnline()) {
        qDebug() << "Network is Offline. Waiting...";
        setBusy(true, CheckingCredentialsWaiting);
        connect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));
        return true;
    }
#endif

    setBusy(true, CheckingCredentials);

    signIn();

    return true;
}

void NetvibesFetcher::signIn()
{
    _data.clear();

    Settings *s = Settings::instance();
    QString password = s->getNetvibesPassword();
    QString username = s->getNetvibesUsername();

    if (password == "" || username == "") {
        qWarning() << "Netvibes username & password do not match!";
        if (_busyType == CheckingCredentials)
            emit errorCheckingCredentials(400);
        else
            emit error(400);
        setBusy(false);
        return;
    }

    QUrl url("http://www.netvibes.com/api/auth/signin");

    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    QString body = "email="+QUrl::toPercentEncoding(username)+"&password="+QUrl::toPercentEncoding(password)+"&session_only=1";
    _currentReply = _manager.post(request,body.toUtf8());

    if (_busyType == CheckingCredentials)
        connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedSignInOnlyCheck()));
    else
        connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedSignIn()));

    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchDashboards()
{
    _data.clear();

    QUrl url("http://www.netvibes.com/api/my/dashboards");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);
    _currentReply = _manager.post(request,"format=json");
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedDashboards()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::set()
{
    DatabaseManager::Action action = actionsList.first();

    _data.clear();

    QUrl url;

    switch (action.type) {
    case DatabaseManager::SetRead:
        url.setUrl("http://www.netvibes.com/api/streams/read/add");
        break;
    case DatabaseManager::SetSaved:
        url.setUrl("http://www.netvibes.com/api/streams/saved/add");
        break;
    case DatabaseManager::UnSetRead:
        url.setUrl("http://www.netvibes.com/api/streams/read/remove");
        break;
    case DatabaseManager::UnSetSaved:
        url.setUrl("http://www.netvibes.com/api/streams/saved/remove");
        break;
    case DatabaseManager::UnSetStreamReadAll:
        url.setUrl("http://www.netvibes.com/api/streams/read/remove");
        break;
    case DatabaseManager::SetStreamReadAll:
        url.setUrl("http://www.netvibes.com/api/streams/read/add");
        break;
    case DatabaseManager::UnSetTabReadAll:
        url.setUrl("http://www.netvibes.com/api/streams/read/remove");
        break;
    case DatabaseManager::SetTabReadAll:
        url.setUrl("http://www.netvibes.com/api/streams/read/add");
        break;
    case DatabaseManager::UnSetAllRead:
        url.setUrl("http://www.netvibes.com/api/streams/read/remove");
        break;
    case DatabaseManager::SetAllRead:
        url.setUrl("http://www.netvibes.com/api/streams/read/add");
        break;
    case DatabaseManager::UnSetSlowRead:
        url.setUrl("http://www.netvibes.com/api/streams/read/remove");
        break;
    case DatabaseManager::SetSlowRead:
        url.setUrl("http://www.netvibes.com/api/streams/read/add");
        break;
    }

    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    Settings *s = Settings::instance();
    QString actions = "[";

    if (action.type == DatabaseManager::SetTabReadAll||
        action.type == DatabaseManager::UnSetTabReadAll) {

        QList<DatabaseManager::StreamModuleTab> list = s->db->readStreamModuleTabListByTab(action.id1);

        if (list.empty()) {
            qWarning() << "No streams found!";
            emit error(502);
            return;
        }

        actions += "{\"options\":{},\"streams\":[";

        QList<DatabaseManager::StreamModuleTab>::iterator i = list.begin();
        while (i != list.end()) {
            if (i != list.begin())
                actions  += ",";

            actions += QString("{\"id\":\"%1\",\"moduleId\":\"%2\"}").arg((*i).streamId).arg((*i).moduleId);

            ++i;
        }

        actions += "]}";
    }

    if (action.type == DatabaseManager::SetAllRead ||
        action.type == DatabaseManager::UnSetAllRead) {

        QList<DatabaseManager::StreamModuleTab> list = s->db->readStreamModuleTabListByDashboard(s->getDashboardInUse());

        if (list.empty()) {
            qWarning() << "No streams found!";
            emit error(502);
            return;
        }

        actions += "{\"options\":{},\"streams\":[";

        QList<DatabaseManager::StreamModuleTab>::iterator i = list.begin();
        while (i != list.end()) {
            if (i != list.begin())
                actions  += ",";

            actions += QString("{\"id\":\"%1\",\"moduleId\":\"%2\"}").arg((*i).streamId).arg((*i).moduleId);

            ++i;
        }

        actions += "]}";
    }

    if (action.type == DatabaseManager::SetSlowRead ||
        action.type == DatabaseManager::UnSetSlowRead) {

        QList<DatabaseManager::StreamModuleTab> list = s->db->readSlowStreamModuleTabListByDashboard(s->getDashboardInUse());

        if (list.empty()) {
            qWarning() << "No streams found!";
            emit error(502);
            return;
        }

        actions += "{\"options\":{},\"streams\":[";

        QList<DatabaseManager::StreamModuleTab>::iterator i = list.begin();
        while (i != list.end()) {
            if (i != list.begin())
                actions  += ",";

            actions += QString("{\"id\":\"%1\",\"moduleId\":\"%2\"}").arg((*i).streamId).arg((*i).moduleId);

            ++i;
        }

        actions += "]}";
    }

    if (action.type == DatabaseManager::SetStreamReadAll ||
        action.type == DatabaseManager::UnSetStreamReadAll) {

        actions += "{\"options\":{},\"streams\":[";
        actions += QString("{\"id\":\"%1\"}").arg(action.id1);
        actions += "]}";
    }

    if (action.type == DatabaseManager::SetRead ||
        action.type == DatabaseManager::UnSetRead ||
        action.type == DatabaseManager::SetSaved ||
        action.type == DatabaseManager::UnSetSaved ) {

        if (action.date1==0) {
            qWarning() << "PublishedAt date is 0!";
        }

        actions += QString("{\"streams\":[{\"id\":\"%1\",\"items\":[{"
                          "\"id\":\"%2\",\"publishedAt\":%3}]}]}")
                .arg(action.id2).arg(action.id1).arg(action.date1);
    }

    actions += "]";

    //qDebug() << "actions=" << actions;
    QString content = "actions="+QUrl::toPercentEncoding(actions)+"&pageId="+s->getDashboardInUse();
    //qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedSet()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchTabs()
{
    _data.clear();

    QString dashbordId = _dashboardList.first();

    QUrl url("http://www.netvibes.com/api/my/dashboards/data");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);
    _currentReply = _manager.post(request,"format=json&pageId="+dashbordId.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedTabs()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchFeeds()
{
    _data.clear();

    QUrl url("http://www.netvibes.com/api/streams");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    Settings *s = Settings::instance();
    int feedsAtOnce = s->getFeedsAtOnce();

    int ii = 0;
    QString actions = "[";
    QList<DatabaseManager::StreamModuleTab>::iterator i = _streamList.begin();
    while (i != _streamList.end()) {
        if (ii > feedsAtOnce)
            break;

        if (ii != 0)
            actions += ",";

        actions += QString("{\"options\":{\"limit\":%1},\"streams\":[{\"id\":\"%2\",\"moduleId\":\"%3\"}]}")
                .arg(limitFeeds)
                .arg((*i).streamId)
                .arg((*i).moduleId);

        i = _streamList.erase(i);
        ++ii;
    }
    actions += "]";

    //qDebug() << "actions=" << actions;
    QString content = "actions="+QUrl::toPercentEncoding(actions)+"&pageId="+s->getDashboardInUse();
    //qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedFeeds()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchFeedsReadlater()
{
    _data.clear();

    QUrl url("http://www.netvibes.com/api/streams/saved");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    Settings *s = Settings::instance();

    QString actions;

    if (publishedBeforeDate==0) {
        actions = QString("[{\"options\":{\"limit\":%1}}]").arg(limitFeedsReadlater);

    } else {
        actions = QString("[{\"options\":{\"limit\":%1, "
                          "\"publishedBeforeDate\":%2"
                          "}}]")
                .arg(limitFeedsReadlater)
                .arg(publishedBeforeDate);
    }

    //qDebug() << "actions=" << actions;
    QString content = "actions="+QUrl::toPercentEncoding(actions)+"&pageId="+s->getDashboardInUse();
    //qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedFeedsReadlater()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchFeedsUpdate()
{
    _data.clear();

    QUrl url("http://www.netvibes.com/api/streams");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    Settings *s = Settings::instance();
    int feedsUpdateAtOnce = s->getFeedsUpdateAtOnce();

    int ii = 0;

    QString actions = "[";

    QList<DatabaseManager::StreamModuleTab>::iterator i = _streamUpdateList.begin();
    while (i != _streamUpdateList.end()) {
        if (ii >= feedsUpdateAtOnce)
            break;

        if (ii != 0)
            actions += ",";

        actions += QString("{\"options\":{\"limit\":%1},\"crawledAfterDate\":%2,"
                           "\"streams\":[{\"id\":\"%3\",\"moduleId\":\"%4\"}]}")
                .arg(limitFeedsUpdate)
                .arg((*i).date)
                .arg((*i).streamId)
                .arg((*i).moduleId);

        i = _streamUpdateList.erase(i);
        ++ii;
    }
    actions += "]";

    //qDebug() << "actions=" << actions;
    QString content = "actions="+QUrl::toPercentEncoding(actions)+"&pageId="+s->getDashboardInUse();
    //qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedFeedsUpdate()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::readyRead()
{
    int statusCode = _currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300) {
        _data += _currentReply->readAll();
    }
}

bool NetvibesFetcher::parse()
{

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QJsonDocument doc = QJsonDocument::fromJson(this->_data);
    if (!doc.isObject()) {
        qWarning() << "Json doc is empty!";
        return false;
    }
    _jsonObj = doc.object();
#else
    QJson qjson(this);
    bool ok;
    _jsonObj = qjson.parse(this->_data, &ok).toMap();
    if (!ok) {
        qWarning() << "An error occurred during parsing Json!";
        return false;
    }
    if (_jsonObj.empty()) {
        qWarning() << "Json doc is empty!";
        return false;
    }
#endif

    return true;
}

void NetvibesFetcher::storeTabs()
{
    if (checkError()) {
        return;
    }

    Settings *s = Settings::instance();
    QString dashboardId = _dashboardList.takeFirst();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (_jsonObj["userData"].toObject()["tabs"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["userData"].toObject()["tabs"].toArray().constBegin();
        QJsonArray::const_iterator end = _jsonObj["userData"].toObject()["tabs"].toArray().constEnd();
#else
    if (_jsonObj["userData"].toMap()["tabs"].type()==QVariant::List) {
        QVariantList::const_iterator i = _jsonObj["userData"].toMap()["tabs"].toList().constBegin();
        QVariantList::const_iterator end = _jsonObj["userData"].toMap()["tabs"].toList().constEnd();
#endif
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = (*i).toObject();
#else
            QVariantMap obj = (*i).toMap();
#endif
            DatabaseManager::Tab t;
            t.id = obj["id"].toString();
            t.dashboardId = dashboardId;
            t.icon = obj["icon"].toString();
            t.title = obj["title"].toString();

            s->db->writeTab(t);
            _tabList.append(t.id);

            //qDebug() << "Writing tab: " << t.id << t.title;

            // Downloading icon file
            if (t.icon!="") {
                DatabaseManager::CacheItem item;
                item.origUrl = t.icon;
                item.finalUrl = t.icon;
                emit addDownload(item);
                //qDebug() << "icon:" << t.icon;
            }

            ++i;
        }
    }  else {
        qWarning() << "No \"tabs\" element found!";
    }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (_jsonObj["userData"].toObject()["modules"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["userData"].toObject()["modules"].toArray().constBegin();
        QJsonArray::const_iterator end = _jsonObj["userData"].toObject()["modules"].toArray().constEnd();
#else
    if (_jsonObj["userData"].toMap()["modules"].type()==QVariant::List) {
        QVariantList::const_iterator i = _jsonObj["userData"].toMap()["modules"].toList().constBegin();
        QVariantList::const_iterator end = _jsonObj["userData"].toMap()["modules"].toList().constEnd();
#endif
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = (*i).toObject();

            if (obj["name"].toString() == "RssReader" ||
                    obj["name"].toString() == "MultipleFeeds") {

                // Module
                DatabaseManager::Module m;
                m.id = obj["id"].toString();
                m.name = obj["name"].toString();
                m.title = obj["title"].toString();
                m.status = obj["status"].toString();
                m.widgetId = obj["widgetId"].toString();
                m.pageId = obj["pageId"].toString();
                m.tabId = obj["tab"].toString();

                if (obj["streams"].isArray()) {
                    QJsonArray::const_iterator mi = obj["streams"].toArray().constBegin();
                    QJsonArray::const_iterator mend = obj["streams"].toArray().constEnd();
                    while (mi != mend) {
                        QJsonObject mobj = (*mi).toObject();
                        DatabaseManager::StreamModuleTab smt;
                        smt.streamId = mobj["id"].toString();
                        smt.moduleId = m.id;
                        smt.tabId = obj["tab"].toString();
                        _streamList.append(smt);
                        m.streamList.append(smt.streamId);
                        //_streamList.insertMulti(streamId,tabId);
                        //_moduleList.insertMulti(streamId,m.id);
                        //qDebug() << "Adding stream:" << streamId << m.title;
                        //_feedTabList.insert(streamId,tabId);
                        //qDebug() << "Writing module: " << smt.moduleId << m.title << "streamId:" << smt.streamId;
                        ++mi;
                    }
                } else {
                    qWarning() << "Module"<<m.id<<"without streams!";
                }


                s->db->writeModule(m);
            }
#else
            //TO-DO
            QVariantMap obj = (*i).toMap();
            if (obj["name"].toString() == "RssReader") {
                QString streamId = obj["data"].toMap()["streamIds"].toString();
                QString tabId = obj["tab"].toString();
                _feedTabList.insert(streamId,tabId);
                _feedList.insert(streamId,tabId);
            }
#endif
            ++i;
        }
    }  else {
        qWarning() << "No modules element found!";
    }
}

int NetvibesFetcher::storeFeeds()
{
    if (checkError()) {
        return 0;
    }

    Settings *s = Settings::instance();
    int entriesCount = 0;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)

    if (_jsonObj["results"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["results"].toArray().constBegin();
        QJsonArray::const_iterator end = _jsonObj["results"].toArray().constEnd();
#else
    if (_jsonObj["results"].type()==QVariant::List) {
        QVariantList::const_iterator i = _jsonObj["results"].toList().constBegin();
        QVariantList::const_iterator end = _jsonObj["results"].toList().constEnd();
#endif
        while (i != end) {
            //qDebug() << "i" << i.i;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            if ((*i).isObject()) {
                if ((*i).toObject()["streams"].isArray()) {
                    QJsonArray::const_iterator ai = (*i).toObject()["streams"].toArray().constBegin();
                    QJsonArray::const_iterator aend = (*i).toObject()["streams"].toArray().constEnd();
                    while (ai != aend) {
                        //qDebug() << "ai" << ai.i;
                        QJsonObject obj = (*ai).toObject();

                        //Check for errors
                        if (obj["error"].isObject()) {
                            qWarning() << "Nested error in Netvibes response!";
                            qWarning() << "Code:" << (int) obj["error"].toObject()["code"].toDouble();
                            qWarning() << "Message:" << obj["error"].toObject()["message"].toString();

                        } else {

                            /*int unread = 0;
                            if (obj["flags"].isObject()) {
                                if (obj["flags"].toObject().contains("unread"))
                                    unread = (int) obj["flags"].toObject()["unread"].toDouble();
                            }*/

                            int slow = 0;
                            if (obj.contains("slow"))
                                slow = obj["slow"].toBool() ? 1 : 0;

                            DatabaseManager::Stream st;
                            st.id = obj["id"].toString();

                            /*QList<DatabaseManager::StreamModuleTab>::iterator smti = _streamList.begin();
                            while (smti != _streamList.end()) {
                                qDebug() << "search| streamId > moduleId > tabId" << (*smti).streamId << (*smti).moduleId << (*smti).tabId;
                                if ((*smti).streamId == st.id)
                                    st.moduleId = (*smti).moduleId;
                                ++smti;
                            }
                            if (st.moduleId=="")
                                qWarning() << "Stream moduleId is Empty!";*/

                            st.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
                            st.link = obj["link"].toString();
                            st.query = obj["query"].toString();
                            st.content = obj["content"].toString();
                            st.type = obj["type"].toString();
                            st.unread = 0;
                            st.read = 0;
                            st.slow = slow;
                            st.newestItemAddedAt = (int) obj["newestItemAddedAt"].toDouble();
                            st.updateAt = (int) obj["updateAt"].toDouble();
                            st.lastUpdate = QDateTime::currentDateTimeUtc().toTime_t();

                            /*qDebug() << ">>>>>>> Feed <<<<<<<";
                            qDebug() << "id" << obj["id"].toString();
                            qDebug() << "title" << obj["title"].toString();
                            qDebug() << "query" << obj["query"].toString();
                            qDebug() << "queue" << obj["queue"].toString();
                            qDebug() << "content" << obj["content"].toString();
                            qDebug() << "status" << obj["status"].toString();
                            qDebug() << "slow" << obj["slow"].toBool();
                            qDebug() << "type" << obj["type"].toString();*/

                            /*QMap<QString,QString>::iterator it = _feedTabList.find(f.id);
                            if (it!=_feedTabList.end()) {
                                // Downloading fav icon file
                                if (s.link!="") {
                                    QUrl iconUrl(s.link);
                                    s.icon = QString("http://avatars.netvibes.com/favicon/%1://%2")
                                            .arg(iconUrl.scheme())
                                            .arg(iconUrl.host());
                                    DatabaseManager::CacheItem item;
                                    item.origUrl = f.icon;
                                    item.finalUrl = f.icon;
                                    //s->dm->addDownload(item);
                                    emit addDownload(item);
                                    //qDebug() << "favicon:" << f.icon;
                                }
                                s->db->writeFeed(it.value(), f);

                            } else {
                                qWarning() << "No matching feed!";
                            }*/

                            // Downloading fav icon file
                            if (st.link!="") {
                                QUrl iconUrl(st.link);
                                st.icon = QString("http://avatars.netvibes.com/favicon/%1://%2")
                                        .arg(iconUrl.scheme())
                                        .arg(iconUrl.host());
                                DatabaseManager::CacheItem item;
                                item.origUrl = st.icon;
                                item.finalUrl = st.icon;
                                emit addDownload(item);
                                //qDebug() << "favicon:" << st.icon;
                            }

                            s->db->writeStream(st);

                            //qDebug() << "Writing stream: " << st.id << st.title;
                        }

                        ++ai;
                    }
                } else {
                    qWarning() << "No \"streams\" element found!";
                }

                if ((*i).toObject()["items"].isArray()) {
                    QJsonArray::const_iterator ai = (*i).toObject()["items"].toArray().constBegin();
                    QJsonArray::const_iterator aend = (*i).toObject()["items"].toArray().constEnd();
                    while (ai != aend) {
                        QJsonObject obj = (*ai).toObject();

                        int read = 1; int saved = 0;
                        if (obj["flags"].isObject()) {

                            if (!obj["flags"].toObject().contains("read")) {
                                read = 2;
                            } else {
                                read = obj["flags"].toObject()["read"].toBool() ? 1 : 0;
                            }

                            if (!obj["flags"].toObject().contains("saved")) {
                                saved = 0;
                            } else {
                                saved = obj["flags"].toObject()["saved"].toBool() ? 1 : 0;
                            }
                        }

                        QString image = "";
                        if (obj["enclosures"].isArray()) {
                            if (!obj["enclosures"].toArray().empty()) {
                                QString link = obj["enclosures"].toArray()[0].toObject()["link"].toString();
                                QString type = obj["enclosures"].toArray()[0].toObject()["type"].toString();
                                if (type=="image"||type=="html")
                                    image = link;
                            }
                        }

                        QString author = "";
                        if (obj["authors"].isArray()) {
                            if (!obj["authors"].toArray().empty())
                                author = obj["authors"].toArray()[0].toObject()["name"].toString();
                        }

                        if (obj["authors"].isArray()) {
                            if (!obj["authors"].toArray().empty())
                                author = obj["authors"].toArray()[0].toObject()["name"].toString();
                        }

                        DatabaseManager::Entry e;
                        e.id = obj["id"].toString();
                        e.streamId = obj["stream"].toObject()["id"].toString();
                        //e.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
                        e.title = obj["title"].toString();
                        e.author = author;
                        e.link = obj["link"].toString();
                        e.image = image;
                        e.content = obj["content"].toString();
                        e.read = read;
                        e.saved = saved;
                        e.cached = 0;
                        e.publishedAt = obj["publishedAt"].toDouble();
                        e.createdAt = obj["createdAt"].toDouble();
                        e.fresh = 1;

                        // Downloading image file
                        if (s->getAutoDownloadOnUpdate()) {
                            if (image!="") {
                                // Image provided by Netvibes API :-)
                                if (!s->db->isCacheExistsByFinalUrl(hash(image))) {
                                    DatabaseManager::CacheItem item;
                                    item.origUrl = image;
                                    item.finalUrl = image;
                                    emit addDownload(item);
                                }
                            } else {
                                // Checking if content contains image
                                QRegExp rx("<img\\s[^>]*src\\s*=\\s*(\"[^\"]*\"|'[^']*')", Qt::CaseInsensitive);
                                if (rx.indexIn(e.content)!=-1) {
                                    QString imgSrc = rx.cap(1); imgSrc = imgSrc.mid(1,imgSrc.length()-2);
                                    if (imgSrc!="") {
                                        if (!s->db->isCacheExistsByFinalUrl(hash(imgSrc))) {
                                            DatabaseManager::CacheItem item;
                                            item.origUrl = imgSrc;
                                            item.finalUrl = imgSrc;
                                            emit addDownload(item);
                                        }
                                        e.image = imgSrc;
                                        //qDebug() << "cap:" << imgSrc;
                                    }
                                }
                            }
                        }

                        /*qDebug() << ">>>>>>> Entry <<<<<<<";
                        qDebug() << "id" << obj["id"].toString();
                        qDebug() << "title" << obj["title"].toString();
                        qDebug() << "stream id" << streamId;
                        qDebug() << "image" << image;*/

                        s->db->writeEntry(e);
                        ++entriesCount;

                        if (e.publishedAt>0)
                            publishedBeforeDate = e.publishedAt;

                        ++ai;
                    }
                } else {
                    qWarning() << "No \"items\" element found!";
                }
            }
#else
            QVariantMap obj = (*i).toMap();
            int unread = obj["flags"].toMap()["unread"].toDouble();
            //int read = obj["flags"].toMap()["read"].toDouble();
            //int readlater = obj["flags"].toMap()["readlater"].toDouble();
#endif
            ++i;
        }
    }  else {
        qWarning() << "No \"relults\" element found!";
    }

    //qDebug() << "publishedBeforeDate:" << publishedBeforeDate;
    //qDebug() << "publishedBeforeItemId:" << publishedBeforeItemId;
    //qDebug() << "publishedBeforeStreamId:" << publishedBeforeStreamId;
    //qDebug() << "entriesCount:" << entriesCount;

    return entriesCount;
}

void NetvibesFetcher::storeDashboards()
{
    if (checkError()) {
        return;
    }

    Settings *s = Settings::instance();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (_jsonObj["dashboards"].isObject()) {
#else
    if (_jsonObj["dashboards"].type()==QVariant::Map) {
#endif
        // Set default dashboard if not set
        QString defaultDashboardId = s->getDashboardInUse();
        int lowestDashboardId = 99999999;
        bool defaultDashboardIdExists = false;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        QJsonObject::const_iterator i = _jsonObj["dashboards"].toObject().constBegin();
        QJsonObject::const_iterator end = _jsonObj["dashboards"].toObject().constEnd();
#else
        QVariantMap::const_iterator i = _jsonObj["dashboards"].toMap().constBegin();
        QVariantMap::const_iterator end = _jsonObj["dashboards"].toMap().constEnd();
#endif
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = i.value().toObject();
#else
            QVariantMap obj = i.value().toMap();
#endif
            if (obj["active"].toString()=="1") {

                DatabaseManager::Dashboard d;
                d.id = obj["pageId"].toString();
                d.name = obj["name"].toString();
                d.title = obj["title"].toString();
                d.description = obj["description"].toString();
                s->db->writeDashboard(d);
                _dashboardList.append(d.id);

                //qDebug() << "Writing dashboard: " << d.title;

                // Search lowest id
                int iid = d.id.toInt();
                if (iid < lowestDashboardId)
                    lowestDashboardId = iid;
                if (defaultDashboardId == d.id)
                    defaultDashboardIdExists = true;
            }

            ++i;
        }

        // Set default dashboard if not set
        //qDebug() << "defaultDashboardId" << defaultDashboardId;
        //qDebug() << "defaultDashboardIdExists" << defaultDashboardIdExists;
        //qDebug() << "lowestDashboardId" << lowestDashboardId;
        if (defaultDashboardId=="" || defaultDashboardIdExists==false) {
            s->setDashboardInUse(QString::number(lowestDashboardId));
        }

    } else {
        qWarning() << "No dashboards element found!";
    }
}

void NetvibesFetcher::finishedSignInOnlyCheck()
{
    //qDebug() << this->_data;

    Settings *s = Settings::instance();

    if (parse()) {
        if (_jsonObj["success"].toBool()) {
            s->setSignedIn(true);
            emit credentialsValid();
            setBusy(false);
        } else {
            s->setSignedIn(false);
            QString message = _jsonObj["message"].toString();
            if (message == "nomatch")
                emit errorCheckingCredentials(402);
            else
                emit errorCheckingCredentials(401);
            setBusy(false);
            qWarning() << "SignIn check error, messsage: " << message;
        }
    } else {
        s->setSignedIn(false);
        qWarning() << "SignIn check error!";
        emit errorCheckingCredentials(501);
        setBusy(false);
    }
}

void NetvibesFetcher::finishedSignIn()
{
    //qDebug() << this->_data;

    Settings *s = Settings::instance();

    if (parse()) {
        if (_jsonObj["success"].toBool()) {

            s->setSignedIn(true);

            _cookie = _currentReply->rawHeader("Set-Cookie");

            // upload actions
            actionsList =s->db->readActions();
            if (actionsList.isEmpty()) {
                //qDebug() << "No actions to upload!";
                s->db->cleanDashboards();
                fetchDashboards();
            } else {
                //qDebug() << actionsList.count() << " actions to upload!";
                uploadActions();
            }

        } else {
            s->setSignedIn(false);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QString message = _jsonObj["error"].toObject()["message"].toString();
#else
            QString message = _jsonObj["error"].toMap()["message"].toString();
#endif
            if (message == "no match")
                emit error(402);
            else
                emit error(401);
            setBusy(false);
            qWarning() << "SignIn error, messsage: " << message;
        }
    } else {
        s->setSignedIn(false);

        qWarning() << "SignIn error!";
        emit error(501);
        setBusy(false);
    }
}

void NetvibesFetcher::finishedDashboards()
{
    //qDebug() << this->_data;
    startJob(StoreDashboards);
}

void NetvibesFetcher::finishedDashboards2()
{
    Settings *s = Settings::instance();

    if(!_dashboardList.isEmpty()) {
        s->db->cleanTabs();
        //s->db->cleanModules();

        // Create Modules and Cache structure
        if(_busyType == Initiating) {
            s->db->cleanCache();
            s->db->cleanModules();
        }

        fetchTabs();
    } else {
        qWarning() << "No Dashboards found!";
        taskEnd();
    }

}

void NetvibesFetcher::finishedTabs()
{
    //qDebug() << this->_data;
    startJob(StoreTabs);
}

void NetvibesFetcher::finishedTabs2()
{
    Settings *s = Settings::instance();
    int feedsAtOnce = s->getFeedsAtOnce();
    int feedsUpdateAtOnce = s->getFeedsUpdateAtOnce();

    if(!_dashboardList.isEmpty()) {
        fetchTabs();
    } else {
        if (_tabList.isEmpty()) {
            qWarning() << "No Tabs!";
        }
        if (_streamList.isEmpty()) {
            qWarning() << "No Streams!";
            taskEnd();
        } else {
            if (_busyType == Updating) {
                // Set current entries as not fresh
                s->db->updateEntriesFreshFlag(0);

                cleanRemovedFeeds();
                cleanNewFeeds();

                _streamUpdateList = s->db->readStreamModuleTabList();

                if (_streamList.isEmpty()) {
                    qDebug() << "No new Feeds!";
                    _total = qCeil(_streamUpdateList.count()/feedsUpdateAtOnce)+3;
                    emit progress(3,_total);
                    fetchFeedsUpdate();
                } else {
                    _total = qCeil(_streamUpdateList.count()/feedsUpdateAtOnce)+qCeil(_streamList.count()/feedsAtOnce)+3;
                    emit progress(3,_total);
                    fetchFeeds();
                }
            }

            if (_busyType == Initiating) {
                s->db->cleanStreams();
                s->db->cleanEntries();
                //s->db->cleanCache();
                _total = qCeil(_streamList.count()/feedsAtOnce)+3;
                emit progress(3,_total);
                fetchFeeds();
            }
        }
    }
}

void NetvibesFetcher::cleanNewFeeds()
{
    Settings *s = Settings::instance();
    QList<DatabaseManager::StreamModuleTab> storedStreamList = s->db->readStreamModuleTabListWithoutDate();
    //QList<DatabaseManager::StreamModuleTab> storedStreamList = s->db->readStreamModuleTabList();
    QList<DatabaseManager::StreamModuleTab>::iterator i = _streamList.begin();

    /*QList<DatabaseManager::StreamModuleTab>::iterator si = storedStreamList.begin();
    while (si != storedStreamList.end()) {
        qDebug() << "stored| streamId > moduleId > tabId" << (*si).streamId << (*si).moduleId << (*si).tabId;
        ++si;
    }
    while (i != _streamList.end()) {
        qDebug() << "fetched| streamId > moduleId > tabId" << (*i).streamId << (*i).moduleId << (*i).tabId;
        ++i;
    }*/

    i = _streamList.begin();
    while (i != _streamList.end()) {

        QList<DatabaseManager::StreamModuleTab>::iterator ci = storedStreamList.begin();
        bool newStream = true;
        while (ci != storedStreamList.end()) {
            if ((*i).streamId==(*ci).streamId && (*i).tabId==(*ci).tabId) {
                //qDebug() << "Old stream" << (*i).streamId << "in tab" << (*i).tabId;
                i = _streamList.erase(i);
                storedStreamList.erase(ci);
                newStream = false;
                break;
            }
            if ((*i).streamId==(*ci).streamId) {
                qDebug() << "Old stream" << (*i).streamId << "in new tab" << (*i).tabId;
            }
            ++ci;
        }
        if (newStream) {
            qDebug() << "New stream" << (*i).streamId << "in tab" << (*i).tabId;
            ++i;
        }
    }
}

void NetvibesFetcher::cleanRemovedFeeds()
{
    Settings *s = Settings::instance();
    QList<DatabaseManager::StreamModuleTab> storedStreamList = s->db->readStreamModuleTabListWithoutDate();
    QList<DatabaseManager::StreamModuleTab>::iterator i = storedStreamList.begin();

    /*QList<DatabaseManager::StreamModuleTab>::iterator si = storedStreamList.begin();
    while (si != storedStreamList.end()) {
        qDebug() << "stored| streamId > moduleId > tabId" << (*si).streamId << (*si).moduleId << (*si).tabId;
        ++si;
    }*/

    while (i != storedStreamList.end()) {

        bool removedStream = true;
        QList<DatabaseManager::StreamModuleTab>::iterator ci = _streamList.begin();
        while (ci != _streamList.end()) {
            if ((*i).streamId==(*ci).streamId && (*i).tabId==(*ci).tabId) {
                //qDebug() << "Existing stream" << (*i).streamId << "in tab" << (*i).tabId;
                removedStream = false;
                break;
            }
            ++ci;
        }
        if (removedStream) {
            qDebug() << "Removing stream" << (*i).streamId << "in tab" << (*i).tabId;
            s->db->removeStreamsByStream((*i).streamId);

            // Removing stream from streamUpdateList
            QList<DatabaseManager::StreamModuleTab>::iterator sui = _streamUpdateList.begin();
            while (sui != _streamUpdateList.end()) {
                if ((*sui).streamId==(*i).streamId && (*sui).tabId==(*i).tabId) {
                    //qDebug() << "Removing stream form _streamUpdateList" << (*sui).streamId;
                    _streamUpdateList.erase(sui);
                    break;
                }
                ++sui;
            }
        }
        ++i;
    }
}

void NetvibesFetcher::finishedFeeds()
{
    //qDebug() << this->_data;
    startJob(StoreFeeds);
}

void NetvibesFetcher::finishedFeeds2()
{
    Settings *s = Settings::instance();
    int feedsAtOnce = s->getFeedsAtOnce();
    int feedsUpdateAtOnce = s->getFeedsUpdateAtOnce();

    emit progress(_total-((_streamList.count()/feedsAtOnce)+(_streamUpdateList.count()/feedsUpdateAtOnce)),_total);

    if (_streamList.isEmpty()) {

        if(_busyType == Updating) {
            _streamUpdateList = s->db->readStreamModuleTabList();
            fetchFeedsUpdate();
        }

        if(_busyType == Initiating) {
            publishedBeforeDate = 0;
            fetchFeedsReadlater();
        }

    } else {
        fetchFeeds();
    }
}

void NetvibesFetcher::finishedFeedsReadlater()
{
    //qDebug() << this->_data;

    publishedBeforeDate = 0;
    startJob(StoreFeedsReadlater);
}

void NetvibesFetcher::finishedFeedsReadlater2()
{
    if (publishedBeforeDate!=0)
        fetchFeedsReadlater();
    else
        taskEnd();
}

void NetvibesFetcher::finishedSet()
{
    //qDebug() << this->_data;

    Settings *s = Settings::instance();

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    checkError();

    // deleting action
    DatabaseManager::Action action = actionsList.takeFirst();
    s->db->removeActionsById(action.id1);

    if (actionsList.isEmpty()) {
        s->db->cleanDashboards();
        fetchDashboards();
    } else {
        uploadActions();
    }
}

void NetvibesFetcher::finishedFeedsUpdate()
{
    //qDebug() << this->_data;

    startJob(StoreFeedsUpdate);
}

void NetvibesFetcher::finishedFeedsUpdate2()
{
    Settings *s = Settings::instance();
    int feedsUpdateAtOnce = s->getFeedsUpdateAtOnce();

    emit progress(_total-qCeil(_streamUpdateList.count()/feedsUpdateAtOnce),_total);

    if (_streamUpdateList.isEmpty()) {
        // Fetching Saved items
        publishedBeforeDate = 0;
        fetchFeedsReadlater();
        //taskEnd();
    } else {
        fetchFeedsUpdate();
    }
}

void NetvibesFetcher::networkError(QNetworkReply::NetworkError e)
{
    _currentReply->disconnect(this);
    _currentReply->deleteLater();
    _currentReply = 0;

    if (e == QNetworkReply::OperationCanceledError) {
        emit canceled();
    } else {
        emit error(500);
        qWarning() << "Network error!, error code: " << e;
    }

    _data.clear();

    setBusy(false);
}

void NetvibesFetcher::taskEnd()
{
    emit progress(_total, _total);

    _currentReply->disconnect(this);
    _currentReply->deleteLater();
    _currentReply = 0;

    Settings *s = Settings::instance();
    s->setLastUpdateDate(QDateTime::currentDateTimeUtc().toTime_t());

    _data.clear();

    emit ready();
    setBusy(false);
}

void NetvibesFetcher::uploadActions()
{
    if (!actionsList.isEmpty()) {
        emit uploading();
        set();
    }
}

void NetvibesFetcher::cancel()
{
    disconnect(&ncm, SIGNAL(onlineStateChanged(bool)), this, SLOT(delayedUpdate(bool)));
    if (_busyType==UpdatingWaiting||_busyType==InitiatingWaiting||_busyType==CheckingCredentialsWaiting) {
        setBusy(false);
    } else {
        if (_currentReply)
            _currentReply->close();
        else
            setBusy(false);
    }
}

QString NetvibesFetcher::hash(const QString &url)
{
    QByteArray data; data.append(url);
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
}

void NetvibesFetcher::run() {

    switch (currentJob) {
    case StoreFeeds:
    case StoreFeedsUpdate:
        storeFeeds();
        break;
    case StoreDashboards:
        storeDashboards();
        break;
    case StoreTabs:
        storeTabs();
        break;
    case StoreFeedsReadlater:
        if (storeFeeds()<limitFeedsReadlater) {
            publishedBeforeDate = 0;
            //publishedBeforeItemId = "";
            //publishedBeforeStreamId = "";
        }
        break;
    default:
        break;
    }
}

void NetvibesFetcher::startJob(Job job)
{
    if (isRunning()) {
        qWarning() << "Job is running";
        return;
    }

    disconnect(this, SIGNAL(finished()), 0, 0);
    currentJob = job;

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    switch (job) {
    case StoreDashboards:
        connect(this, SIGNAL(finished()), this, SLOT(finishedDashboards2()));
        break;
    case StoreTabs:
        connect(this, SIGNAL(finished()), this, SLOT(finishedTabs2()));
        break;
    case StoreFeeds:
        connect(this, SIGNAL(finished()), this, SLOT(finishedFeeds2()));
        break;
    case StoreFeedsInfo:
        connect(this, SIGNAL(finished()), this, SLOT(finishedFeedsInfo2()));
        break;
    case StoreFeedsUpdate:
        connect(this, SIGNAL(finished()), this, SLOT(finishedFeedsUpdate2()));
        break;
    case StoreFeedsReadlater:
        connect(this, SIGNAL(finished()), this, SLOT(finishedFeedsReadlater2()));
        break;
    default:
        break;
    }

    //start(QThread::IdlePriority);
    start(QThread::LowPriority);
}

bool NetvibesFetcher::checkError()
{
    if(_jsonObj["error"].isObject()) {
        qWarning() << "Error in Netvibes response!";
        qWarning() << "Code:" << (int) _jsonObj["error"].toObject()["code"].toDouble();
        qWarning() << "Message:" << _jsonObj["error"].toObject()["message"].toString();
        qWarning() << "JSON:" << _jsonObj;
        return true;
    }
    return false;
}
