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

#include <QList>
#include <QDebug>
#include <QModelIndex>
#include <QTextDocument>
#include <QChar>
#include <QRegExp>

#include "entrymodel.h"

EntryModel::EntryModel(DatabaseManager *db, QObject *parent) :
    ListModel(new EntryItem, parent)
{
    _db = db;
    reInit = false;

    Settings *s = Settings::instance();
    connect(s,SIGNAL(showOnlyUnreadChanged()),this,SLOT(init()));
}

void EntryModel::init(const QString &feedId)
{
    if(rowCount()>0) removeRows(0,rowCount());
    _feedId = feedId;
    Settings *s = Settings::instance();
    createItems(0,s->getOffsetLimit());
}

void EntryModel::init()
{
    reInit = false;
    if(rowCount()>0) removeRows(0,rowCount());
    Settings *s = Settings::instance();
    createItems(0,s->getOffsetLimit());
}

void EntryModel::createItems(int offset, int limit)
{
    QList<DatabaseManager::Entry> list;

    Settings *s = Settings::instance();

    if (_feedId == "readlater") {
        list = _db->readEntriesReadlater(s->getDashboardInUse(),offset,limit);
    } else {
        int mode = s->getViewMode();
        switch (mode) {
        case 0:
            // View mode: Tabs->Feeds->Entries
            if (s->getShowOnlyUnread())
                list = _db->readEntriesUnreadByFeed(_feedId,offset,limit);
            else
                list = _db->readEntriesByFeed(_feedId,offset,limit);
            break;
        case 1:
            // View mode: Tabs->Entries
            if (s->getShowOnlyUnread())
                list = _db->readEntriesUnreadByTab(_feedId,offset,limit);
            else
                list = _db->readEntriesByTab(_feedId,offset,limit);
            break;
        case 2:
            // View mode: Feeds->Entries
            if (s->getShowOnlyUnread())
                list = _db->readEntriesUnreadByFeed(_feedId,offset,limit);
            else
                list = _db->readEntriesByFeed(_feedId,offset,limit);
            break;
        case 3:
            // View mode: Entries
            if (s->getShowOnlyUnread())
                list = _db->readEntriesUnread(s->getDashboardInUse(),offset,limit);
            else
                list = _db->readEntries(s->getDashboardInUse(),offset,limit);
            break;
        }
    }

    QList<DatabaseManager::Entry>::iterator i = list.begin();
    while( i != list.end() ) {

        // Removing html tags!
        QTextDocument doc;
        doc.setHtml((*i).content);
        QString content = doc.toPlainText()
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                .replace(QChar::ObjectReplacementCharacter,QChar::Space)
#else
                .replace(QChar::ObjectReplacementCharacter,QChar(0x0020))
#endif
                .simplified();
        if (content.length()>1000)
            content = content.left(997)+"...";

        doc.setHtml((*i).title);
        QString title = doc.toPlainText()
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                .replace(QChar::ObjectReplacementCharacter,QChar::Space)
#else
                .replace(QChar::ObjectReplacementCharacter,QChar(0x0020))
#endif
                .simplified();
        if (title.length()>200)
            title = title.left(197)+QString("...");

        /*QRegExp rx("(\\S*)\\s*\((\\S*)\)", Qt::CaseInsensitive);
        if (rx.indexIn((*i).author)!=-1) {
            qDebug() << "(*i).author:" << (*i).author << "cap:" << rx.cap(1).toUtf8();
            //(*i).author = rx.cap(1).toUtf8();
        }*/

        appendRow(new EntryItem((*i).id,
                                title,
                                (*i).author,
                                content,
                                (*i).link,
                                (*i).image,
                                (*i).feedIcon,
                                _db->isCacheItemExistsByEntryId((*i).id),
                                (*i).fresh,
                                (*i).read,
                                (*i).readlater,
                                (*i).date
                                ));
        ++i;
    }
}

