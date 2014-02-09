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
                              (*i).streamId,
                              (*i).unread,
                              (*i).readlater
                             ));
        ++i;
    }
}

void FeedModel::sort()
{
}

int FeedModel::count()
{
    return this->rowCount();
}

QObject* FeedModel::get(int i)
{
    return (QObject*) this->readRow(i);
}

void FeedModel::setData(int row, const QString &fieldName, QVariant newValue)
{
    FeedItem* item = static_cast<FeedItem*>(readRow(row));

    if (fieldName=="readlater") {
        item->setReadlater(newValue.toInt());
        //_db->updateFeedReadlaterFlag(item->id(),newValue.toInt());
    }

    if (fieldName=="unread") {
        item->setUnread(newValue.toInt());
        //_db->updateFeedUnreadFlag(item->id(),newValue.toInt());
    }
}

// ----------------------------------------------------------------

FeedItem::FeedItem(const QString &uid,
                   const QString &title,
                   const QString &content,
                   const QString &link,
                   const QString &url,
                   const QString &streamId,
                   int unread,
                   int readlater,
                   QObject *parent) :
    ListItem(parent),
    m_uid(uid),
    m_title(title),
    m_content(content),
    m_link(link),
    m_url(url),
    m_streamid(streamId),
    m_unread(unread),
    m_readlater(readlater)
{}

QHash<int, QByteArray> FeedItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[UidRole] = "uid";
    names[TitleRole] = "title";
    names[ContentRole] = "content";
    names[LinkRole] = "link";
    names[UrlRole] = "url";
    names[StreamIdRole] = "streamId";
    names[UnreadRole] = "unread";
    names[ReadlaterRole] = "readlater";
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
    case StreamIdRole:
        return streamId();
    case UnreadRole:
        return unread();
    case ReadlaterRole:
        return readlater();
    default:
        return QVariant();
    }
}

void FeedItem::setReadlater(int value)
{
    m_readlater = value;
}

void FeedItem::setUnread(int value)
{
    m_unread = value;
}

