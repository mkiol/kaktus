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
//#include <QStringList>
#include <QDebug>
#include <QModelIndex>
#include <QTextDocument>
#include <QChar>
//#include <QDateTime>
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
    createItems(feedId);
}

void EntryModel::init()
{
    reInit = false;
    if(rowCount()>0) removeRows(0,rowCount());
    createItems(_feedId);
}

void EntryModel::createItems(const QString &feedId)
{
    QList<DatabaseManager::Entry> list;

    Settings *s = Settings::instance();

    if (feedId == "readlater") {
        list = _db->readEntriesReadlater();
    } else {
        if (s->getShowOnlyUnread())
            list = _db->readEntriesUnread(feedId);
        else
            list = _db->readEntries(feedId);
    }

    QList<DatabaseManager::Entry>::iterator i = list.begin();
    while( i != list.end() ) {

        // Removing html tags!
        QTextDocument doc;
        doc.setHtml((*i).content);
        QString content = doc.toPlainText()
                .replace(QChar::ObjectReplacementCharacter,QChar::Space)
                .simplified();
        if (content.length()>1000)
            content = content.left(997)+"...";

        doc.setHtml((*i).title);
        QString title = doc.toPlainText()
                .replace(QChar::ObjectReplacementCharacter,QChar::Space)
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
                                _db->isCacheItemExistsByEntryId((*i).id),
                                (*i).read,
                                (*i).readlater,
                                (*i).date
                                ));
        ++i;
    }
}

/*int EntryModel::count()
{
    return this->rowCount();
}

QObject* EntryModel::get(int i)
{
    return (QObject*) this->readRow(i);
}*/

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
                   const bool cached,
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
    m_cached(cached),
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
    names[CachedRole] = "cached";
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
    case CachedRole:
        return cached();
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
