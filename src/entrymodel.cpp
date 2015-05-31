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
#include <QtGui/QTextDocument>
#include <QChar>
#include <QRegExp>
#include <QDateTime>
#include <QUrl>

#include "entrymodel.h"
#include "utils.h"

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
    emit ready();
}

int EntryModel::getDateRowId(int date)
{
    QDateTime qdate = QDateTime::fromTime_t(date);
    int days = qdate.daysTo(QDateTime::currentDateTimeUtc());
    if (days==0)
        return 1;
    if (days==1)
        return 2;
    if (Utils::isSameWeek(qdate.date(),QDate::currentDate()))
        return 3;
    int months = Utils::monthsTo(qdate.date(),QDate::currentDate());
    if (months==0)
        return 4;
    if (months==1)
        return 5;
    if (Utils::yearsTo(qdate.date(),QDate::currentDate())==0)
        return 6;
    return 10;
}

int EntryModel::createItems(int offset, int limit)
{
    QList<DatabaseManager::Entry> list;

    Settings *s = Settings::instance();

    int mode = s->getViewMode();
    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        if (s->getShowOnlyUnread())
            list = _db->readEntriesUnreadByStream(_feedId,offset,limit);
        else
            list = _db->readEntriesByStream(_feedId,offset,limit);
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
            list = _db->readEntriesUnreadByStream(_feedId,offset,limit);
        else
            list = _db->readEntriesByStream(_feedId,offset,limit);
        break;
    case 3:
        // View mode: Entries
        if (s->getShowOnlyUnread())
            list = _db->readEntriesUnreadByDashboard(s->getDashboardInUse(),offset,limit);
        else
            list = _db->readEntriesByDashboard(s->getDashboardInUse(),offset,limit);
        break;
    case 4:
        // View mode: Saved
        list = _db->readEntriesSavedByDashboard(s->getDashboardInUse(),offset,limit);
        break;
    case 5:
        // View mode: Slow
        if (s->getShowOnlyUnread())
            list = _db->readEntriesSlowUnreadByDashboard(s->getDashboardInUse(),offset,limit);
        else
            list = _db->readEntriesSlowByDashboard(s->getDashboardInUse(),offset,limit);
        break;
    }

    // Remove dummy row
    if (list.count()>0) {
        int l = rowCount();
        if (l>0) {
            EntryItem* item = dynamic_cast<EntryItem*>(readRow(l-1));
            //qDebug() << "item->id()" << item->id() << "l" << l;
            if (item->id()=="last")
                removeRow(l-1);
        }
    }

    QList<DatabaseManager::Entry>::iterator i = list.begin();

    int prevDateRow = 0;
    if (rowCount()>0) {
        EntryItem* item = dynamic_cast<EntryItem*>(readRow(rowCount()-1));
        prevDateRow = getDateRowId(item->date());
    }

    QRegExp re("<[^>]*>");
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
        //QString title = doc.toPlainText().remove(QRegExp("<[^>]*>"))
        QString title = doc.toPlainText()
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                .replace(QChar::ObjectReplacementCharacter,QChar::Space)
#else
                .replace(QChar::ObjectReplacementCharacter,QChar(0x0020))
