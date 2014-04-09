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

#include "tabmodel.h"

TabModel::TabModel(DatabaseManager *db, QObject *parent) :
    ListModel(new TabItem, parent)
{
    _db = db;
    _dashboardId = "";
}

void TabModel::init(const QString &dashboardId)
{
    _dashboardId = dashboardId;
    init();
}

void TabModel::init()
{
    if(rowCount()>0) removeRows(0,rowCount());
    createItems(_dashboardId);
}

void TabModel::createItems(const QString &dashboardId)
{
    // Readlater extra Tab, id="readlater"
    appendRow(new TabItem("readlater",tr("Saved"),""));

    QList<DatabaseManager::Tab> list = _db->readTabs(dashboardId);
    QList<DatabaseManager::Tab>::iterator i = list.begin();
    while( i != list.end() ) {
        //qDebug() << "tab: " << (*i).id << (*i).title;
        appendRow(new TabItem((*i).id,
                              (*i).title,
                              (*i).icon
                             ));
        ++i;
    }
}

void TabModel::sort()
{
}

int TabModel::count()
{
    return this->rowCount();
}

QObject* TabModel::get(int i)
{
    return (QObject*) this->readRow(i);
}

// ----------------------------------------------------------------

TabItem::TabItem(const QString &uid,
                   const QString &title,
                   const QString &icon,
                   QObject *parent) :
    ListItem(parent),
    m_uid(uid),
    m_title(title),
    m_icon(icon)
{}

QHash<int, QByteArray> TabItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[UidRole] = "uid";
    names[TitleRole] = "title";
    names[IconRole] = "iconUrl";
    return names;
}

QVariant TabItem::data(int role) const
{
    switch(role) {
    case UidRole:
        return uid();
    case TitleRole:
        return title();
    case IconRole:
        return icon();
    default:
        return QVariant();
    }
}

