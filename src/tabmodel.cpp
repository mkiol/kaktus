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
    appendRow(new TabItem("readlater",tr("Saved"),"",0,0,0));

    QList<DatabaseManager::Tab> list = _db->readTabs(dashboardId);
    QList<DatabaseManager::Tab>::iterator i = list.begin();
    while( i != list.end() ) {
        DatabaseManager::Flags flags = _db->readTabFlags((*i).id);
        appendRow(new TabItem((*i).id,
                              (*i).title,
                              (*i).icon,
                              flags.unread,
                              flags.read,
                              flags.readlater
                             ));
        ++i;
    }
}

void TabModel::sort()
{
}

void TabModel::updateFlags()
{
    int i, size = this->rowCount();
    for (i=0; i<size; ++i) {
        TabItem* item = static_cast<TabItem*>(this->readRow(i));
        DatabaseManager::Flags flags = _db->readTabFlags(item->id());
        item->setUnread(flags.unread);
        //item.setRead(flags.read);
        //item.setReadlater(flags.readlater);
    }
}

void TabModel::markAllAsUnread(int row)
{
    TabItem* item = static_cast<TabItem*>(readRow(row));
    int unread = item->unread();
    int read = item->read();
    item->setUnread(unread+read);
    item->setRead(0);

    if (_db->updateFeedAllAsUnreadByTab(item->id()) &&
        _db->updateEntriesReadFlagByTab(item->id(),0)) {

        DatabaseManager::Action action;
        action.type = DatabaseManager::UnSetTabReadAll;
        action.feedId = item->id();
        action.olderDate = _db->readTabLastUpadate(item->id());
        _db->writeAction(action);

    } else {
        qWarning() << "Unable to update Read flag";
    }
}

void TabModel::markAllAsRead(int row)
{
    TabItem* item = static_cast<TabItem*>(readRow(row));
    int unread = item->unread();
    int read = item->read();
    item->setRead(unread+read);
    item->setUnread(0);


    //qDebug() << _db->updateTabReadFlag(item->id(),0,unread+read);
    //qDebug() << _db->updateFeedAllAsReadByTab(item->id());
    //qDebug() << _db->updateEntriesReadFlagByTab(item->id(),1);

    if (_db->updateFeedAllAsReadByTab(item->id()) &&
        _db->updateEntriesReadFlagByTab(item->id(),1)) {

        DatabaseManager::Action action;
        action.type = DatabaseManager::SetTabReadAll;
        action.feedId = item->id();
        action.olderDate = _db->readTabLastUpadate(item->id());
        _db->writeAction(action);

    } else {
        qWarning() << "Unable to update Read flag";
    }
}

// ----------------------------------------------------------------

TabItem::TabItem(const QString &uid,
                   const QString &title,
                   const QString &icon,
                   int unread,
                   int read,
                   int readlater,
                   QObject *parent) :
    ListItem(parent),
    m_uid(uid),
    m_title(title),
    m_icon(icon),
    m_unread(unread),
    m_read(read),
    m_readlater(readlater)
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

