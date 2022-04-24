/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "entrymodel.h"

#include <QChar>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QModelIndex>
#include <QRegExp>
#include <QUrl>
#include <QtGui/QTextDocument>

#include "databasemanager.h"
#include "utils.h"

EntryModelIniter::EntryModelIniter(QObject *parent) : QThread{parent} {}

void EntryModelIniter::init(EntryModel *model) {
    m_model = model;
    m_feedId.clear();
    start(QThread::LowestPriority);
}

void EntryModelIniter::init(EntryModel *model, const QString &feedId) {
    m_model = model;
    m_feedId = feedId;
    start(QThread::LowestPriority);
}

void EntryModelIniter::run() {
    if (m_feedId.isEmpty())
        m_model->init();
    else
        m_model->init(m_feedId);
}

EntryModel::EntryModel(QObject *parent)
    : ListModel(new EntryItem, parent), m_initer(parent) {
    reInit = false;
    connect(Settings::instance(), SIGNAL(filterChanged()), this,
            SLOT(initInThread()));
    connect(&m_initer, SIGNAL(finished()), this, SLOT(initFinished()));
    connect(Settings::instance(), SIGNAL(showOldestFirstChanged()), this,
            SLOT(init()));
}

void EntryModel::init(const QString &feedId) {
    if (rowCount() > 0) removeRows(0, rowCount());
    m_feedId = feedId;
    createItems(0, Settings::instance()->getOffsetLimit());
}

void EntryModel::init() {
    reInit = false;
    if (rowCount() > 0) removeRows(0, rowCount());
    createItems(0, Settings::instance()->getOffsetLimit());
    emit ready();
}

void EntryModel::initInThread(const QString &feedId) {
    m_initer.init(this, feedId);
}

void EntryModel::initInThread() { m_initer.init(this); }

void EntryModel::initFinished() {
    this->beginResetModel();
    this->endResetModel();
}

int EntryModel::getDateRowId(int date) {
    auto qdate = QDateTime::fromTime_t(date);
    int days = qdate.daysTo(QDateTime::currentDateTimeUtc());
    if (days == 0) return 1;
    if (days == 1) return 2;
    if (Utils::isSameWeek(qdate.date(), QDate::currentDate())) return 3;
    int months = Utils::monthsTo(qdate.date(), QDate::currentDate());
    if (months == 0) return 4;
    if (months == 1) return 5;
    if (Utils::yearsTo(qdate.date(), QDate::currentDate()) == 0) return 6;
    return 10;
}

