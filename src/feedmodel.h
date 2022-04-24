/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QAbstractListModel>
#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QStringList>

#include "listmodel.h"

class FeedItem : public ListItem {
    Q_OBJECT

   public:
    enum Roles {
        UidRole = Qt::UserRole + 1,
        TitleRole = Qt::DisplayRole,
        ContentRole,
        LinkRole,
        UrlRole,
        IconRole,
        StreamIdRole,
        UnreadRole,
        ReadRole,
        ReadlaterRole,
        FreshRole
    };

   public:
    FeedItem(QObject *parent = nullptr) : ListItem(parent) {}
    explicit FeedItem(const QString &uid, const QString &title,
                      const QString &content, const QString &link,
                      const QString &url, const QString &icon,
                      const QString &streamId, int unread, int read,
                      int readlater, int fresh, QObject *parent = nullptr);
    QVariant data(int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    inline QString id() const override { return m_uid; }
    inline QString uid() const { return m_uid; }
    inline QString title() const { return m_title; }
    inline QString content() const { return m_content; }
    inline QString link() const { return m_link; }
    inline QString url() const { return m_url; }
    inline QString icon() const { return m_icon; }
    inline QString streamId() const { return m_streamid; }
    inline int unread() const { return m_unread; }
    inline int read() const { return m_read; }
    inline int readlater() const { return m_readlater; }
    inline int fresh() const { return m_fresh; }

    void setReadlater(int value);
    void setUnread(int value);
    void setRead(int value);

   private:
    QString m_uid;
    QString m_title;
    QString m_content;
    QString m_link;
    QString m_url;
    QString m_icon;
    QString m_streamid;
    int m_unread = 0;
    int m_read = 0;
    int m_readlater = 0;
    int m_fresh = 0;
};

class FeedModel : public ListModel {
    Q_OBJECT

   public:
    explicit FeedModel(QObject *parent = nullptr);
    void init(const QString &tabId);
    void init();

    Q_INVOKABLE void markAsUnread(int row);
    Q_INVOKABLE void markAsRead(int row);

    Q_INVOKABLE void setAllAsUnread();
    Q_INVOKABLE void setAllAsRead();

    Q_INVOKABLE void updateFlags();

    Q_INVOKABLE int countRead() const;
    Q_INVOKABLE int countUnread() const;
    Q_INVOKABLE int count() const;

   private:
    QString _tabId;

    void createItems(const QString &dashboardId);
};

#endif  // FEEDMODEL_H
