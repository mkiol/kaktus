/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "feedmodel.h"

#include <QRegExp>

#include "databasemanager.h"

FeedModel::FeedModel(QObject *parent) : ListModel{new FeedItem, parent} {}

void FeedModel::init(const QString &tabId) {
    _tabId = tabId;
    init();
}

void FeedModel::init() {
    if (rowCount() > 0) removeRows(0, rowCount());
    createItems(_tabId);
}

void FeedModel::createItems(const QString &tabId) {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    QList<DatabaseManager::Stream> list;

    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
        case Settings::ViewMode::TabsEntries:
            list = db->readStreamsByTab(tabId);
            break;
        case Settings::ViewMode::FeedsEntries:
            list = db->readStreamsByDashboard(s->getDashboardInUse());
            break;
        default:
            qWarning() << "invalid mode";
            return;
    }

    QRegExp re{"<[^>]*>"};
    QList<DatabaseManager::Stream>::iterator i = list.begin();
    while (i != list.end()) {
        appendRow(new FeedItem{i->id, i->title.remove(re), i->content, i->link,
                               i->query, i->icon, 0,
                               db->countEntriesUnreadByStream(i->id),
                               db->countEntriesReadByStream(i->id), i->saved,
                               db->countEntriesFreshByStream(i->id)});
        ++i;
    }

    // Dummy row as workaround!
    if (!list.isEmpty())
        appendRow(new FeedItem{"last", "", "", "", "", "", 0, 0, 0, 0, 0});
}

void FeedModel::markAsUnread(int row) {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    if (s->getSigninType() >= 10) {
        // markAsUnread not supported in API
        qWarning() << "Mark feed as unread is not supported";
        return;
    }

    FeedItem *item = dynamic_cast<FeedItem *>(readRow(row));
    db->updateEntriesReadFlagByStream(item->id(), 0);
    item->setRead(0);
    item->setUnread(db->countEntriesUnreadByStream(item->id()));

    DatabaseManager::Action action;
    action.type = DatabaseManager::UnSetStreamReadAll;
    action.id1 = item->id();
    action.date1 = db->readLastUpdateByStream(item->id());
    db->writeAction(action);
}

void FeedModel::markAsRead(int row) {
    auto *db = DatabaseManager::instance();

    FeedItem *item = dynamic_cast<FeedItem *>(readRow(row));
    db->updateEntriesReadFlagByStream(item->id(), 1);
    item->setUnread(0);
    item->setRead(db->countEntriesReadByStream(item->id()));

    DatabaseManager::Action action;
    action.type = DatabaseManager::SetStreamReadAll;
    action.id1 = item->id();
    action.date1 = db->readLastUpdateByStream(item->id());
    db->writeAction(action);
}

int FeedModel::countRead() const {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            return db->countEntriesReadByTab(_tabId);
        case Settings::ViewMode::FeedsEntries:
            return db->countEntriesReadByDashboard(s->getDashboardInUse());
        default:
            qWarning() << "invalid mode";
            return 0;
    }
}

int FeedModel::countUnread() const {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            return db->countEntriesUnreadByTab(_tabId);
        case Settings::ViewMode::FeedsEntries:
            return db->countEntriesUnreadByDashboard(s->getDashboardInUse());
        default:
            qWarning() << "invalid mode";
            return 0;
    }
}

void FeedModel::setAllAsUnread() {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    if (s->getSigninType() >= 10) {
        // setAllAsUnread not supported in API
        qWarning() << "Mark tab as unread is not supported";
        return;
    }

    DatabaseManager::Action action;
    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            db->updateEntriesReadFlagByTab(_tabId, 0);
            action.type = DatabaseManager::UnSetTabReadAll;
            action.id1 = _tabId;
            action.date1 = db->readLastUpdateByTab(_tabId);
            break;
        case Settings::ViewMode::FeedsEntries:
            db->updateEntriesReadFlagByDashboard(s->getDashboardInUse(), 0);
            action.type = DatabaseManager::UnSetAllRead;
            action.id1 = s->getDashboardInUse();
            action.date1 = db->readLastUpdateByTab(_tabId);
            break;
        default:
            qWarning() << "invalid mode";
            return;
    }

    updateFlags();
    db->writeAction(action);
}

void FeedModel::setAllAsRead() {
    auto *s = Settings::instance();
    auto *db = DatabaseManager::instance();

    DatabaseManager::Action action;

    auto mode = s->getViewMode();
    switch (mode) {
        case Settings::ViewMode::TabsFeedsEntries:
            db->updateEntriesReadFlagByTab(_tabId, 1);
            action.type = DatabaseManager::SetTabReadAll;
            action.id1 = _tabId;
            action.date1 = db->readLastUpdateByTab(_tabId);
            break;
        case Settings::ViewMode::FeedsEntries:
            db->updateEntriesReadFlagByDashboard(s->getDashboardInUse(), 1);
            action.type = DatabaseManager::SetAllRead;
            action.id1 = s->getDashboardInUse();
            action.date1 =
                db->readLastUpdateByDashboard(s->getDashboardInUse());
            break;
        default:
            qWarning() << "invalid mode";
            return;
    }

    updateFlags();
    db->writeAction(action);
}

void FeedModel::updateFlags() {
    auto *db = DatabaseManager::instance();

    int l = this->rowCount();
    for (int i = 0; i < l; ++i) {
        FeedItem *item = dynamic_cast<FeedItem *>(readRow(i));
        item->setUnread(db->countEntriesUnreadByStream(item->uid()));
        item->setRead(db->countEntriesReadByStream(item->uid()));
    }
}

int FeedModel::count() const { return this->rowCount(); }

// ----------------------------------------------------------------

FeedItem::FeedItem(const QString &uid, const QString &title,
                   const QString &content, const QString &link,
                   const QString &url, const QString &icon,
                   const QString &streamId, int unread, int read, int readlater,
                   int fresh, QObject *parent)
    : ListItem(parent),
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
      m_fresh(fresh) {}

QHash<int, QByteArray> FeedItem::roleNames() const {
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

QVariant FeedItem::data(int role) const {
    switch (role) {
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
            return {};
    }
}

void FeedItem::setReadlater(int value) {
    if (m_readlater != value) {
        m_readlater = value;
        emit dataChanged();
    }
}

void FeedItem::setUnread(int value) {
    if (m_unread != value) {
        m_unread = value;
        emit dataChanged();
    }
}

void FeedItem::setRead(int value) {
    if (m_read != value) {
        m_read = value;
        emit dataChanged();
    }
}