int EntryModel::createItems(int offset, int limit) {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    QList<DatabaseManager::Entry> list;

    bool ascOrder = s->getShowOldestFirst();

    // Counting 'last' & 'daterow' rows
    if (offset > 0) {
        int dummyRowsCount = 0;
        int l = this->rowCount();
        // qDebug() << "this->rowCount():" << l;
        for (int i = 0; i < l; ++i) {
            EntryItem *item = dynamic_cast<EntryItem *>(readRow(i));
            // qDebug() << item->id();
            if (item->id() == "last" || item->id() == "daterow") {
                ++dummyRowsCount;
            }
        }
        // qDebug() << "dummyRowsCount:" << dummyRowsCount << "orig offset:" <<
        // offset;
        if (offset > dummyRowsCount) offset = offset - dummyRowsCount;
    }

    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            if (s->getFilter() == 2)
                list = db->readEntriesUnreadByStream(m_feedId, offset, limit,
                                                     ascOrder);
            else if (s->getFilter() == 1)
                list = db->readEntriesUnreadAndSavedByStream(m_feedId, offset,
                                                             limit, ascOrder);
            else
                list =
                    db->readEntriesByStream(m_feedId, offset, limit, ascOrder);
            break;
        case Settings::ViewMode::TabsEntries:
            if (s->getFilter() == 2)
                list = db->readEntriesUnreadByTab(m_feedId, offset, limit,
                                                  ascOrder);
            else if (s->getFilter() == 1)
                list = db->readEntriesUnreadAndSavedByTab(m_feedId, offset,
                                                          limit, ascOrder);
            else
                list = db->readEntriesByTab(m_feedId, offset, limit, ascOrder);
            break;
        case Settings::ViewMode::FeedsEntries:
            if (s->getFilter() == 2)
                list = db->readEntriesUnreadByStream(m_feedId, offset, limit,
                                                     ascOrder);
            else if (s->getFilter() == 1)
                list = db->readEntriesUnreadAndSavedByStream(m_feedId, offset,
                                                             limit, ascOrder);
            else
                list =
                    db->readEntriesByStream(m_feedId, offset, limit, ascOrder);
            break;
        case Settings::ViewMode::AllEntries:
            if (s->getFilter() == 2)
                list = db->readEntriesUnreadByDashboard(
                    s->getDashboardInUse(), offset, limit, ascOrder);
            else if (s->getFilter() == 1)
                list = db->readEntriesUnreadAndSavedByDashboard(
                    s->getDashboardInUse(), offset, limit, ascOrder);
            else
                list = db->readEntriesByDashboard(s->getDashboardInUse(),
                                                  offset, limit, ascOrder);
            break;
        case Settings::ViewMode::SavedEntries:
            list = db->readEntriesSavedByDashboard(s->getDashboardInUse(),
                                                   offset, limit, ascOrder);
            break;
        case Settings::ViewMode::SlowEntries:
            if (s->getFilter() == 2)
                list = db->readEntriesSlowUnreadByDashboard(
                    s->getDashboardInUse(), offset, limit, ascOrder);
            else if (s->getFilter() == 1)
                list = db->readEntriesSlowUnreadAndSavedByDashboard(
                    s->getDashboardInUse(), offset, limit, ascOrder);
            else
                list = db->readEntriesSlowByDashboard(s->getDashboardInUse(),
                                                      offset, limit, ascOrder);
            break;
        case Settings::ViewMode::LikedEntries:
            list = db->readEntriesLikedByDashboard(s->getDashboardInUse(),
                                                   offset, limit, ascOrder);
            break;
        case Settings::ViewMode::BroadcastedEntries:
            list = db->readEntriesBroadcastByDashboard(s->getDashboardInUse(),
                                                       offset, limit, ascOrder);
            break;
    }

    // Remove dummy row
    if (list.count() > 0) {
        int l = rowCount();
        if (l > 0) {
            EntryItem *item = qobject_cast<EntryItem *>(readRow(l - 1));
            if (item->id() == "last") removeRow(l - 1);
        }
    }

    QList<DatabaseManager::Entry>::iterator i = list.begin();

    int prevDateRow = 0;
    if (rowCount() > 0) {
        EntryItem *item = dynamic_cast<EntryItem *>(readRow(rowCount() - 1));
        prevDateRow = getDateRowId(item->date());
        // qDebug() << "prevDateRow UID:" << item->uid();
    }

    QRegExp re("<[^>]*>");
    while (i != list.end()) {
        // Removing html tags!
        QTextDocument doc;
        doc.setHtml((*i).content);

        QString content0 =
            doc.toPlainText()
                .replace(QChar::ObjectReplacementCharacter, QChar::Space)
                .trimmed();
        QString content = content0.simplified();
        if (content.length() > 1000) {
            content = content.left(997) + "...";
        } else if (content.length() < 15) {
            content.clear();
        }

        doc.setHtml((*i).title);
        QString title =
            doc.toPlainText()
                .replace(QChar::ObjectReplacementCharacter, QChar::Space)
                .simplified();
        if (title.length() > 200) title = title.left(197) + QString("...");

        // qDebug() << title;

        /*QRegExp rx("(\\S*)\\s*\((\\S*)\)", Qt::CaseInsensitive);
        if (rx.indexIn((*i).author)!=-1) {
            qDebug() << "(*i).author:" << (*i).author << "cap:" <<
        rx.cap(1).toUtf8();
            //(*i).author = rx.cap(1).toUtf8();
        }*/

        // Detecting invalid images
        bool imageOk = true;
        QUrl imageUrl((*i).image);
        // qDebug() << imageUrl.path();
        if (imageUrl.path() == "/assets/images/transparent.png")
            imageOk = false;
        if (imageUrl.host() == "rc.feedsportal.com") imageOk = false;

        // Adding date row
        int dateRow = getDateRowId((*i).publishedAt);
        if ((!ascOrder && dateRow > prevDateRow) ||
            (ascOrder && dateRow < prevDateRow) || prevDateRow == 0) {
            switch (dateRow) {
                case 1:
                    appendRow(new EntryItem("daterow", tr("Today"), "", "", "",
                                            "", "", "", "", "", "", "", false,
                                            false, false, 0, 0, 0, 0));
                    break;
                case 2:
                    appendRow(new EntryItem("daterow", tr("Yesterday"), "", "",
                                            "", "", "", "", "", "", "", "",
                                            false, false, false, 0, 0, 0, 0));
                    break;
                case 3:
                    appendRow(new EntryItem("daterow", tr("Current week"), "",
                                            "", "", "", "", "", "", "", "", "",
                                            false, false, false, 0, 0, 0, 0));
                    break;
                case 4:
                    appendRow(new EntryItem("daterow", tr("Current month"), "",
                                            "", "", "", "", "", "", "", "", "",
                                            false, false, false, 0, 0, 0, 0));
                    break;
                case 5:
                    appendRow(new EntryItem("daterow", tr("Previous month"), "",
                                            "", "", "", "", "", "", "", "", "",
                                            false, false, false, 0, 0, 0, 0));
                    break;
                case 6:
                    appendRow(new EntryItem("daterow", tr("Current year"), "",
                                            "", "", "", "", "", "", "", "", "",
                                            false, false, false, 0, 0, 0, 0));
                    break;
                default:
                    appendRow(new EntryItem("daterow",
                                            tr("Previous year & older"), "", "",
                                            "", "", "", "", "", "", "", "",
                                            false, false, false, 0, 0, 0, 0));
                    break;
            }
        }
        prevDateRow = dateRow;
        appendRow(new EntryItem{
            i->id, title.remove(re), i->author, content, content0, i->content,
            i->link, imageOk ? i->image : "", i->feedId, i->feedIcon,
            i->feedTitle.remove(re), i->annotations,
            db->isCacheExistsByEntryId(i->id), i->broadcast == 1, i->liked == 1,
            i->fresh == 1, i->read, i->saved, i->publishedAt});
        ++i;
    }

    // Dummy row as workaround!
    if (!list.isEmpty())
        appendRow(new EntryItem("last", "", "", "", "", "", "", "", "", "", "",
                                "", false, false, false, 0, 0, 0, 0));

    return list.count();
}

