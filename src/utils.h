/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILS_H
#define UTILS_H

#include <QDate>
#include <QList>
#include <QObject>
#include <QString>
#include <memory>

#include "dashboardmodel.h"
#include "entrymodel.h"
#include "feedmodel.h"
#include "settings.h"
#include "tabmodel.h"

class Utils : public QObject {
    Q_OBJECT

   public:
    explicit Utils(QObject *parent = nullptr);
    Q_INVOKABLE void setEntryModel(const QString &feedId);
    Q_INVOKABLE void setFeedModel(const QString &tabId);
    Q_INVOKABLE void setRootModel();
    Q_INVOKABLE void setDashboardModel();
    Q_INVOKABLE void setTabModel(const QString &dashboardId);
    Q_INVOKABLE QList<QString> dashboards() const;
    Q_INVOKABLE void copyToClipboard(const QString &text) const;
    Q_INVOKABLE QString defaultDashboardName() const;
    Q_INVOKABLE QString getHumanFriendlyTimeString(int date) const;
    Q_INVOKABLE QString getHumanFriendlySizeString(int size) const;
    Q_INVOKABLE int countUnread() const;
    Q_INVOKABLE void resetWebView() const;
    Q_INVOKABLE void resetFetcher(int type) const;
    Q_INVOKABLE QString formatHtml(const QString &data, bool offline,
                                   const QString &style = {});
    Q_INVOKABLE QString readAsset(const QString &path) const;
    static QString hash(const QString &url);
    static int monthsTo(const QDate &from, const QDate &to);
    static int yearsTo(const QDate &from, const QDate &to);
    static bool isSameWeek(const QDate &date1, const QDate &date2);
    static void addExtension(const QString &contentType, QString *path);
    static void log(const QString &data);
    static void resetWebViewStatic();

   public slots:
    void updateModels();

   private:
    std::unique_ptr<EntryModel> entryModel;
    std::unique_ptr<FeedModel> feedModel;
    std::unique_ptr<TabModel> tabModel;
    std::unique_ptr<DashboardModel> dashboardModel;

    bool removeDir(const QString &dirName);
};

#endif  // UTILS_H
