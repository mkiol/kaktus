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

#include "tabmodel.h"

TabModel::TabModel(DatabaseManager *db, QObject *parent) :
    ListModel(new TabItem, parent)
{
    _db = db;
    _dashboardId = "";
}

void TabModel::init(const QString &dashboardId)
{
    _dashboardId = dashboardId;
    init();
}

void TabModel::init()
{
    if(rowCount()>0) removeRows(0,rowCount());
    createItems(_dashboardId);
}

void TabModel::createItems(const QString &dashboardId)
{
    // Readlater extra Tab, id="readlater"
    appendRow(new TabItem("readlater",tr("Saved"),"",0,0,0,0));

    QList<DatabaseManager::Tab> list = _db->readTabs(dashboardId);
    QList<DatabaseManager::Tab>::iterator i = list.begin();
    while( i != list.end() ) {
        appendRow(new TabItem((*i).id,
                              (*i).title,
                              (*i).icon,
                              _db->readEntriesUnreadByTabCount((*i).id),
                              _db->readEntriesReadByTabCount((*i).id),
                              0,
                              _db->readEntriesFreshByTabCount((*i).id)
                             ));
        ++i;
    }
}

void TabModel::updateFlags()
{
    int l = this->rowCount();
    for (int i=0; i<l; ++i) {
        TabItem* item = static_cast<TabItem*>(this->readRow(i));
        item->setUnread(_db->readEntriesUnreadByTabCount(item->uid()));
        item->setRead(_db->readEntriesReadByTabCount(item->uid()));
    }
}

void TabModel::markAsUnread(int row)
{
    TabItem* item = static_cast<TabItem*>(readRow(row));
    _db->updateEntriesReadFlagByTab(item->id(),0);
    item->setRead(0); item->setUnread(_db->readEntriesUnreadByTabCount(item->id()));

    DatabaseManager::Action action;
    action.type = DatabaseManager::UnSetTabReadAll;
    action.feedId = item->id();
    action.olderDate = _db->readTabLastUpadate(item->id());
    _db->writeAction(action);
}

void TabModel::markAsRead(int row)
{
    TabItem* item = static_cast<TabItem*>(readRow(row));
    _db->updateEntriesReadFlagByTab(item->id(),1);
    item->setUnread(0); item->setRead(_db->readEntriesReadByTabCount(item->id()));

    DatabaseManager::Action action;
    action.type = DatabaseManager::SetTabReadAll;
    action.feedId = item->id();
    action.olderDate = _db->readTabLastUpadate(item->id());
    _db->writeAction(action);
}

int TabModel::countRead()
{
    Settings *s = Settings::instance();
    return _db->readEntriesReadCount(s->getDashboardInUse());
}

int TabModel::countUnread()
{
    Settings *s = Settings::instance();
    return _db->readEntriesUnreadCount(s->getDashboardInUse());
}

void TabModel::setAllAsUnread()
{
    Settings *s = Settings::instance();
    DatabaseManager::Action action;

    _db->updateEntriesReadFlag(s->getDashboardInUse(),0);

    action.type = DatabaseManager::UnSetAllRead;
    action.feedId = s->getDashboardInUse();
    action.olderDate = _db->readFeedLastUpdate(s->getDashboardInUse());

    updateFlags();

    _db->writeAction(action);
}

void TabModel::setAllAsRead()
{
    Settings *s = Settings::instance();
    DatabaseManager::Action action;

    _db->updateEntriesReadFlag(s->getDashboardInUse(),1);

    action.type = DatabaseManager::SetAllRead;
    action.feedId = s->getDashboardInUse();
    action.olderDate = _db->readFeedLastUpdate(s->getDashboardInUse());

    updateFlags();

    _db->writeAction(action);
}

// ----------------------------------------------------------------

TabItem::TabItem(const QString &uid,
                   const QString &title,
                   const QString &icon,
                   int unread,
                   int read,
                   int readlater,
                   int fresh,
                   QObject *parent) :
    ListItem(parent),
    m_uid(uid),
    m_title(title),
    m_icon(icon),
    m_unread(unread),
    m_read(read),
    m_readlater(readlater),
    m_fresh(fresh)
{}

QHash<int, QByteArray> TabItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[UidRole] = "uid";
    names[TitleRole] = "title";
    names[IconRole] = "iconUrl";
    names[UnreadRole] = "unread";
    names[ReadRole] = "read";
    names[ReadlaterRole] = "readlater";
    names[FreshRole] = "fresh";
    return names;
}

QVariant TabItem::data(int role) const
{
    switch(role) {
    case UidRole:
        return uid();
    case TitleRole:
        return title();
    case IconRole:
        return icon();
    case UnreadRole:
        return unread();
    case ReadRole:
        return read();
    case ReadlaterRole:
        return readlater();
    case FreshRole:
        return fresh();
    default:
        return QVariant();
    }
}

void TabItem::setReadlater(int value)
{
    m_readlater = value;
    emit dataChanged();
}

void TabItem::setUnread(int value)
{
    m_unread = value;
    emit dataChanged();
}

void TabItem::setRead(int value)
{
    m_read = value;
    emit dataChanged();
}