void EntryModel::setAllAsUnread() {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    if (s->getSigninType() >= 10) {
        // setAllAsUnread not supported in API
        qWarning() << "Mark all as unread is not supported";
        return;
    }

    int l = this->rowCount();
    for (int i = 0; i < l; ++i) {
        EntryItem *item = qobject_cast<EntryItem *>(readRow(i));
        item->setRead(0);
    }

    // DB change & Action
    DatabaseManager::Action action;
    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            db->updateEntriesReadFlagByStream(m_feedId, 0);

            action.type = DatabaseManager::UnSetStreamReadAll;
            action.id1 = m_feedId;
            action.date1 = db->readLastUpdateByStream(m_feedId);

            break;
        case Settings::ViewMode::TabsEntries:
            db->updateEntriesReadFlagByTab(m_feedId, 0);

            action.type = DatabaseManager::UnSetTabReadAll;
            action.id1 = m_feedId;
            action.date1 = db->readLastUpdateByTab(m_feedId);

            break;
        case Settings::ViewMode::FeedsEntries:
            db->updateEntriesReadFlagByStream(m_feedId, 0);

            action.type = DatabaseManager::UnSetStreamReadAll;
            action.id1 = m_feedId;
            action.date1 = db->readLastUpdateByStream(m_feedId);

            break;
        case Settings::ViewMode::AllEntries:
            db->updateEntriesReadFlagByDashboard(s->getDashboardInUse(), 0);

            action.type = DatabaseManager::UnSetAllRead;
            action.id1 = s->getDashboardInUse();
            action.date1 =
                db->readLastUpdateByDashboard(s->getDashboardInUse());

            break;
        case Settings::ViewMode::SavedEntries:
            qWarning() << "Error: This should never happened";
            return;
        case Settings::ViewMode::SlowEntries:
            db->updateEntriesSlowReadFlagByDashboard(s->getDashboardInUse(), 0);

            action.type = DatabaseManager::UnSetSlowRead;
            action.id1 = s->getDashboardInUse();
            action.date1 =
                db->readLastUpdateByDashboard(s->getDashboardInUse());

            break;
        default:
            qWarning() << "invalid mode";
            return;
    }

    db->writeAction(action);
}

