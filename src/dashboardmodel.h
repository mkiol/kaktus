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

#ifndef DASHBOARDMODEL_H
#define DASHBOARDMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QModelIndex>

#include "listmodel.h"
#include "databasemanager.h"
#include "tabmodel.h"

class DashboardItem : public ListItem
{
    Q_OBJECT

public:
    enum Roles {
        UidRole = Qt::UserRole+1,
        NameRole,
        TitleRole = Qt::DisplayRole,
        DescriptionRole
    };

public:
    DashboardItem(QObject *parent = 0): ListItem(parent) {}
    explicit DashboardItem(const QString &uid,
                      const QString &name,
                      const QString &title,
                      const QString &description,
                      QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;
    inline QString id() const { return m_uid; }
    inline QString uid() const { return m_uid; }
    inline QString name() const { return m_name; }
    inline QString title() const { return m_title; }
    inline QString description() const { return m_description; }

private:
    QString m_uid;
    QString m_name;
    QString m_title;
    QString m_description;
};

class DashboardModel : public ListModel
{
    Q_OBJECT

public:
    explicit DashboardModel(DatabaseManager* db, QObject *parent = 0);
    Q_INVOKABLE void init();
    Q_INVOKABLE int count();
    Q_INVOKABLE QObject* get(int i);

private:
    DatabaseManager* _db;

    void createItems();
    void sort();
};

#endif // DASHBOARDMODEL_H
