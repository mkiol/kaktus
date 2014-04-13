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

#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QModelIndex>

#include "listmodel.h"
#include "databasemanager.h"

class FeedItem : public ListItem
{
    Q_OBJECT

public:
    enum Roles {
        UidRole = Qt::UserRole+1,
        TitleRole = Qt::DisplayRole,
        ContentRole,
        LinkRole,
        UrlRole,
        IconRole,
        StreamIdRole,
        UnreadRole,
        ReadRole,
        ReadlaterRole
    };

public:
    FeedItem(QObject *parent = 0): ListItem(parent) {}
    explicit FeedItem(const QString &uid,
                      const QString &title,
                      const QString &content,
                      const QString &link,
                      const QString &url,
                      const QString &icon,
                      const QString &streamId,
                      int unread,
                      int read,
                      int readlater,
                      QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;
    inline QString id() const { return m_uid; }
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
    int m_unread;
    int m_read;
    int m_readlater;
};

class FeedModel : public ListModel
{
    Q_OBJECT

public:
    explicit FeedModel(DatabaseManager* db, QObject *parent = 0);
    void init(const QString &tabId);
    void init();
    //int count();
    //QObject* get(int i);
    void setData(int row, const QString &fieldName, QVariant newValue);

    Q_INVOKABLE void decrementUnread(int row);
    Q_INVOKABLE void incrementUnread(int row);
    Q_INVOKABLE void markAllAsUnread(int row);
    Q_INVOKABLE void markAllAsRead(int row);

private:
    DatabaseManager* _db;
    QString _tabId;

    void createItems(const QString &dashboardId);
};

#endif // FEEDMODEL_H