void EntryModel::setAllAsRead() {
    int l = this->rowCount();
    for (int i = 0; i < l; ++i) {
        EntryItem *item = static_cast<EntryItem *>(readRow(i));
        item->setRead(1);
    }

    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    DatabaseManager::Action action;
    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            db->updateEntriesReadFlagByStream(m_feedId, 1);

            action.type = DatabaseManager::SetStreamReadAll;
            action.id1 = m_feedId;
            action.date1 = db->readLastUpdateByStream(m_feedId);

            break;
        case Settings::ViewMode::TabsEntries:
            db->updateEntriesReadFlagByTab(m_feedId, 1);

            action.type = DatabaseManager::SetTabReadAll;
            action.id1 = m_feedId;
            action.date1 = db->readLastUpdateByTab(m_feedId);

            break;
        case Settings::ViewMode::FeedsEntries:
            db->updateEntriesReadFlagByStream(m_feedId, 1);

            action.type = DatabaseManager::SetStreamReadAll;
            action.id1 = m_feedId;
            action.date1 = db->readLastUpdateByStream(m_feedId);

            break;
        case Settings::ViewMode::AllEntries:
            db->updateEntriesReadFlagByDashboard(s->getDashboardInUse(), 1);

            action.type = DatabaseManager::SetAllRead;
            action.id1 = s->getDashboardInUse();
            action.date1 =
                db->readLastUpdateByDashboard(s->getDashboardInUse());

            break;
        case Settings::ViewMode::SlowEntries:
            db->updateEntriesSlowReadFlagByDashboard(s->getDashboardInUse(), 1);

            action.type = DatabaseManager::SetSlowRead;
            action.id1 = s->getDashboardInUse();
            action.date1 =
                db->readLastUpdateByDashboard(s->getDashboardInUse());

            break;
        default:
            qWarning() << "invalid mode";
            return;
    }

    db->writeAction(action);
}

void EntryModel::setAboveAsRead(int index) {
    auto *db = DatabaseManager::instance();

    int a = index <= idsOnActionLimit ? 0 : index - idsOnActionLimit;

    QString itemIds;
    QString feedIds;
    QString dates;

    bool ok = false;
    for (; a <= index; ++a) {
        EntryItem *item = dynamic_cast<EntryItem *>(readRow(a));
        QString id = item->id();
        if (id != "daterow" && id != "last" && item->read() == 0) {
            item->setRead(1);
            db->updateEntriesReadFlagByEntry(id, 1);
            itemIds.append(QString("%1&").arg(id));
            feedIds.append(QString("%1&").arg(item->feedId()));
            dates.append(QString("%1&").arg(item->date()));
            ok = true;
        }
    }

    if (ok) {
        itemIds.remove(itemIds.length() - 1, 1);
        feedIds.remove(feedIds.length() - 1, 1);
        dates.remove(dates.length() - 1, 1);
        DatabaseManager::Action action;
        action.type = DatabaseManager::SetListRead;
        action.id1 = itemIds;
        action.id2 = feedIds;
        action.id3 = dates;
        db->writeAction(action);
    }

    if (index > idsOnActionLimit) setAboveAsRead(index - idsOnActionLimit - 1);
}

