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

#ifndef TABMODEL_H
#define TABMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QModelIndex>

#include "listmodel.h"
#include "databasemanager.h"

class TabItem : public ListItem
{
    Q_OBJECT

public:
    enum Roles {
        UidRole = Qt::UserRole+1,
        TitleRole = Qt::DisplayRole,
        IconRole,
        UnreadRole,
        ReadRole,
        ReadlaterRole
    };

    TabItem(QObject *parent = 0): ListItem(parent) {}
    explicit TabItem(const QString &uid,
                     const QString &title,
                     const QString &icon,
                     int unread,
                     int read,
                     int readlater,
                     QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;
    inline QString id() const { return m_uid; }
    inline QString uid() const { return m_uid; }
    inline QString title() const { return m_title; }
    inline QString icon() const { return m_icon; }
    inline int unread() const { return m_unread; }
    inline int read() const { return m_read; }
    inline int readlater() const { return m_readlater; }

    void setReadlater(int value);
    void setUnread(int value);
    void setRead(int value);

private:
    QString m_uid;
    QString m_title;
    QString m_icon;
    int m_unread;
    int m_read;
    int m_readlater;
};

class TabModel : public ListModel
{
    Q_OBJECT

public:
    explicit TabModel(DatabaseManager* db, QObject *parent = 0);
    void init(const QString &dashboardId);
    void init();
    //int count();
    //QObject* get(int i);
    Q_INVOKABLE void updateFlags();

private:
    DatabaseManager* _db;
    QString _dashboardId;

    void createItems(const QString &dashboardId);
    void sort();
};

#endif // TABMODEL_H
