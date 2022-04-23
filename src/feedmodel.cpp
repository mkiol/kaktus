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

#include <QRegExp>

#include "databasemanager.h"
#include "feedmodel.h"

FeedModel::FeedModel(QObject *parent) :
    ListModel(new FeedItem, parent),
    _tabId()
{
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
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    QList<DatabaseManager::Stream> list;

    int mode = s->getViewMode();

    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        list = db->readStreamsByTab(tabId);
        break;
    case 1:
        // View mode: Tabs->Entries
        list = db->readStreamsByTab(tabId);
        break;
    case 2:
        // View mode: Feeds->Entries
        list = db->readStreamsByDashboard(s->getDashboardInUse());
        break;
    case 3:
        // View mode: Entries
    case 4:
        // View mode: Saved
    case 5:
        // View mode: Slow
        qWarning() << "Error: This should never happened";
        return;
    }

    QRegExp re("<[^>]*>");
    QList<DatabaseManager::Stream>::iterator i = list.begin();
    while (i != list.end() ) {
        appendRow(new FeedItem((*i).id,
                              (*i).title.remove(re),
                              (*i).content,
                              (*i).link,
                              (*i).query,
                              (*i).icon,
                              0,
                              db->countEntriesUnreadByStream((*i).id),
                              db->countEntriesReadByStream((*i).id),
                              (*i).saved,
                              db->countEntriesFreshByStream((*i).id)
                             ));
        ++i;
    }

    // Dummy row as workaround!
    if (!list.isEmpty())
        appendRow(new FeedItem("last","","","","","",0,0,0,0,0));
}

void FeedModel::markAsUnread(int row)
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    if (s->getSigninType() >= 10) {
        // markAsUnread not supported in API
        qWarning() << "Mark feed as unread is not supported";
        return;
    }

    FeedItem* item = dynamic_cast<FeedItem*>(readRow(row));
    db->updateEntriesReadFlagByStream(item->id(),0);
    item->setRead(0); item->setUnread(db->countEntriesUnreadByStream(item->id()));

    DatabaseManager::Action action;
    action.type = DatabaseManager::UnSetStreamReadAll;
    action.id1 = item->id();
    action.date1 = db->readLastUpdateByStream(item->id());
    db->writeAction(action);
}

void FeedModel::markAsRead(int row)
{
    auto db = DatabaseManager::instance();

    FeedItem* item = dynamic_cast<FeedItem*>(readRow(row));
    db->updateEntriesReadFlagByStream(item->id(),1);
    item->setUnread(0); item->setRead(db->countEntriesReadByStream(item->id()));

    DatabaseManager::Action action;
    action.type = DatabaseManager::SetStreamReadAll;
    action.id1 = item->id();
    action.date1 = db->readLastUpdateByStream(item->id());
    db->writeAction(action);
}

int FeedModel::countRead()
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    int mode = s->getViewMode();

    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        return db->countEntriesReadByTab(_tabId);
        break;
    case 1:
        // View mode: Tabs->Entries
        qWarning() << "Error: This should never happened";
        return 0;
    case 2:
        // View mode: Feeds->Entries
        return db->countEntriesReadByDashboard(s->getDashboardInUse());
    case 3:
        // View mode: Entries
    case 4:
        // View mode: Saved
    case 5:
        // View mode: Slow
        qWarning() << "Error: This should never happened";
        return 0;
    }

    return 0;
}

int FeedModel::countUnread()
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    int mode = s->getViewMode();

    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        return db->countEntriesUnreadByTab(_tabId);
        break;
    case 1:
        // View mode: Tabs->Entries
        qWarning() << "Error: This should never happened";
        return 0;
    case 2:
        // View mode: Feeds->Entries
        return db->countEntriesUnreadByDashboard(s->getDashboardInUse());
    case 3:
        // View mode: Entries
    case 4:
        // View mode: Saved
    case 5:
        // View mode: Slow
        qWarning() << "Error: This should never happened";
        return 0;
    }

    return 0;
}

void FeedModel::setAllAsUnread()
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    if (s->getSigninType() >= 10) {
        // setAllAsUnread not supported in API
        qWarning() << "Mark tab as unread is not supported";
        return;
    }

    DatabaseManager::Action action;
    int mode = s->getViewMode();
    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        db->updateEntriesReadFlagByTab(_tabId,0);
        action.type = DatabaseManager::UnSetTabReadAll;
        action.id1 = _tabId;
        action.date1 = db->readLastUpdateByTab(_tabId);
        break;
    case 1:
        // View mode: Tabs->Entries
        qWarning() << "Error: This should never happened";
        return;
    case 2:
        // View mode: Feeds->Entries
        db->updateEntriesReadFlagByDashboard(s->getDashboardInUse(),0);
        action.type = DatabaseManager::UnSetAllRead;
        action.id1 = s->getDashboardInUse();
        action.date1 = db->readLastUpdateByTab(_tabId);
        break;
    case 3:
        // View mode: Entries
    case 4:
        // View mode: Saved
    case 5:
        // View mode: Slow
        qWarning() << "Error: This should never happened";
        return;
    }

    updateFlags();

    db->writeAction(action);
}

void FeedModel::setAllAsRead()
{
    auto s = Settings::instance();
    auto db = DatabaseManager::instance();

    DatabaseManager::Action action;

    int mode = s->getViewMode();

    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        db->updateEntriesReadFlagByTab(_tabId,1);
        action.type = DatabaseManager::SetTabReadAll;
        action.id1 = _tabId;
        action.date1 = db->readLastUpdateByTab(_tabId);
        break;
    case 1:
        // View mode: Tabs->Entries
        qWarning() << "Error: This should never happened";
        return;
    case 2:
        // View mode: Feeds->Entries
        db->updateEntriesReadFlagByDashboard(s->getDashboardInUse(),1);
        action.type = DatabaseManager::SetAllRead;
        action.id1 = s->getDashboardInUse();
        action.date1 = db->readLastUpdateByDashboard(s->getDashboardInUse());
        break;
    case 3:
        // View mode: Entries
    case 4:
        // View mode: Saved
    case 5:
        // View mode: Slow
        qWarning() << "Error: This should never happened";
        return;
    }

    updateFlags();

    db->writeAction(action);
}

void FeedModel::updateFlags()
{
    auto db = DatabaseManager::instance();

    int l = this->rowCount();
    for (int i = 0; i < l; ++i) {
        FeedItem* item = dynamic_cast<FeedItem*>(readRow(i));
        item->setUnread(db->countEntriesUnreadByStream(item->uid()));
        item->setRead(db->countEntriesReadByStream(item->uid()));
    }
}

int FeedModel::count()
{
    return this->rowCount();
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
    if (m_readlater!=value) {
        m_readlater = value;
        emit dataChanged();
    }
}

void FeedItem::setUnread(int value)
{
    if (m_unread!=value) {
        m_unread = value;
        emit dataChanged();
    }
}

void FeedItem::setRead(int value)
{
    if (m_read!=value) {
        m_read = value;
        emit dataChanged();
    }
}