int EntryModel::countRead() {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            return db->countEntriesReadByStream(m_feedId);
        case Settings::ViewMode::TabsEntries:
            return db->countEntriesReadByTab(m_feedId);
        case Settings::ViewMode::FeedsEntries:
            return db->countEntriesReadByStream(m_feedId);
        case Settings::ViewMode::AllEntries:
            return db->countEntriesReadByDashboard(s->getDashboardInUse());
        case Settings::ViewMode::SlowEntries:
            return db->countEntriesSlowReadByDashboard(s->getDashboardInUse());
        default:
            qWarning() << "invalid mode";
            return 0;
    }
}

int EntryModel::countUnread() {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            return db->countEntriesUnreadByStream(m_feedId);
        case Settings::ViewMode::TabsEntries:
            return db->countEntriesUnreadByTab(m_feedId);
        case Settings::ViewMode::FeedsEntries:
            return db->countEntriesUnreadByStream(m_feedId);
        case Settings::ViewMode::AllEntries:
            return db->countEntriesUnreadByDashboard(s->getDashboardInUse());
        case Settings::ViewMode::SlowEntries:
            return db->countEntriesSlowUnreadByDashboard(
                s->getDashboardInUse());
        default:
            qWarning() << "invalid mode";
            return 0;
    }
}

int EntryModel::count() const { return this->rowCount(); }

void EntryModel::setData(int row, const QString &fieldName,
                         const QVariant &newValue, const QVariant &newValue2) {
    auto db = DatabaseManager::instance();
    auto item = qobject_cast<EntryItem *>(readRow(row));

    if (fieldName == "readlater") {
        item->setReadlater(newValue.toInt());
        DatabaseManager::Action action;
        action.id2 = db->readStreamIdByEntry(item->id());
        if (newValue == 1) {
            action.type = DatabaseManager::SetSaved;
            action.id1 = item->id();
            action.date1 = item->date();
            action.id3 = QString::number(action.date1);
        } else {
            action.type = DatabaseManager::UnSetSaved;
            action.id1 = item->id();
            action.date1 = item->date();
            action.id3 = QString::number(action.date1);
        }
        db->writeAction(action);
        db->updateEntriesSavedFlagByEntry(item->id(), newValue.toInt());
    } else if (fieldName == "read") {
        item->setRead(newValue.toInt());
        DatabaseManager::Action action;
        action.id2 = db->readStreamIdByEntry(item->id());
        if (newValue == 1) {
            action.type = DatabaseManager::SetRead;
            action.id1 = item->id();
            action.date1 = item->date();
            action.id3 = QString::number(action.date1);
        } else {
            action.type = DatabaseManager::UnSetRead;
            action.id1 = item->id();
            action.date1 = item->date();
            action.id3 = QString::number(action.date1);
        }
        db->writeAction(action);
        db->updateEntriesReadFlagByEntry(item->id(), newValue.toInt());
    } else if (fieldName == "liked") {
        item->setLiked(newValue.toBool() ? 1 : 0);
        DatabaseManager::Action action;
        action.id2 = db->readStreamIdByEntry(item->id());
        if (newValue.toBool()) {
            action.type = DatabaseManager::SetLiked;
            action.id1 = item->id();
            action.date1 = item->date();
            action.id3 = QString::number(action.date1);
        } else {
            action.type = DatabaseManager::UnSetLiked;
            action.id1 = item->id();
            action.date1 = item->date();
            action.id3 = QString::number(action.date1);
        }
        db->writeAction(action);
        db->updateEntriesLikedFlagByEntry(item->id(),
                                          newValue.toBool() ? 1 : 0);
    } else if (fieldName == "broadcast") {
        if (auto type = Settings::instance()->getSigninType();
            type < 10 || type >= 20) {
            qWarning() << "broadcast is not supported";
            return;
        }
        item->setBroadcast(newValue.toBool(), newValue2.toString());
        DatabaseManager::Action action;
        if (newValue.toBool()) {
            action.type = DatabaseManager::SetBroadcast;
            action.id1 = item->id();
            action.date1 = item->date();
            action.text = newValue2.toString();
            action.id3 = QString::number(action.date1);
        } else {
            action.type = DatabaseManager::UnSetBroadcast;
            action.id1 = item->id();
            action.date1 = item->date();
            action.text = newValue2.toString();
            action.id3 = QString::number(action.date1);
        }
        db->writeAction(action);
        db->updateEntriesBroadcastFlagByEntry(item->id(), newValue.toInt(),
                                              newValue2.toString());
    } else if (fieldName == "cached") {
        item->setCached(newValue.toInt());
    }
}

