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

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QQuickView>
#include <QQmlContext>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QGuiApplication>
#include <QClipboard>

#include "tabmodel.h"
#include "dashboardmodel.h"
#include "feedmodel.h"
#include "entrymodel.h"
#include "databasemanager.h"
#include "settings.h"


class Utils : public QObject
{
    Q_OBJECT

public:
    explicit Utils(QObject *parent = 0);
    ~Utils();
    Q_INVOKABLE void setEntryModel(const QString &feedId);
    Q_INVOKABLE void setFeedModel(const QString &tabId);
    Q_INVOKABLE void setTabModel(const QString &dashboardId);
    Q_INVOKABLE void setDashboardModel();
    Q_INVOKABLE QList<QString> dashboards();
    Q_INVOKABLE void copyToClipboard(const QString &text);
    Q_INVOKABLE QString defaultDashboardName();
    /*Q_INVOKABLE bool showNotification(const QString previewSummary,
                                      const QString previewBody = "",
                                      const QString icon = "");*/

public slots:
    void updateModels();
    
private:
    QMap<QString, EntryModel*> _entryModelsList;
    QMap<QString, FeedModel*> _feedModelsList;
    QMap<QString, TabModel*> _tabModelsList;
    DashboardModel* _dashboardModel;
};

#endif // UTILS_H