void EntryModel::setAllAsUnread()
{
    int l = this->rowCount();
    for (int i=0; i<l; ++i) {
        EntryItem* item = static_cast<EntryItem*>(readRow(i));
        item->setRead(0);
    }

    // DB change & Action
    Settings *s = Settings::instance();
    DatabaseManager::Action action;
    int mode = s->getViewMode();
    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        _db->updateEntriesReadFlagByFeed(_feedId,0);

        action.type = DatabaseManager::UnSetFeedReadAll;
        action.feedId = _feedId;
        action.olderDate = _db->readFeedLastUpdateByFeed(_feedId);

        break;
    case 1:
        // View mode: Tabs->Entries
        _db->updateEntriesReadFlagByTab(_feedId,0);

        action.type = DatabaseManager::UnSetTabReadAll;
        action.feedId = _feedId;
        action.olderDate = _db->readTabLastUpadate(_feedId);

        break;
    case 2:
        // View mode: Feeds->Entries
        _db->updateEntriesReadFlagByFeed(_feedId,0);

        action.type = DatabaseManager::UnSetFeedReadAll;
        action.feedId = _feedId;
        action.olderDate = _db->readFeedLastUpdateByFeed(_feedId);

        break;
    case 3:
        // View mode: Entries
        _db->updateEntriesReadFlag(s->getDashboardInUse(),0);

        action.type = DatabaseManager::UnSetAllRead;
        action.feedId = s->getDashboardInUse();
        action.olderDate = _db->readFeedLastUpdate(s->getDashboardInUse());

        break;
    }

    _db->writeAction(action);
}

void EntryModel::setAllAsRead()
{
    int l = this->rowCount();
    for (int i=0; i<l; ++i) {
        EntryItem* item = static_cast<EntryItem*>(readRow(i));
        item->setRead(1);
    }

    // DB change
    Settings *s = Settings::instance();
    DatabaseManager::Action action;
    int mode = s->getViewMode();
    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        _db->updateEntriesReadFlagByFeed(_feedId,1);

        action.type = DatabaseManager::SetFeedReadAll;
        action.feedId = _feedId;
        action.olderDate = _db->readFeedLastUpdateByFeed(_feedId);

        break;
    case 1:
        // View mode: Tabs->Entries
        _db->updateEntriesReadFlagByTab(_feedId,1);

        action.type = DatabaseManager::SetTabReadAll;
        action.feedId = _feedId;
        action.olderDate = _db->readTabLastUpadate(_feedId);

        break;
    case 2:
        // View mode: Feeds->Entries
        _db->updateEntriesReadFlagByFeed(_feedId,1);

        action.type = DatabaseManager::SetFeedReadAll;
        action.feedId = _feedId;
        action.olderDate = _db->readFeedLastUpdateByFeed(_feedId);

        break;
    case 3:
        // View mode: Entries
        _db->updateEntriesReadFlag(s->getDashboardInUse(),1);

        action.type = DatabaseManager::SetAllRead;
        action.feedId = s->getDashboardInUse();
        action.olderDate = _db->readFeedLastUpdate(s->getDashboardInUse());

        break;
    }

    _db->writeAction(action);
}

int EntryModel::countRead()
{
    Settings *s = Settings::instance();
    int mode = s->getViewMode();
    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        return _db->readEntriesReadByFeedCount(_feedId);
        break;
    case 1:
        // View mode: Tabs->Entries
        return _db->readEntriesReadByTabCount(_feedId);
        break;
    case 2:
        // View mode: Feeds->Entries
        return _db->readEntriesReadByFeedCount(_feedId);
        break;
    case 3:
        // View mode: Entries
        return _db->readEntriesReadCount(s->getDashboardInUse());
        break;
    }

    return 0;
}