EntryItem::EntryItem(const QString &uid, const QString &title,
                     const QString &author, const QString &content,
                     const QString &contentall, const QString &contentraw,
                     const QString &link, const QString &image,
                     const QString &feedId, const QString &feedIcon,
                     const QString &feedTitle, const QString &annotations,
                     bool cached, bool broadcast, bool liked, bool fresh,
                     int read, int readlater, int date, QObject *parent)
    : ListItem(parent),
      m_uid(uid),
      m_title(title),
      m_author(author),
      m_content(content),
      m_contentall(contentall),
      m_contentraw(contentraw),
      m_link(link),
      m_image(image),
      m_feedId(feedId),
      m_feedIcon(feedIcon),
      m_feedTitle(feedTitle),
      m_annotations(annotations),
      m_cached(cached),
      m_broadcast(broadcast),
      m_liked(liked),
      m_fresh(fresh),
      m_read(read),
      m_readlater(readlater),
      m_date(date) {}

QHash<int, QByteArray> EntryItem::roleNames() const {
    QHash<int, QByteArray> names;
    names[UidRole] = "uid";
    names[TitleRole] = "title";
    names[AuthorRole] = "author";
    names[ContentRole] = "content";
    names[ContentAllRole] = "contentall";
    names[ContentRawRole] = "contentraw";
    names[LinkRole] = "link";
    names[ImageRole] = "image";
    names[FeedIdRole] = "feedId";
    names[FeedIconRole] = "feedIcon";
    names[FeedTitleRole] = "feedTitle";
    names[AnnotationsRole] = "annotations";
    names[CachedRole] = "cached";
    names[BroadcastRole] = "broadcast";
    names[LikedRole] = "liked";
    names[FreshRole] = "fresh";
    names[ReadRole] = "read";
    names[ReadLaterRole] = "readlater";
    names[DateRole] = "date";
    return names;
}

QVariant EntryItem::data(int role) const {
    switch (role) {
        case UidRole:
            return uid();
        case TitleRole:
            return title();
        case AuthorRole:
            return author();
        case ContentRole:
            return content();
        case ContentAllRole:
            return contentall();
        case ContentRawRole:
            return contentraw();
        case LinkRole:
            return link();
        case ImageRole:
            return image();
        case FeedIdRole:
            return feedId();
        case FeedIconRole:
            return feedIcon();
        case FeedTitleRole:
            return feedTitle();
        case AnnotationsRole:
            return annotations();
        case CachedRole:
            return cached();
        case BroadcastRole:
            return broadcast();
        case LikedRole:
            return liked();
        case FreshRole:
            return fresh();
        case ReadRole:
            return read();
        case ReadLaterRole:
            return readlater();
        case DateRole:
            return date();
        default:
            return {};
    }
}

void EntryItem::setReadlater(int value) {
    if (m_readlater != value) {
        m_readlater = value;
        emit dataChanged();
    }
}

void EntryItem::setRead(int value) {
    if (m_read != value) {
        m_read = value;
        emit dataChanged();
    }
}

void EntryItem::setLiked(int value) {
    if (m_liked != value) {
        m_liked = value;
        emit dataChanged();
    }
}

void EntryItem::setBroadcast(bool value, const QString &annotations) {
    if (m_broadcast != value) {
        m_broadcast = value;
        m_annotations = annotations;
        emit dataChanged();
    }
}

void EntryItem::setCached(int value) {
    if (m_cached != value) {
        m_cached = value;
        emit dataChanged();
    }
}