#endif
                .simplified();
        if (title.length()>200)
            title = title.left(197)+QString("...");

        //qDebug() << title;

        /*QRegExp rx("(\\S*)\\s*\((\\S*)\)", Qt::CaseInsensitive);
        if (rx.indexIn((*i).author)!=-1) {
            qDebug() << "(*i).author:" << (*i).author << "cap:" << rx.cap(1).toUtf8();
            //(*i).author = rx.cap(1).toUtf8();
        }*/

        // Detecting transparent images
        bool imageOk = true;
        QUrl imageUrl((*i).image);
        //qDebug() << imageUrl.path();
        if (imageUrl.path() == "/assets/images/transparent.png")
            imageOk = false;

        // Adding date row
        int dateRow = getDateRowId((*i).publishedAt);
        if (dateRow>prevDateRow) {
            switch (dateRow) {
            case 1:
                appendRow(new EntryItem("daterow",tr("Today"),"","","","","","",false,0,0,0,0));
                break;
            case 2:
                appendRow(new EntryItem("daterow",tr("Yesterday"),"","","","","","",false,0,0,0,0));
                break;
            case 3:
                appendRow(new EntryItem("daterow",tr("Current week"),"","","","","","",false,0,0,0,0));
                break;
            case 4:
                appendRow(new EntryItem("daterow",tr("Current month"),"","","","","","",false,0,0,0,0));
                break;
            case 5:
                appendRow(new EntryItem("daterow",tr("Previous month"),"","","","","","",false,0,0,0,0));
                break;
            case 6:
                appendRow(new EntryItem("daterow",tr("Current year"),"","","","","","",false,0,0,0,0));
                break;
            default:
                appendRow(new EntryItem("daterow",tr("Previous year & older"),"","","","","","",false,0,0,0,0));
                break;
            }
        }
        prevDateRow = dateRow;
        appendRow(new EntryItem((*i).id,
                                title.remove(re),
                                (*i).author,
                                content,
                                (*i).link,
                                imageOk? (*i).image : "",
                                (*i).feedIcon,
                                (*i).feedTitle.remove(re),
                                _db->isCacheExistsByEntryId((*i).id),
                                (*i).fresh,
                                (*i).read,
                                (*i).saved,
                                (*i).publishedAt
                                ));
        ++i;
    }

    // Dummy row as workaround!
    if (list.count()>0)
        appendRow(new EntryItem("last","","","","","","","",false,0,0,0,0));

    return list.count();
}

void EntryModel::setAllAsUnread()
{
    Settings *s = Settings::instance();
    if (s->getSigninType() >= 10) {
        // setAllAsUnread not supported in API
        qWarning() << "Mark all as unread is not supported!";
        return;
    }

    int l = this->rowCount();
    for (int i=0; i<l; ++i) {
        EntryItem* item = static_cast<EntryItem*>(readRow(i));
        item->setRead(0);
    }

    // DB change & Action
    DatabaseManager::Action action;
    int mode = s->getViewMode();
    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        _db->updateEntriesReadFlagByStream(_feedId,0);

        action.type = DatabaseManager::UnSetStreamReadAll;
        action.id1 = _feedId;
        action.date1 = _db->readLastUpdateByStream(_feedId);

        break;
    case 1:
        // View mode: Tabs->Entries
        _db->updateEntriesReadFlagByTab(_feedId,0);

        action.type = DatabaseManager::UnSetTabReadAll;
        action.id1 = _feedId;
        action.date1 = _db->readLastUpdateByTab(_feedId);

        break;
    case 2:
        // View mode: Feeds->Entries
        _db->updateEntriesReadFlagByStream(_feedId,0);

        action.type = DatabaseManager::UnSetStreamReadAll;
        action.id1 = _feedId;
        action.date1 = _db->readLastUpdateByStream(_feedId);

        break;
    case 3:
        // View mode: Entries
        _db->updateEntriesReadFlagByDashboard(s->getDashboardInUse(),0);

        action.type = DatabaseManager::UnSetAllRead;
        action.id1 = s->getDashboardInUse();
        action.date1 = _db->readLastUpdateByDashboard(s->getDashboardInUse());

        break;
    case 4:
        // View mode: Saved
        qWarning() << "Error: This should never happened";
        return;
    case 5:
        // View mode: Slow
        _db->updateEntriesSlowReadFlagByDashboard(s->getDashboardInUse(),0);

        action.type = DatabaseManager::UnSetSlowRead;
        action.id1 = s->getDashboardInUse();
        action.date1 = _db->readLastUpdateByDashboard(s->getDashboardInUse());

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
        _db->updateEntriesReadFlagByStream(_feedId,1);

        action.type = DatabaseManager::SetStreamReadAll;
        action.id1 = _feedId;
        action.date1 = _db->readLastUpdateByStream(_feedId);

        break;
    case 1:
        // View mode: Tabs->Entries
        _db->updateEntriesReadFlagByTab(_feedId,1);

        action.type = DatabaseManager::SetTabReadAll;
        action.id1 = _feedId;
        action.date1 = _db->readLastUpdateByTab(_feedId);

        break;
    case 2:
        // View mode: Feeds->Entries
        _db->updateEntriesReadFlagByStream(_feedId,1);

        action.type = DatabaseManager::SetStreamReadAll;
        action.id1 = _feedId;
        action.date1 = _db->readLastUpdateByStream(_feedId);

        break;
    case 3:
        // View mode: Entries
        _db->updateEntriesReadFlagByDashboard(s->getDashboardInUse(),1);

        action.type = DatabaseManager::SetAllRead;
        action.id1 = s->getDashboardInUse();
        action.date1 = _db->readLastUpdateByDashboard(s->getDashboardInUse());

        break;
    case 4:
        // View mode: Saved
        qWarning() << "Error: This should never happened";
        return;
    case 5:
        // View mode: Slow
        _db->updateEntriesSlowReadFlagByDashboard(s->getDashboardInUse(),1);

        action.type = DatabaseManager::SetSlowRead;
        action.id1 = s->getDashboardInUse();
        action.date1 = _db->readLastUpdateByDashboard(s->getDashboardInUse());

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
        return _db->countEntriesReadByStream(_feedId);
        break;
    case 1:
        // View mode: Tabs->Entries
        return _db->countEntriesReadByTab(_feedId);
        break;
    case 2:
        // View mode: Feeds->Entries
        return _db->countEntriesReadByStream(_feedId);
        break;
    case 3:
        // View mode: Entries
        return _db->countEntriesReadByDashboard(s->getDashboardInUse());
        break;
    case 4:
        // View mode: Saved
        qWarning() << "Error: This should never happened";
        return 0;
    case 5:
        // View mode: Saved
        return _db->countEntriesSlowReadByDashboard(s->getDashboardInUse());
        break;
    }

    return 0;
}

