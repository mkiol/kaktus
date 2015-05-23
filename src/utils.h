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
#include <QList>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDate>
//#include <QNetworkConfigurationManager>

#ifdef BB10
#include <bb/device/DisplayInfo>
#include <bb/cascades/Color>
#endif

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

    //static bool removeDir(const QString &dirName);

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
    Q_INVOKABLE bool isLight();
    Q_INVOKABLE void resetQtWebKit();
    Q_INVOKABLE void resetFetcher(int type);
    //Q_INVOKABLE bool isOnline();

    static QString hash(const QString &url);
    static int monthsTo(const QDate &from, const QDate &to);
    static int yearsTo(const QDate &from, const QDate &to);
    static bool isSameWeek(const QDate &date1, const QDate &date2);

#ifdef BB10
    Q_INVOKABLE bool checkOSVersion(int major, int minor, int patch = 0, int build = 0);
    Q_INVOKABLE int du(float value);
    Q_INVOKABLE bb::cascades::Color background();
    Q_INVOKABLE bb::cascades::Color plain();
    Q_INVOKABLE bb::cascades::Color plainBase();
    Q_INVOKABLE bb::cascades::Color textOnPlain();
    Q_INVOKABLE bb::cascades::Color text();
    Q_INVOKABLE bb::cascades::Color secondaryText();
    Q_INVOKABLE bb::cascades::Color primary();
    Q_INVOKABLE void launchBrowser(const QString &url);
#endif

public slots:
    void updateModels();

private:
    EntryModel* entryModel;
    FeedModel* feedModel;
    TabModel* tabModel;
    DashboardModel* dashboardModel;
    //QNetworkConfigurationManager* ncm;
#ifdef BB10
    bb::device::DisplayInfo display;
#endif
    bool removeDir(const QString &dirName);
};

#endif // UTILS_H
