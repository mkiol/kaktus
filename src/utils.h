/*
  Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>

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

#include <QDate>
#include <QList>
#include <QObject>
#include <QString>

#include "dashboardmodel.h"
#include "entrymodel.h"
#include "feedmodel.h"
#include "settings.h"
#include "tabmodel.h"

class Utils : public QObject {
    Q_OBJECT

   public:
    explicit Utils(QObject *parent = nullptr);
    ~Utils();

    Q_INVOKABLE void setEntryModel(const QString &feedId);
    Q_INVOKABLE void setFeedModel(const QString &tabId);
    Q_INVOKABLE void setRootModel();
    Q_INVOKABLE void setDashboardModel();
    Q_INVOKABLE QList<QString> dashboards();
    Q_INVOKABLE void copyToClipboard(const QString &text);
    Q_INVOKABLE QString defaultDashboardName();
    Q_INVOKABLE QString getHumanFriendlyTimeString(int date);
    Q_INVOKABLE QString getHumanFriendlySizeString(int size);
    Q_INVOKABLE int countUnread();
    Q_INVOKABLE void resetQtWebKit();
    Q_INVOKABLE void resetFetcher(int type);
    Q_INVOKABLE QString formatHtml(const QString &data, bool offline,
                                   const QString &style = QString());
    Q_INVOKABLE QString readAsset(const QString &path);
    Q_INVOKABLE QString nameFromPath(const QString &path);

    static QString hash(const QString &url);
    static int monthsTo(const QDate &from, const QDate &to);
    static int yearsTo(const QDate &from, const QDate &to);
    static bool isSameWeek(const QDate &date1, const QDate &date2);
    static void addExtension(const QString &contentType, QString &path);
    Q_INVOKABLE static void log(const QString &data);

   public slots:
    void updateModels();

   private:
    EntryModel *entryModel;
    FeedModel *feedModel;
    TabModel *tabModel;
    DashboardModel *dashboardModel;

    bool removeDir(const QString &dirName);
};

#endif  // UTILS_H