int EntryModel::countUnread()
{
    Settings *s = Settings::instance();
    int mode = s->getViewMode();
    switch (mode) {
    case 0:
        // View mode: Tabs->Feeds->Entries
        return _db->countEntriesUnreadByStream(_feedId);
        break;
    case 1:
        // View mode: Tabs->Entries
        return _db->countEntriesUnreadByTab(_feedId);
        break;
    case 2:
        // View mode: Feeds->Entries
        return _db->countEntriesUnreadByStream(_feedId);
        break;
    case 3:
        // View mode: Entries
        return _db->countEntriesUnreadByDashboard(s->getDashboardInUse());
        break;
    case 4:
        // View mode: Saved
        qWarning() << "Error: This should never happened";
        return 0;
    case 5:
        // View mode: Saved
        return _db->countEntriesSlowUnreadByDashboard(s->getDashboardInUse());
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
        action.id2 = s->db->readStreamIdByEntry(item->id());
        if (newValue==1) {
            action.type = DatabaseManager::SetSaved;
            action.id1 = item->id();
            action.date1 = item->date();
        } else {
            action.type = DatabaseManager::UnSetSaved;
            action.id1 = item->id();
            action.date1 = item->date();
        }
        _db->writeAction(action);
        _db->updateEntriesSavedFlagByEntry(item->id(),newValue.toInt());
    }

    if (fieldName=="read") {
        item->setRead(newValue.toInt());
        DatabaseManager::Action action;
        Settings *s = Settings::instance();
        action.id2 = s->db->readStreamIdByEntry(item->id());
        if (newValue==1) {
            action.type = DatabaseManager::SetRead;
            action.id1 = item->id();
            action.date1 = item->date();
        } else {
            action.type = DatabaseManager::UnSetRead;
            action.id1 = item->id();
            action.date1 = item->date();
        }
        _db->writeAction(action);
        _db->updateEntriesReadFlagByEntry(item->id(),newValue.toInt());
    }

    if (fieldName=="cached") {
        item->setCached(newValue.toInt());
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
                   const QString &feedTitle,
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
    m_feedTitle(feedTitle),
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
    names[FeedTitleRole] = "feedTitle";
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
    case FeedTitleRole:
        return feedTitle();
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
    if(m_readlater!=value) {
        m_readlater = value;
        emit dataChanged();
    }
}

void EntryItem::setRead(int value)
{
    if(m_read!=value) {
        m_read = value;
        emit dataChanged();
    }
}

void EntryItem::setCached(int value)
{
    if(m_cached!=value) {
        m_cached = value;
        emit dataChanged();
    }
}