int EntryModel::countUnread()
{
    /*int unread = 0; int l = this->rowCount();
    for (int i=0; i<l; ++i) {
        EntryItem* item = static_cast<EntryItem*>(readRow(i));
        if (item->read() == 0)
            ++unread;
    }
    return unread;*/

    Settings *s = Settings::instance();
    int mode = s->getViewMode();
    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        return _db->readEntriesUnreadByFeedCount(_feedId);
        break;
    case 1:
        // View mode: Tabs->Entries
        return _db->readEntriesUnreadByTabCount(_feedId);
        break;
    case 2:
        // View mode: Feeds->Entries
        return _db->readEntriesUnreadByFeedCount(_feedId);
        break;
    case 3:
        // View mode: Entries
        return _db->readEntriesUnreadCount(s->getDashboardInUse());
        break;
    }

    return 0;
}

int EntryModel::count()
{
    return this->rowCount();
}

void EntryModel::setData(int row, const QString &fieldName, QVariant newValue)
{
    EntryItem* item = static_cast<EntryItem*>(readRow(row));

    if (fieldName=="readlater") {
        item->setReadlater(newValue.toInt());
        DatabaseManager::Action action;
        Settings *s = Settings::instance();
        action.feedId = s->db->readFeedId(item->id());
        if (newValue==1) {
            action.type = DatabaseManager::SetReadlater;
            action.entryId = item->id();
        } else {
            action.type = DatabaseManager::UnSetReadlater;
            action.entryId = item->id();
        }
        _db->writeAction(action);
        _db->updateEntryReadlaterFlag(item->id(),newValue.toInt());
    }

    if (fieldName=="read") {
        item->setRead(newValue.toInt());
        DatabaseManager::Action action;
        Settings *s = Settings::instance();
        action.feedId = s->db->readFeedId(item->id());
        if (newValue==1) {
            action.type = DatabaseManager::SetRead;
            action.entryId = item->id();
        } else {
            action.type = DatabaseManager::UnSetRead;
            action.entryId = item->id();
        }
        _db->writeAction(action);
        _db->updateEntryReadFlag(item->id(),newValue.toInt());
    }
}

// ----------------------------------------------------------------

EntryItem::EntryItem(const QString &uid,
                   const QString &title,
                   const QString &author,
                   const QString &content,
                   const QString &link,
                   const QString &image,
                   const QString &feedIcon,
                   const bool cached,
                   const bool fresh,
                   const int read,
                   const int readlater,
                   const int date,
                   QObject *parent) :
    ListItem(parent),
    m_uid(uid),
    m_title(title),
    m_author(author),
    m_content(content),
    m_link(link),
    m_image(image),
    m_feedIcon(feedIcon),
    m_cached(cached),
    m_fresh(fresh),
    m_read(read),
    m_readlater(readlater),
    m_date(date)
{}

QHash<int, QByteArray> EntryItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[UidRole] = "uid";
    names[TitleRole] = "title";
    names[AuthorRole] = "author";
    names[ContentRole] = "content";
    names[LinkRole] = "link";
    names[ImageRole] = "image";
    names[FeedIconRole] = "feedIcon";
    names[CachedRole] = "cached";
    names[FreshRole] = "fresh";
    names[ReadRole] = "read";
    names[ReadLaterRole] = "readlater";
    names[DateRole] = "date";
    return names;
}

QVariant EntryItem::data(int role) const
{
    switch(role) {
    case UidRole:
        return uid();
    case TitleRole:
        return title();
    case AuthorRole:
        return author();
    case ContentRole:
        return content();
    case LinkRole:
        return link();
    case ImageRole:
        return image();
    case FeedIconRole:
        return feedIcon();
    case CachedRole:
        return cached();
    case FreshRole:
        return fresh();
    case ReadRole:
        return read();
    case ReadLaterRole:
        return readlater();
    case DateRole:
        return date();
    default:
        return QVariant();
    }
}

void EntryItem::setReadlater(int value)
{
    m_readlater = value;
    emit dataChanged();
}

void EntryItem::setRead(int value)
{
    m_read = value;
    emit dataChanged();
}
