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

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QVariant>
#include <QList>
#include <QMap>

#include "settings.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static const int dashboardsLimit = 100;
    static const int tabsLimit = 100;
    static const int streamLimit = 100;
    static const int entriesLimit = 100;

    struct StreamModuleTab {
        QString streamId;
        QString moduleId;
        QString tabId;
        int date;
    };

    struct Dashboard {
        QString id;
        QString name;
        QString title;
        QString description;
    };

    struct Tab {
        QString id;
        QString dashboardId;
        QString title;
        QString icon;
    };

    struct Module {
        QString id;
        QString tabId;
        QList<QString> streamList;
        QString widgetId;
        QString pageId;
        QString name;
        QString title;
        QString status;
        QString icon;
    };

    struct Stream {
        QString id;
        QString title;
        QString content;
        QString link;
        QString query;
        QString icon;
        QString type;
        int unread;
        int read;
        int saved;
        int slow;
        int newestItemAddedAt;
        int updateAt;
        int lastUpdate;
    };

    struct Entry {
        QString id;
        QString streamId;
        QString title;
        QString author;
        QString link;
        QString content;
        QString image;
        QString feedIcon;
        int fresh;
        int read;
        int saved;
        int cached;
        int publishedAt;
        int createdAt;
    };

    struct CacheItem {
        QString id;
        QString origUrl;
        QString finalUrl;
        QString type;
        QString contentType;
        QString entryId;
        QString streamId;
        int date;
        int flag;
    };

    enum ActionsTypes {
        SetRead = 11,
        UnSetRead = 10,
        SetSaved = 21,
        UnSetSaved = 20,
        SetStreamReadAll = 30,
        UnSetStreamReadAll= 31,
        SetTabReadAll = 40,
        UnSetTabReadAll= 41,
        SetAllRead = 51,
        UnSetAllRead = 50,
        SetSlowRead = 61,
        UnSetSlowRead = 60
    };

    struct Action {
        ActionsTypes type;
        QString id1;
        QString id2;
        QString id3;
        int date1;
        int date2;
        int date3;
    };

    explicit DatabaseManager(QObject *parent = 0);

    Q_INVOKABLE void init();
    Q_INVOKABLE void newInit();

    void cleanDashboards();
    void cleanTabs();
    void cleanModules();
    void cleanStreams();
    void cleanEntries();
    void cleanCache();

    void writeDashboard(const Dashboard &item);
    void writeTab(const Tab &item);
    void writeModule(const Module &item);
    void writeStream(const Stream &item);
    void writeEntry(const Entry &item);
    void writeCache(const CacheItem &item);
    void writeAction(const Action &item);

    void updateEntriesReadFlagByStream(const QString &id, int flag);
    void updateEntriesReadFlagByDashboard(const QString &id, int flag);
    void updateEntriesSlowReadFlagByDashboard(const QString &id, int flag);
    void updateEntriesReadFlagByTab(const QString &id, int flag);
    void updateEntriesReadFlagByEntry(const QString &id, int flag);
    void updateEntriesSavedFlagByEntry(const QString &id, int flag);
    void updateEntriesCachedFlagByEntry(const QString &id, int cacheDate, int flag);
    void updateEntriesFreshFlag(int flag);

    bool isDashboardExists();
    bool isCacheExists(const QString &id);
    bool isCacheExistsByFinalUrl(const QString &id);
    bool isCacheExistsByEntryId(const QString &id);

    Dashboard readDashboard(const QString &id);
    QList<Action> readActions();
    QList<Dashboard> readDashboards();
    QList<Tab> readTabsByDashboard(const QString &id);
    QList<Stream> readStreamsByTab(const QString &id);
    QList<Stream> readStreamsByDashboard(const QString &id);
    //QList<QString> readStreamIdsByTab(const QString &id);
    //QList<QString> readStreamIdsByDashboard(const QString &id);
    //QList<QString> readStreamSlowIdsByDashboard(const QString &id);
    QList<QString> readStreamIds();
    QString readStreamIdByEntry(const QString &id);
    QList<QString> readModuleIdByStream(const QString &id);
    QMap<QString,QString> readStreamIdsTabIds();
    QList<StreamModuleTab> readStreamModuleTabList();
    QList<StreamModuleTab> readStreamModuleTabListByTab(const QString &id);
    QList<StreamModuleTab> readStreamModuleTabListByDashboard(const QString &id);
    QList<StreamModuleTab> readSlowStreamModuleTabListByDashboard(const QString &id);
    QList<StreamModuleTab> readStreamModuleTabListWithoutDate();

    QList<Entry> readEntriesByDashboard(const QString &id, int offset, int limit);
    QList<Entry> readEntriesUnreadByDashboard(const QString &id, int offset, int limit);
    QList<Entry> readEntriesSlowUnreadByDashboard(const QString &id, int offset, int limit);
    QList<Entry> readEntriesSavedByDashboard(const QString &id, int offset, int limit);
    QList<Entry> readEntriesSlowByDashboard(const QString &id, int offset, int limit);
    QList<Entry> readEntriesByStream(const QString &id, int offset, int limit);
    QList<Entry> readEntriesUnreadByStream(const QString &id, int offset, int limit);
    QList<Entry> readEntriesByTab(const QString &id, int offset, int limit);
    QList<Entry> readEntriesUnreadByTab(const QString &id, int offset, int limit);
    QList<Entry> readEntriesCachedOlderThan(int cacheDate, int limit);
    QList<QString> readCacheFinalUrlOlderThan(int cacheDate, int limit);
    QList<QString> readCacheIdsOlderThan(int cacheDate, int limit);

    CacheItem readCacheByOrigUrl(const QString &id);
    CacheItem readCacheByEntry(const QString &id);
    CacheItem readCacheByFinalUrl(const QString &id);
    CacheItem readCacheByCache(const QString &id);
    QList<QString> readCacheFinalUrlsByStream(const QString &id, int limit);
    QMap<QString,QString> readNotCachedEntries();

    int readLastUpdateByTab(const QString &id);
    int readLastUpdateByDashboard(const QString &id);
    int readLastUpdateByStream(const QString &id);

    int readLastPublishedAtByTab(const QString &id);
    int readLastPublishedAtByDashboard(const QString &id);
    int readLastPublishedAtByStream(const QString &id);
    int readLastPublishedAtSlowByDashboard(const QString &id);

    void removeStreamsByStream(const QString &id);
    void removeEntriesOlderThan(int cacheDate, int limit);
    void removeEntriesByStream(const QString &id, int limit);
    void removeActionsById(const QString &id);
    void removeCacheItems();

    int countEntries();
    int countStreams();
    int countTabs();
    int countEntriesByStream(const QString &id);
    int countEntriesUnreadByStream(const QString &id);
    int countEntriesUnreadByTab(const QString &id);
    int countEntriesReadByStream(const QString &id);
    int countEntriesReadByTab(const QString &id);
    int countEntriesFreshByStream(const QString &id);
    int countEntriesFreshByTab(const QString &id);
    int countEntriesReadByDashboard(const QString &id);
    int countEntriesUnreadByDashboard(const QString &id);
    int countEntriesSlowReadByDashboard(const QString &id);
    int countEntriesSlowUnreadByDashboard(const QString &id);
    int countEntriesNotCached();

signals:
    void error();
    void empty();
    void notEmpty();

private:
    static const QString version;
    QSqlDatabase db;
    QString dbFilePath;

    bool openDB();//
    bool createDB();//
    bool deleteDB();//

    bool createStructure();//
    bool createDashboardsStructure();//
    bool createTabsStructure();//
    bool createModulesStructure();//
    bool createStreamsStructure();//
    bool createEntriesStructure();//
    bool createCacheStructure();//
    bool createActionsStructure();//
    bool checkParameters();//
    bool isTableExists(const QString &name);//
    void decodeBase64(const QVariant &source, QString &result);//
};

Q_DECLARE_METATYPE(DatabaseManager::CacheItem)

#endif // DATABASEMANAGER_H
