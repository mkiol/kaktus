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

#include "netvibesfetcher.h"

NetvibesFetcher::NetvibesFetcher(DatabaseManager* db, QObject *parent) :
    QObject(parent)
{
    _db = db;
    _currentReply = 0;
    _busy = false;
    _busyType = false;

    connect(&_manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
}

void NetvibesFetcher::init()
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        emit error(200);
        return;
    }

    if (!ncm.isOnline()) {
        qWarning() << "Network is Offline!";
        emit networkNotAccessible();
        return;
    }

    _busy = true; _busyType = false;
    emit busy();
    emit initiating();

    _feedList.clear(); _feedTabList.clear();
    signIn();
}

bool NetvibesFetcher::isBusy()
{
    return _busy;
}

void NetvibesFetcher::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (this->_busy) {
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

void NetvibesFetcher::update()
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        emit error(200);
        return;
    }

    if (!ncm.isOnline()) {
        qWarning() << "Network is Offline!";
        emit networkNotAccessible();
        return;
    }

    int feedCount =_db->readFeedsCount();
    int entriesCount =_db->readFeedsCount();

    emit busy();

    _busy = true;
    if (feedCount == 0 || entriesCount == 0) {
        _busyType = false;
        emit initiating();
    } else {
        _busyType = true;
        emit updating();
    }

    _feedList.clear(); _feedTabList.clear(); actionsList.clear();
    signIn();
}

void NetvibesFetcher::checkCredentials()
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        emit error(200);
        return;
    }

    emit busy();
    emit checkingCredentials();
    _busy = true;

    signIn(true);
}

void NetvibesFetcher::updateFeeds()
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        emit error(200);
        return;
    }

    _busy = true; _busyType = true;
    emit busy();

    fetchFeedsUpdate();
}

void NetvibesFetcher::updateTab(const QString &tabId)
{
    if (_busy) {
        qWarning() << "Fetcher is busy!";
        emit error(200);
        return;
    }

    _busy = true; _busyType = true;
    emit busy();

    fetchFeedsInfo(tabId);
}

