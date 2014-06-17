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
    QObject(parent)
{
    _currentReply = NULL;
    _busy = false;
    _busyType = Unknown;

    connect(&_manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
}

bool NetvibesFetcher::init()
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        return false;
    }

#ifdef ONLINE_CHECK
    if (!ncm.isOnline()) {
        qWarning() << "Network is Offline!";
        emit networkNotAccessible();
        return false;
    }
#endif

    setBusy(true, Initiating);

    _feedList.clear();
    _feedTabList.clear();

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

#ifdef ONLINE_CHECK
    if (!ncm.isOnline()) {
        qWarning() << "Network is Offline!";
        emit networkNotAccessible();
        return false;
    }
#endif

    Settings *s = Settings::instance();
    int feedCount =s->db->readFeedsCount();
    int entriesCount =s->db->readFeedsCount();

    if (feedCount == 0 || entriesCount == 0) {
        setBusy(true, Initiating);
    } else {
        setBusy(true, Updating);
    }

    _feedList.clear();
    _feedTabList.clear();
    actionsList.clear();

    signIn();
    //emit progress(0,100);
    return true;
}

bool NetvibesFetcher::checkCredentials()
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        return false;
    }

    setBusy(true, CheckingCredentials);

    signIn();

    return true;
}

void NetvibesFetcher::signIn()
{
    _data = QByteArray();

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

    //QUrl url("http://www.netvibes.com/ajax/user/signIn.php");
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
    _data = QByteArray();

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

    _data = QByteArray();

    QUrl url;

    switch (action.type) {
    case DatabaseManager::SetRead:
        url.setUrl("http://www.netvibes.com/api/feeds/read/add");
        break;
    case DatabaseManager::SetReadlater:
        url.setUrl("http://www.netvibes.com/api/feeds/readlater/add");
        break;
    case DatabaseManager::UnSetRead:
        url.setUrl("http://www.netvibes.com/api/feeds/read/remove");
        break;
    case DatabaseManager::UnSetReadlater:
        url.setUrl("http://www.netvibes.com/api/feeds/readlater/remove");
        break;
    case DatabaseManager::UnSetFeedReadAll:
        url.setUrl("http://www.netvibes.com/api/feeds/read/remove");
        break;
    case DatabaseManager::SetFeedReadAll:
        url.setUrl("http://www.netvibes.com/api/feeds/read/add");
        break;
    case DatabaseManager::UnSetTabReadAll:
        url.setUrl("http://www.netvibes.com/api/feeds/read/remove");
        break;
    case DatabaseManager::SetTabReadAll:
        url.setUrl("http://www.netvibes.com/api/feeds/read/add");
        break;
    }

    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    QString content;

    if (action.type == DatabaseManager::SetTabReadAll ||
        action.type == DatabaseManager::UnSetTabReadAll) {

        Settings *s = Settings::instance();
        QStringList list = s->db->readFeedsIdsByTab(action.feedId);
        QStringList::iterator i = list.begin();
        QString feeds;
        while (i != list.end()) {
            if (i != list.begin())
                feeds += ",";
            feeds += *i+":"+QString::number(action.olderDate);
            ++i;
        }
        content = QString("feeds=%1&format=json").arg(feeds);
    }

    if (action.type == DatabaseManager::SetFeedReadAll ||
        action.type == DatabaseManager::UnSetFeedReadAll) {
        content = QString("feeds=%1:%2&format=json").arg(action.feedId).arg(action.olderDate);
    }

    if (action.type == DatabaseManager::SetRead ||
        action.type == DatabaseManager::UnSetRead ||
        action.type == DatabaseManager::SetReadlater ||
        action.type == DatabaseManager::UnSetReadlater) {
        content = QString("feeds=%1&items=%2&format=json").arg(action.feedId).arg(action.entryId);
    }

    //qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedSet()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));

    //Logging
    /*Settings *s = Settings::instance();
    QFile file(s->getSettingsDir() + "/log_request.txt");
    if (!file.open(QIODevice::Append)) {
        qWarning() << "Could not open" << file.fileName() << "for append: " << file.errorString();
    } else {
        file.write(("["+QDateTime::currentDateTime().toString()+"]\n").toUtf8());
        file.write(content.toUtf8()+"\n");
        file.close();
    }*/
}

