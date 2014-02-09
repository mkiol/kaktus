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

#include "utils.h"

Utils::Utils(DatabaseManager* db, QQuickView* view, QObject *parent) :
    QObject(parent)
{
    _db = db;
    _view = view;
    _dashboardModel = NULL;
    //offLine = false;
}

/*bool Utils::isOffline()
{
    return offLine;
}

void Utils::setMode(bool isOffline)
{
    offLine = isOffline;
}*/

void Utils::setTabModel(const QString &dashboardId)
{
    TabModel* tabModel;
    QMap<QString,TabModel*>::iterator i = _tabModelsList.find(dashboardId);
    if (i == _tabModelsList.end()) {
        tabModel = new TabModel(_db);
        tabModel->init(dashboardId);
        _tabModelsList.insert(dashboardId,tabModel);
    } else {
        tabModel = i.value();
    }

    _view->rootContext()->setContextProperty("tabModel", tabModel);
}

void Utils::setFeedModel(const QString &tabId)
{
    FeedModel* feedModel;
    QMap<QString,FeedModel*>::iterator i = _feedModelsList.find(tabId);
    if (i == _feedModelsList.end()) {
        feedModel = new FeedModel(_db);
        feedModel->init(tabId);
        _feedModelsList.insert(tabId,feedModel);
    } else {
        feedModel = i.value();
    }

    _view->rootContext()->setContextProperty("feedModel", feedModel);
}

void Utils::setEntryModel(const QString &feedId)
{
    EntryModel* entryModel;
    QMap<QString,EntryModel*>::iterator i = _entryModelsList.find(feedId);
    if (i == _entryModelsList.end()) {
        entryModel = new EntryModel(_db);
        entryModel->init(feedId);
        _entryModelsList.insert(feedId,entryModel);
    } else {
        if (i.value()->reInit)
            i.value()->init();
        entryModel = i.value();
    }

    _view->rootContext()->setContextProperty("entryModel", entryModel);
}

void Utils::setDashboardModel()
{
    if(!_dashboardModel) {
        _dashboardModel = new DashboardModel(_db);
        _dashboardModel->init();
    }
    _view->rootContext()->setContextProperty("dashboardModel", _dashboardModel);
}

void Utils::updateModels()
{
    // Dashboard
    if(_dashboardModel) {
        _dashboardModel->init();
    }

    // Tabs
    QMap<QString,TabModel*>::iterator ti = _tabModelsList.begin();
    while (ti != _tabModelsList.end()) {
        ti.value()->init();
        ++ti;
    }

    // Feeds
    QMap<QString,FeedModel*>::iterator fi = _feedModelsList.begin();
    while (fi != _feedModelsList.end()) {
        fi.value()->init();
        ++fi;
    }

    // Entries
    QMap<QString,EntryModel*>::iterator ei = _entryModelsList.begin();
    while (ei != _entryModelsList.end()) {
        ei.value()->init();
        ++ei;
    }
}

Utils::~Utils()
{
    QMap<QString,EntryModel*>::iterator ei = _entryModelsList.begin();
    while (ei != _entryModelsList.end()) {
        delete ei.value();
        ++ei;
    }

    QMap<QString,FeedModel*>::iterator fi = _feedModelsList.begin();
    while (fi != _feedModelsList.end()) {
        delete fi.value();
        ++fi;
    }

    QMap<QString,TabModel*>::iterator ti = _tabModelsList.begin();
    while (ti != _tabModelsList.end()) {
        delete ti.value();
        ++ti;
    }

    delete _dashboardModel;
}

QList<QString> Utils::dashboards()
{
    QList<QString> simpleList;
    QList<DatabaseManager::Dashboard> list = _db->readDashboards();
    QList<DatabaseManager::Dashboard>::iterator i = list.begin();
    while (i != list.end()) {
        simpleList.append((*i).title);
        ++i;
    }
    return simpleList;
}

/*void Utils::setAsRead(const QString &entryId)
{
    _db->updateEntryReadFlag(entryId,1);
    QString feedId = _db->readFeedId(entryId);
    QMap<QString,EntryModel*>::iterator i = _entryModelsList.find(feedId);
    if (i != _entryModelsList.end()) {
        i.value()->reInit = true;
    }

    DatabaseManager::Action action;
    action.type = DatabaseManager::SetRead;
    action.entryId = entryId;

    _db->writeAction(action,QDateTime::currentDateTime().toTime_t());
}

void Utils::unsetAsRead(const QString &entryId)
{
    _db->updateEntryReadFlag(entryId,0);
    QString feedId = _db->readFeedId(entryId);
    QMap<QString,EntryModel*>::iterator i = _entryModelsList.find(feedId);
    if (i != _entryModelsList.end()) {
        i.value()->reInit = true;
    }

    DatabaseManager::Action action;
    action.type = DatabaseManager::UnSetRead;
    action.entryId = entryId;

    _db->writeAction(action,QDateTime::currentDateTime().toTime_t());
}

void Utils::setAsReadlater(const QString &entryId)
{
    _db->updateEntryReadlaterFlag(entryId,1);
    QString feedId = _db->readFeedId(entryId);
    QMap<QString,EntryModel*>::iterator i = _entryModelsList.find(feedId);
    if (i != _entryModelsList.end()) {
        i.value()->reInit = true;
    }

    DatabaseManager::Action action;
    action.type = DatabaseManager::SetReadlater;
    action.entryId = entryId;

    _db->writeAction(action,QDateTime::currentDateTime().toTime_t());
}

void Utils::unsetAsReadlater(const QString &entryId)
{
    _db->updateEntryReadlaterFlag(entryId,0);
    QString feedId = _db->readFeedId(entryId);
    QMap<QString,EntryModel*>::iterator i = _entryModelsList.find(feedId);
    if (i != _entryModelsList.end()) {
        i.value()->reInit = true;
    }

    DatabaseManager::Action action;
    action.type = DatabaseManager::UnSetReadlater;
    action.entryId = entryId;

    _db->writeAction(action,QDateTime::currentDateTime().toTime_t());
}*/