void NetvibesFetcher::signIn(bool onlyCheck)
{
    _data = QByteArray();

    Settings *s = Settings::instance();
    QString password = s->getNetvibesPassword();
    QString username = s->getNetvibesUsername();

    if (password == "" || username == "") {
        qWarning() << "Netvibes username & password do not match!";
        if (onlyCheck)
            emit errorCheckingCredentials(400);
        else
            emit error(400);
        _busy = false;
        return;
    }

    QUrl url("http://www.netvibes.com/ajax/user/signIn.php");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    QString body = "email="+QUrl::toPercentEncoding(username)+"&password="+QUrl::toPercentEncoding(password)+"&session_only=1";
    _currentReply = _manager.post(request,body.toUtf8());

    if (onlyCheck)
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

void NetvibesFetcher::set(const QString &entryId, DatabaseManager::ActionsTypes type)
{
    _data = QByteArray();

    QUrl url;

    switch (type) {
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
    }

    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    QString feedId = _db->readFeedId(entryId);
    QString content = "feeds="+feedId+"&items="+entryId+"&format=json";

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

    Settings *s = Settings::instance();
    int limitFeeds= s->getNetvibesFeedLimit();

    QString feeds, limit; int ii = 0;
    QStringList::iterator i = _feedList.begin();
    while (i != _feedList.end()) {
        if (ii > NetvibesFetcher::feedsAtOnce) {
            break;
        }

        if (ii != 0) {
            feeds += ",";
            limit += "&";
        }

        feeds += *i;
        limit += "limit[" + QString::number(ii) + "]=" + QString::number(limitFeeds);

        i = _feedList.erase(i);
        ++ii;
    }

    QString content = "feeds=" + QUrl::toPercentEncoding(feeds) + "&" + limit + "&merged=0&format=json";

    _currentReply = _manager.post(request, content.toUtf8());
    connect(_currentReply, SIGNAL(finished()), this, SLOT(finishedFeeds()));
    connect(_currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void NetvibesFetcher::fetchFeedsInfo(const QString &tabId)
{
    _data = QByteArray();

    QUrl url("http://www.netvibes.com/api/feeds/info");
    QNetworkRequest request(url);

    if (_currentReply) {
        _currentReply->disconnect();
        _currentReply->deleteLater();
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setRawHeader("Cookie", _cookie);

    Settings *s = Settings::instance();
    int limitFeeds= s->getNetvibesFeedLimit();

    QString feeds, limit; int ii = 0;
    QStringList::iterator i = _feedList.begin();
    while (i != _feedList.end()) {
        if (ii > NetvibesFetcher::feedsAtOnce) {
            break;
        }

        if (ii != 0) {
            feeds += ",";
            limit += "&";
        }

        feeds += *i;
        limit += "limit[" + QString::number(ii) + "]=" + QString::number(limitFeeds);

        i = _feedList.erase(i);
        ++ii;
    }

    QString content = "feeds=" + QUrl::toPercentEncoding(feeds) + "&" + limit + "&merged=0&format=json";

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

    Settings *s = Settings::instance();
    int feedsUpdateAtOnce = s->getNetvibesFeedUpdateAtOnce();

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
    QJsonDocument doc = QJsonDocument::fromJson(this->_data);
    if (!doc.isObject()) {
        qWarning() << "Json doc is empty!";
        return false;
    }
    _jsonObj = doc.object();
   return true;
}

void NetvibesFetcher::storeTabs(const QString &dashboardId)
{
    if (_jsonObj["userData"].toObject()["tabs"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["userData"].toObject()["tabs"].toArray().constBegin();
        while (i != _jsonObj["userData"].toObject()["tabs"].toArray().constEnd()) {
            QJsonObject obj = (*i).toObject();

            DatabaseManager::Tab t;
            t.id = obj["id"].toString();
            t.icon = obj["icon"].toString();
            t.title = obj["title"].toString();
            _db->writeTab(dashboardId, t);
            _tabList.append(t.id);

            // Downloading icon file
            if (t.icon!="") {
                Settings *s = Settings::instance();
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

    if (_jsonObj["userData"].toObject()["modules"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["userData"].toObject()["modules"].toArray().constBegin();
        while (i != _jsonObj["userData"].toObject()["modules"].toArray().constEnd()) {
            QJsonObject obj = (*i).toObject();

            if (obj["name"].toString() == "RssReader") {
                _feedTabList.insert(
                            obj["data"].toObject()["streamIds"].toString(),
                        obj["tab"].toString()
                        );
                _feedList.append(obj["data"].toObject()["streamIds"].toString());
            }

            ++i;
        }
    }  else {
        qWarning() << "No modules element found!";
    }
}

void NetvibesFetcher::storeFeeds()
{
    if (_jsonObj["feeds"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["feeds"].toArray().constBegin();
        while (i != _jsonObj["feeds"].toArray().constEnd()) {
            QJsonObject obj = (*i).toObject();

            DatabaseManager::Feed f;
            f.id = obj["id"].toString();
            f.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
            //f.title = obj["title"].toString();
            f.link = obj["link"].toString();
            f.url = obj["url"].toString();
            f.content = obj["content"].toString();
            f.streamId = f.id;
            f.unread = obj["flags"].toObject()["unread"].toDouble();
            f.readlater = obj["flags"].toObject()["readlater"].toDouble();
            f.lastUpdate = QDateTime::currentDateTime().toTime_t();

            QMap<QString,QString>::iterator it = _feedTabList.find(f.id);
            if (it!=_feedTabList.end()) {

                // Downloading fav icon file
                if (f.link!="") {
                    Settings *s = Settings::instance();
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

                _db->writeFeed(it.value(), f);

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
    if (_jsonObj["items"].isArray()) {
        QJsonArray::const_iterator i = _jsonObj["items"].toArray().constBegin();
        while (i != _jsonObj["items"].toArray().constEnd()) {
            QJsonArray::const_iterator ii = (*i).toArray().constBegin();
            while (ii != (*i).toArray().constEnd()) {
                QJsonObject obj = (*ii).toObject();

                DatabaseManager::Entry e;
                e.id = obj["id"].toString();
                //e.title = obj["title"].toString().remove(QRegExp("<[^>]*>"));
                e.title = obj["title"].toString();
                e.author = obj["author"].toString();
                e.link = obj["link"].toString();
                e.content = obj["content"].toString();
                e.read = (int) obj["flags"].toObject()["read"].toDouble();
                e.readlater = (int) obj["flags"].toObject()["readlater"].toDouble();
                e.date = (int) obj["date"].toDouble();
                _db->writeEntry(obj["feed_id"].toString(), e);

                ++ii;
            }
            ++i;
        }
    }  else {
        qWarning() << "No items element found!";
    }

}

void NetvibesFetcher::storeDashboards()
{
    if (_jsonObj["dashboards"].isObject()) {

        // Set default dashboard if not set
        Settings *s = Settings::instance();
        QString defaultDashboardId = s->getNetvibesDefaultDashboard();
        int lowestDashboardId = 99999999;
        bool defaultDashboardIdExists = false;

        QJsonObject::const_iterator i = _jsonObj["dashboards"].toObject().constBegin();
        while (i != _jsonObj["dashboards"].toObject().constEnd()) {
            QJsonObject obj = i.value().toObject();

            if (obj["active"].toString()=="1") {

                DatabaseManager::Dashboard d;
                d.id = obj["pageId"].toString();
                d.name = obj["name"].toString();
                d.title = obj["title"].toString();
                d.description = obj["descrition"].toString();
                _db->writeDashboard(d);
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
            s->setNetvibesDefaultDashboard(QString::number(lowestDashboardId));
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
            _busy = false;
        } else {
            s->setSignedIn(false);
            QString message = _jsonObj["message"].toString();
            if (message == "nomatch")
                emit errorCheckingCredentials(402);
            else
                emit errorCheckingCredentials(401);
            _busy = false;
            qWarning() << "SignIn check error, messsage: " << message;
        }
    } else {
        s->setSignedIn(false);
        qWarning() << "SignIn check error!";
        emit errorCheckingCredentials(501);
        _busy = false;
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
            actionsList =_db->readActions();
            if (actionsList.isEmpty()) {
                //qDebug() << "No actions to upload!";
                _db->cleanDashboards();
                fetchDashboards();
            } else {
                //qDebug() << actionsList.count() << " actions to upload!";
                uploadActions();
            }

        } else {
            s->setSignedIn(false);

            QString message = _jsonObj["message"].toString();
            if (message == "nomatch")
                emit error(402);
            else
                emit error(401);
            _busy = false;
            qWarning() << "SignIn error, messsage: " << message;
        }
    } else {
        s->setSignedIn(false);

        qWarning() << "SignIn error!";
        emit error(501);
        _busy = false;
    }
}

void NetvibesFetcher::finishedDashboards()
{
    //qDebug() << this->_data;

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        _busy = false;
        return;
    }

    storeDashboards();

    if(!_dashboardList.isEmpty()) {
        _db->cleanTabs();

        // Create Cache structure for Tab icons
        if(!_busyType) {
            _db->cleanCache();
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

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        _busy = false;
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

            if(_busyType) {

                //update
                cleanRemovedFeeds();
                cleanNewFeeds();

                Settings *s = Settings::instance();
                int feedsUpdateAtOnce = s->getNetvibesFeedUpdateAtOnce();

                _feedUpdateList = _db->readFeedsFirstUpdate();

                if (_feedList.isEmpty()) {
                    //qDebug() << "No new Feeds!";
                    fetchFeedsUpdate();

                    _total = (_feedUpdateList.count()/feedsUpdateAtOnce)+3;
                    emit progress(3,_total);

                } else {
                    fetchFeeds();

                    _total = (_feedUpdateList.count()/feedsUpdateAtOnce)+(_feedList.length()/feedsAtOnce)+3;
                    emit progress(3,_total);
                }

            } else {

                // init
                _db->cleanFeeds();
                _db->cleanEntries();
                //_db->cleanCache();
                fetchFeeds();

                _total = (_feedList.length()/feedsAtOnce)+3;
                emit progress(3,_total);
            }
        }
    }
}

void NetvibesFetcher::cleanNewFeeds()
{
    QMap<QString,int> storedFeedList = _db->readFeedsLastUpdate();
    QStringList::iterator i = _feedList.begin();
    while (i != _feedList.end()) {
        if (storedFeedList.find(*i) == storedFeedList.end()) {
            //qDebug() << "New feed " << *i;
            ++i;
        } else {
            i = _feedList.erase(i);
        }
    }
}

void NetvibesFetcher::cleanRemovedFeeds()
{
    QMap<QString,int> storedFeedList = _db->readFeedsLastUpdate();
    QMap<QString,int>::iterator i = storedFeedList.begin();
    while (i != storedFeedList.end()) {
        if (!_feedList.contains(i.key())) {
            _db->removeFeed(i.key());
        }
        ++i;
    }
}

void NetvibesFetcher::finishedFeeds()
{
    //qDebug() << this->_data;

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        _busy = false;
        return;
    }

    storeFeeds();
    storeEntries();

    Settings *s = Settings::instance();
    int feedsUpdateAtOnce = s->getNetvibesFeedUpdateAtOnce();
    emit progress(_total-((_feedList.length()/feedsAtOnce)+(_feedUpdateList.count()/feedsUpdateAtOnce)),_total);

    if (_feedList.isEmpty()) {

        if(_busyType) {
            _feedUpdateList = _db->readFeedsFirstUpdate();
            fetchFeedsUpdate();
        } else {
            taskEnd();
        }

    } else {
        fetchFeeds();
    }
}

void NetvibesFetcher::finishedFeedsInfo()
{
    //qDebug() << this->_data;

    if(!parse()) {
        qWarning() << "Error parsing Json!";
        emit error(600);
        _busy = false;
        return;
    }

    emit ready();
    _busy = false;
}

void NetvibesFetcher::finishedSet()
{
    //qDebug() << this->_data;

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
        _busy = false;
        return;
    }

    // deleting action
    //qDebug() << "Deleting action...";
    DatabaseManager::Action action = actionsList.takeFirst();
    _db->removeAction(action.entryId);

    if (actionsList.isEmpty()) {
        _db->cleanDashboards();
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
        _busy = false;
        return;
    }

    storeFeeds();
    storeEntries();

    Settings *s = Settings::instance();
    int feedsUpdateAtOnce = s->getNetvibesFeedUpdateAtOnce();
    emit progress(_total-(_feedUpdateList.count()/feedsUpdateAtOnce),_total);

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

    _busy = false;
}

void NetvibesFetcher::taskEnd()
{
    emit progress(_total, _total);

    _currentReply->disconnect(this);
    _currentReply->deleteLater();
    _currentReply = 0;

    Settings *s = Settings::instance();
    s->setNetvibesLastUpdateDate(QDateTime::currentDateTime().toTime_t());

    if(_busyType) {
        //qDebug() << "Update ends!";
        emit ready();
        _busy = false;
    } else {
        //qDebug() << "Init ends!";
        emit ready();
        _busy = false;
    }
}

void NetvibesFetcher::uploadActions()
{
    if (!actionsList.isEmpty()) {
        emit uploading();
        DatabaseManager::Action action = actionsList.first();
        set(action.entryId,action.type);
    }
}

void NetvibesFetcher::cancel()
{
    if (_currentReply)
        _currentReply->close();
}