void NetvibesFetcher::fetchTabs(const QString &dashboardID)
{
    _data = QByteArray();

    QUrl url("http://www.netvibes.com/api/my/dashboards/data");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);
    _currentReply = _manager.post(request,"format=json&pageId="+dashboardID.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedTabs()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchFeeds()
{
    _data = QByteArray();

    QUrl url("http://www.netvibes.com/api/feeds");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    QString feeds, limit; int ii = 0;
    QMap<QString,QString>::iterator i = _feedList.begin();
    while (i != _feedList.end()) {
        if (ii > feedsAtOnce) {
            break;
        }

        if (ii != 0) {
            feeds += ",";
            limit += "&";
        }

        feeds += i.key();
        limit += "limit[" + QString::number(ii) + "]=" + QString::number(limitFeeds);

        i = _feedList.erase(i);
        ++ii;
    }

    QString content = "feeds=" + QUrl::toPercentEncoding(feeds) + "&" + limit + "&merged=0&format=json";
    //qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedFeeds()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

/*void NetvibesFetcher::fetchFeeds2()
{
    _data = QByteArray();

    QUrl url("http://www.netvibes.com/api/feeds");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    QString feeds; int ii = 0;
    QStringList::iterator i = _feedList.begin();
    while (i != _feedList.end()) {
        if (ii > feedsAtOnce) {
            break;
        }
        if (ii != 0)
            feeds += ",";
        feeds += *i;
        i = _feedList.erase(i);
        ++ii;
    }

    QString content = "offset=0&limit=" +QString::number(feedsAtOnce*limitFeeds)+ "&feeds=" + QUrl::toPercentEncoding(feeds) + "&format=json";
    qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedFeeds()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}*/

void NetvibesFetcher::fetchFeedsReadlater()
{
    _data = QByteArray();

    QUrl url("http://www.netvibes.com/api/feeds/readlater");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    Settings *s = Settings::instance();
    QStringList list = s->db->readAllFeedIds();
    QStringList::iterator i = list.begin();
    QString feeds;

    while (i != list.end()) {
        if (i != list.begin())
            feeds += ",";
        feeds += *i;
        ++i;
    }

    QString content = QString("offset=%1&limit=%2&feeds=%3&format=json")
            .arg(offset*limitFeedsReadlater)
            .arg(limitFeedsReadlater)
            .arg(QUrl::toPercentEncoding(feeds).data());

    //qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedFeedsReadlater()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchFeedsInfo(const QString &tabId)
{
    Q_UNUSED(tabId)

    _data = QByteArray();

    QUrl url("http://www.netvibes.com/api/feeds/info");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    QString feeds, limit; int ii = 0;
    QMap<QString,QString>::iterator i = _feedList.begin();
    while (i != _feedList.end()) {
        if (ii > feedsAtOnce) {
            break;
        }

        if (ii != 0) {
            feeds += ",";
            limit += "&";
        }

        feeds += i.key();
        limit += "limit[" + QString::number(ii) + "]=" + QString::number(limitFeeds);

        i = _feedList.erase(i);
        ++ii;
    }

    QString content = "feeds=" + QUrl::toPercentEncoding(feeds) + "&" + limit + "&merged=0&format=json";

    //qDebug() << "content=" << content;

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedFeedsInfo()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchFeedsUpdate()
{
    _data = QByteArray();

    QUrl url("http://www.netvibes.com/api/feeds/update");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    QString feeds;  int ii = 0;
    QMap<QString,int>::iterator i = _feedUpdateList.begin();
    //qDebug() << "feedUpdateList.count=" << _feedUpdateList.count();
    while (i != _feedUpdateList.end()) {
        if (ii >= feedsUpdateAtOnce) {
            break;
        }
        if (ii != 0) {
            feeds += ",";
        }
        feeds += i.key() + ":" + QString::number(i.value());

        i = _feedUpdateList.erase(i);
        ++ii;
    }

    QString content = "feeds=" + QUrl::toPercentEncoding(feeds) + "&merged=0&format=json";

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

void NetvibesFetcher::storeTabs(const QString &dashboardId)
{
    Settings *s = Settings::instance();

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
            t.icon = obj["icon"].toString();
            t.title = obj["title"].toString();

            s->db->writeTab(dashboardId, t);
            _tabList.append(t.id);

            // Downloading icon file
            if (t.icon!="") {
                DatabaseManager::CacheItem item;
                item.origUrl = t.icon;
                item.finalUrl = t.icon;
                s->dm->addDownload(item);
                //qDebug() << "icon:" << t.icon;
            }

            ++i;
        }
    }  else {
        qWarning() << "No tabs element found!";
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
            if (obj["name"].toString() == "RssReader") {
                //qDebug() << obj["tab"].toString() << obj["data"].toObject()["streamIds"].toString();
                QString streamId = obj["data"].toObject()["streamIds"].toString();
                QString tabId = obj["tab"].toString();
                _feedTabList.insert(streamId,tabId);
                _feedList.insert(streamId,tabId);
            }
#else
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

void NetvibesFetcher::storeFeeds()
{
    Settings *s = Settings::instance();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (_jsonObj["feeds"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["feeds"].toArray().constBegin();
        QJsonArray::const_iterator end = _jsonObj["feeds"].toArray().constEnd();
#else
    if (_jsonObj["feeds"].type()==QVariant::List) {
        QVariantList::const_iterator i = _jsonObj["feeds"].toList().constBegin();
        QVariantList::const_iterator end = _jsonObj["feeds"].toList().constEnd();
#endif
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = (*i).toObject();
            int unread = obj["flags"].toObject()["unread"].toDouble();
            int read = obj["flags"].toObject()["read"].toDouble();
            int readlater = obj["flags"].toObject()["readlater"].toDouble();
#else
            QVariantMap obj = (*i).toMap();
            int unread = obj["flags"].toMap()["unread"].toDouble();
            int read = obj["flags"].toMap()["read"].toDouble();
            int readlater = obj["flags"].toMap()["readlater"].toDouble();
#endif
            DatabaseManager::Feed f;
            f.id = obj["id"].toString();
            f.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
            f.link = obj["link"].toString();
            f.url = obj["url"].toString();
            f.content = obj["content"].toString();
            f.streamId = f.id;
            f.unread = unread;
            f.read = read;
            f.readlater = readlater;
            //qDebug() << f.title;
            f.lastUpdate = QDateTime::currentDateTimeUtc().toTime_t();

            QMap<QString,QString>::iterator it = _feedTabList.find(f.id);
            if (it!=_feedTabList.end()) {
                // Downloading fav icon file
                if (f.link!="") {
                    QUrl iconUrl(f.link);
                    f.icon = QString("http://avatars.netvibes.com/favicon/%1://%2")
                            .arg(iconUrl.scheme())
                            .arg(iconUrl.host());
                    DatabaseManager::CacheItem item;
                    item.origUrl = f.icon;
                    item.finalUrl = f.icon;
                    s->dm->addDownload(item);
                    //qDebug() << "favicon:" << f.icon;
                }

                s->db->writeFeed(it.value(), f);

            } else {
                qWarning() << "No matching feed!";
            }

            ++i;
        }
    }  else {
        qWarning() << "No feeds element found!";
    }
}

void NetvibesFetcher::storeEntries()
{
    Settings *s = Settings::instance();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (_jsonObj["items"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["items"].toArray().constBegin();
        QJsonArray::const_iterator end = _jsonObj["items"].toArray().constEnd();
#else
    if (_jsonObj["items"].type()==QVariant::List) {
        QVariantList::const_iterator i = _jsonObj["items"].toList().constBegin();
        QVariantList::const_iterator end = _jsonObj["items"].toList().constEnd();
#endif
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonArray::const_iterator ii = (*i).toArray().constBegin();
            while (ii != (*i).toArray().constEnd()) {
                QJsonObject obj = (*ii).toObject();
                int read = (int) obj["flags"].toObject()["read"].toDouble();
                int readlater = (int) obj["flags"].toObject()["readlater"].toDouble();
                QString image = "";
                if (obj["enclosures"].isArray()) {
                    if (!obj["enclosures"].toArray().empty()) {
                        //qDebug() << obj["enclosures"].toArray()[0].toObject()["type"].toString() << obj["enclosures"].toArray()[0].toObject()["url"].toString();
                        image = obj["enclosures"].toArray()[0].toObject()["url"].toString();
                    }
                }
#else
            QVariantList::const_iterator ii = (*i).toList().constBegin();
            while (ii != (*i).toList().constEnd()) {
                QVariantMap obj = (*ii).toMap();
                int read = (int) obj["flags"].toMap()["read"].toDouble();
                int readlater = (int) obj["flags"].toMap()["readlater"].toDouble();
#endif
                DatabaseManager::Entry e;
                e.id = obj["id"].toString();
                //e.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
                e.title = obj["title"].toString();
                e.author = obj["author"].toString();
                e.link = obj["link"].toString();
                e.image = image;
                e.content = obj["content"].toString();
                e.read = read;
                e.readlater = readlater;
                e.date = (int) obj["date"].toDouble();
                s->db->writeEntry(obj["feed_id"].toString(), e);

                // Downloading image file
                if (image!="" && s->getAutoDownloadOnUpdate()) {
                    if (!s->db->isCacheItemExistsByFinalUrl(hash(image))) {
                        DatabaseManager::CacheItem item;
                        item.origUrl = image;
                        item.finalUrl = image;
                        s->dm->addDownload(item);
                    }
                }

                ++ii;
            }
            ++i;
        }
    }  else {
        qWarning() << "No items element found!";
    }
}

bool NetvibesFetcher::storeEntriesMerged()
{
    Settings *s = Settings::instance();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (_jsonObj["items"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["items"].toArray().constBegin();
        QJsonArray::const_iterator end = _jsonObj["items"].toArray().constEnd();
#else
    if (_jsonObj["items"].type()==QVariant::List) {
        QVariantList::const_iterator i = _jsonObj["items"].toList().constBegin();
        QVariantList::const_iterator end = _jsonObj["items"].toList().constEnd();
#endif
        while (i != end) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            QJsonObject obj = (*i).toObject();
            int read = (int) obj["flags"].toObject()["read"].toDouble();
            int readlater = (int) obj["flags"].toObject()["readlater"].toDouble();
            QString image = "";
            if (obj["enclosures"].isArray()) {
                if (!obj["enclosures"].toArray().empty()) {
                    //qDebug() << obj["enclosures"].toArray()[0].toObject()["type"].toString() << obj["enclosures"].toArray()[0].toObject()["url"].toString();
                    image = obj["enclosures"].toArray()[0].toObject()["url"].toString();
                }
            }
#else
            QVariantMap obj = (*i).toMap();
            int read = (int) obj["flags"].toMap()["read"].toDouble();
            int readlater = (int) obj["flags"].toMap()["readlater"].toDouble();
#endif

            DatabaseManager::Entry e;
            e.id = obj["id"].toString();
            //e.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
            e.title = obj["title"].toString();
            e.author = obj["author"].toString();
            e.link = obj["link"].toString();
            e.image = image;
            e.content = obj["content"].toString();
            e.read = read;
            e.readlater = readlater;
            e.date = (int) obj["date"].toDouble();
            s->db->writeEntry(obj["feed_id"].toString(), e);

            // Downloading image file
            if (image!="") {
                DatabaseManager::CacheItem item;
                item.origUrl = image;
                item.finalUrl = image;
                s->dm->addDownload(item);
            }

            ++i;
        }

    }  else {
        qWarning() << "No items element found!";
    }

    //qDebug() << "hasMore:" << _jsonObj["hasMore"].toBool();

    // returns true if has more
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (_jsonObj["hasMore"].isBool())
#else
    if (_jsonObj["hasMore"].type()==QVariant::Bool)
#endif
        return _jsonObj["hasMore"].toBool();
    return false;
}

void NetvibesFetcher::storeDashboards()
{
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
                d.description = obj["descrition"].toString();
                s->db->writeDashboard(d);
                _dashboardList.append(d.id);

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

    Settings *s = Settings::instance();

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    storeDashboards();

    if(!_dashboardList.isEmpty()) {
        s->db->cleanTabs();

        // Create Cache structure for Tab icons
        if(_busyType == Initiating) {
            s->db->cleanCache();
        }

        fetchTabs(_dashboardList.first());
    } else {
        qWarning() << "No Dashboards found!";
        taskEnd();
    }

}

void NetvibesFetcher::finishedTabs()
{
    //qDebug() << this->_data;

    Settings *s = Settings::instance();

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    QString dashboardId = _dashboardList.takeFirst();

    storeTabs(dashboardId);

    if(!_dashboardList.isEmpty()) {
        fetchTabs(_dashboardList.first());
    } else {
        if (_tabList.isEmpty()) {
            qWarning() << "No Tabs!";
        }
        if (_feedList.isEmpty()) {
            qWarning() << "No Feeds!";
            taskEnd();
        } else {

            if (_busyType == Updating) {
                cleanRemovedFeeds();
                cleanNewFeeds();
                _feedUpdateList = s->db->readFeedsFirstUpdate();
                //qDebug() << "_feedUpdateList.count:"<<_feedUpdateList.count();

                if (_feedList.isEmpty()) {
                    qDebug() << "No new Feeds!";
                    _total = qCeil(_feedUpdateList.count()/feedsUpdateAtOnce)+3;
                    emit progress(3,_total);
                    fetchFeedsUpdate();
                } else {
                    _total = qCeil(_feedUpdateList.count()/feedsUpdateAtOnce)+qCeil(_feedList.count()/feedsAtOnce)+3;
                    emit progress(3,_total);
                    fetchFeeds();
                }

            }

            if (_busyType == Initiating) {
                s->db->cleanFeeds();
                s->db->cleanEntries();
                //s->db->cleanCache();
                _total = qCeil(_feedList.count()/feedsAtOnce)+3;
                emit progress(3,_total);
                fetchFeeds();
            }
        }
    }
}

void NetvibesFetcher::cleanNewFeeds()
{
    Settings *s = Settings::instance();
    QMap<QString,QString> storedFeedList = s->db->readAllFeedsIdsTabs();
    QMap<QString,QString>::iterator i = _feedList.begin();
    while (i != _feedList.end()) {
        //qDebug() << i.value() << i.key();
        QMap<QString,QString>::iterator ci = storedFeedList.find(i.key());
        if (ci == storedFeedList.end()) {
            qDebug() << "New feed " << i.value() << i.key();
            ++i;
        } else {
            if (ci.value() == i.value()) {
                i = _feedList.erase(i);
            } else {
                qDebug() << "Old feed in new tab found" << ci.value() << i.value() << i.key();
                ++i;
            }
        }
    }
}

void NetvibesFetcher::cleanRemovedFeeds()
{
    Settings *s = Settings::instance();
    QMap<QString,QString> storedFeedList = s->db->readAllFeedsIdsTabs();
    QMap<QString,QString>::iterator i = storedFeedList.begin();
    QMap<QString,QString>::iterator end = _feedList.end();
    while (i != storedFeedList.end()) {
        QMap<QString,QString>::iterator ci = _feedList.find(i.key());
        if (ci == end) {
            qDebug() << "Removing feed" << i.value() << i.key();
            s->db->removeFeed(i.key());
        } else {
            if (ci.value() != i.value()) {
                qDebug() << "Removing existing feed in old tab" << ci.value() << i.value() << i.key();
                s->db->removeFeed(i.key());
            }
        }
        ++i;
    }
}

void NetvibesFetcher::finishedFeeds()
{
    //qDebug() << this->_data;

    Settings *s = Settings::instance();

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    storeFeeds();
    storeEntries();

    emit progress(_total-((_feedList.count()/feedsAtOnce)+(_feedUpdateList.count()/feedsUpdateAtOnce)),_total);

    if (_feedList.isEmpty()) {

        if(_busyType == Updating) {
            _feedUpdateList = s->db->readFeedsFirstUpdate();
            fetchFeedsUpdate();
        }

        if(_busyType == Initiating) {
            offset = 0;
            fetchFeedsReadlater();
        }

    } else {
        fetchFeeds();
    }
}

void NetvibesFetcher::finishedFeedsReadlater()
{
    //qDebug() << this->_data;

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    ++offset;

    if (storeEntriesMerged())
        fetchFeedsReadlater();
    else
        taskEnd();
}

void NetvibesFetcher::finishedFeedsInfo()
{
    //qDebug() << this->_data;

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    emit ready();
    setBusy(false);
}

void NetvibesFetcher::finishedSet()
{
    //qDebug() << this->_data;

    Settings *s = Settings::instance();

    //Logging
    /*Settings *s = Settings::instance();
    QFile file(s->getSettingsDir() + "/log_reply.txt");
    if (!file.open(QIODevice::Append)) {
        qWarning() << "Could not open" << file.fileName() << "for append: " << file.errorString();
    } else {
        file.write(("["+QDateTime::currentDateTime().toString()+"]\n").toUtf8());
        file.write(this->_data+"\n");
        file.close();
    }*/

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    // deleting action
    DatabaseManager::Action action = actionsList.takeFirst();
    s->db->removeAction(action.entryId);

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

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        setBusy(false);
        return;
    }

    storeFeeds();
    storeEntries();

    emit progress(_total-qCeil(_feedUpdateList.count()/feedsUpdateAtOnce),_total);

    if (_feedUpdateList.isEmpty())
        taskEnd();
    else
        fetchFeedsUpdate();
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
    if (_currentReply)
        _currentReply->close();
}

QString NetvibesFetcher::hash(const QString &url)
{
    QByteArray data; data.append(url);
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
}
