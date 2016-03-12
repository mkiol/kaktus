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

#include "dashboardmodel.h"

DashboardModel::DashboardModel(DatabaseManager *db, QObject *parent) :
    ListModel(new DashboardItem, parent)
{
    _db = db;
}

void DashboardModel::init()
{
    if(rowCount()>0) removeRows(0,rowCount());
    createItems();
}

void DashboardModel::createItems()
{
    QList<DatabaseManager::Dashboard> list = _db->readDashboards();
    QList<DatabaseManager::Dashboard>::iterator i = list.begin();
    while( i != list.end() ) {
        //qDebug() << (*i).id << (*i).title;
        appendRow(new DashboardItem((*i).id,
                                    (*i).name,
                                    (*i).title,
                                    (*i).description
                                    ));
        ++i;
    }

}

void DashboardModel::sort()
{
}

int DashboardModel::count()
{
    return this->rowCount();
}

QObject* DashboardModel::get(int i)
{
    return (QObject*) this->readRow(i);
}

// ----------------------------------------------------------------

DashboardItem::DashboardItem(const QString &uid,
                   const QString &name,
                   const QString &title,
                   const QString &description,
                   QObject *parent) :
    ListItem(parent),
    m_uid(uid),
    m_name(name),
    m_title(title),
    m_description(description)
{}

QHash<int, QByteArray> DashboardItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[UidRole] = "uid";
    names[NameRole] = "name";
    names[TitleRole] = "title";
    names[DescriptionRole] = "description";
    return names;
}

QVariant DashboardItem::data(int role) const
{
    switch(role) {
    case UidRole:
        return uid();
    case NameRole:
        return name();
    case TitleRole:
        return title();
    case DescriptionRole:
        return description();
    default:
        return QVariant();
    }
}

