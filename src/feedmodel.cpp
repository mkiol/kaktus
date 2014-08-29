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


#include "feedmodel.h"

FeedModel::FeedModel(DatabaseManager *db, QObject *parent) :
    ListModel(new FeedItem, parent)
{
    _db = db;
    _tabId = "";
}

void FeedModel::init(const QString &tabId)
{
    _tabId = tabId;
    init();
}

void FeedModel::init()
{
    if(rowCount()>0) removeRows(0,rowCount());
    createItems(_tabId);
}


void FeedModel::createItems(const QString &tabId)
{
    QList<DatabaseManager::Feed> list = _db->readFeeds(tabId);
    QList<DatabaseManager::Feed>::iterator i = list.begin();
    while( i != list.end() ) {
        //qDebug() << "feed: " << (*i).id << (*i).title << (*i).streamId;
        appendRow(new FeedItem((*i).id,
                              (*i).title,
                              (*i).content,
                              (*i).link,
                              (*i).url,
                              (*i).icon,
                              (*i).streamId,
                              //(*i).unread,
                              //(*i).read,
                              _db->readEntriesUnreadByFeedCount((*i).id),
                              _db->readEntriesReadByFeedCount((*i).id),
                              (*i).readlater,
                              _db->readEntriesFreshByFeedCount((*i).id)
                             ));
        ++i;
    }
}

void FeedModel::markAsUnread(int row)
{
    FeedItem* item = static_cast<FeedItem*>(readRow(row));
    _db->updateEntriesReadFlag(item->id(),0);
    item->setRead(0); item->setUnread(_db->readEntriesUnreadByFeedCount(item->id()));

    DatabaseManager::Action action;
    action.type = DatabaseManager::UnSetFeedReadAll;
    action.feedId = item->id();
    action.olderDate = _db->readFeedLastUpadate(item->id());
    _db->writeAction(action);
}

void FeedModel::markAsRead(int row)
{
    FeedItem* item = static_cast<FeedItem*>(readRow(row));
    _db->updateEntriesReadFlag(item->id(),1);
    item->setUnread(0); item->setRead(_db->readEntriesReadByFeedCount(item->id()));

    DatabaseManager::Action action;
    action.type = DatabaseManager::SetFeedReadAll;
    action.feedId = item->id();
    action.olderDate = _db->readFeedLastUpadate(item->id());
    _db->writeAction(action);
}

int FeedModel::countRead()
{
    return _db->readEntriesReadByTabCount(_tabId);
}

int FeedModel::countUnread()
{
    return _db->readEntriesUnreadByTabCount(_tabId);
}

void FeedModel::setAllAsUnread()
{
    _db->updateEntryReadFlagByTab(_tabId,0);
    updateFlags();

    DatabaseManager::Action action;
    action.type = DatabaseManager::UnSetTabReadAll;
    action.feedId = _tabId;
    action.olderDate = _db->readTabLastUpadate(_tabId);
    _db->writeAction(action);
}

void FeedModel::updateFlags()
{
    int l = this->rowCount();
    for (int i=0; i<l; ++i) {
        FeedItem* item = static_cast<FeedItem*>(readRow(i));
        item->setUnread(_db->readEntriesUnreadByFeedCount(item->uid()));
        item->setRead(_db->readEntriesReadByFeedCount(item->uid()));
    }
}

void FeedModel::setAllAsRead()
{
    _db->updateEntryReadFlagByTab(_tabId,1);
    updateFlags();

    DatabaseManager::Action action;
    action.type = DatabaseManager::SetTabReadAll;
    action.feedId = _tabId;
    action.olderDate = _db->readTabLastUpadate(_tabId);
    _db->writeAction(action);
}


// ----------------------------------------------------------------

FeedItem::FeedItem(const QString &uid,
                   const QString &title,
                   const QString &content,
                   const QString &link,
                   const QString &url,
                   const QString &icon,
                   const QString &streamId,
                   int unread,
                   int read,
                   int readlater,
                   int fresh,
                   QObject *parent) :
    ListItem(parent),
    m_uid(uid),
    m_title(title),
    m_content(content),
    m_link(link),
    m_url(url),
    m_icon(icon),
    m_streamid(streamId),
    m_unread(unread),
    m_read(read),
    m_readlater(readlater),
    m_fresh(fresh)
{}

QHash<int, QByteArray> FeedItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[UidRole] = "uid";
    names[TitleRole] = "title";
    names[ContentRole] = "content";
    names[LinkRole] = "link";
    names[UrlRole] = "url";
    names[IconRole] = "icon";
    names[StreamIdRole] = "streamId";
    names[UnreadRole] = "unread";
    names[ReadRole] = "read";
    names[ReadlaterRole] = "readlater";
    names[FreshRole] = "fresh";
    return names;
}

QVariant FeedItem::data(int role) const
{
    switch(role) {
    case UidRole:
        return uid();
    case TitleRole:
        return title();
    case ContentRole:
        return content();
    case LinkRole:
        return link();
    case UrlRole:
        return url();
    case IconRole:
        return icon();
    case StreamIdRole:
        return streamId();
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

void FeedItem::setReadlater(int value)
{
    m_readlater = value;
    emit dataChanged();
}

void FeedItem::setUnread(int value)
{
    m_unread = value;
    emit dataChanged();
}

void FeedItem::setRead(int value)
{
    m_read = value;
    emit dataChanged();
}

