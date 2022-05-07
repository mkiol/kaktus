/* Copyright (C) 2014-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include "settings.h"
#include "singleton.h"

class DatabaseManager : public QObject, public Singleton<DatabaseManager> {
    Q_OBJECT
    Q_PROPERTY (bool synced READ isSynced NOTIFY syncedChanged)
public:
    static const int version = 23;

    static const int dashboardsLimit = 100;
    static const int tabsLimit = 100;
    static const int streamLimit = 100;
    static const int entriesLimit = 100;

    struct StreamModuleTab {
        QString streamId;
        QString moduleId;
        QString tabId;
        int date = 0;
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
        int unread = 0;
        int read = 0;
        int saved = 0;
        int slow = 0;
        int newestItemAddedAt = 0;
        int updateAt = 0;
        int lastUpdate = 0;
    };

    struct Entry {
        QString id;
        QString streamId;
        QString title;
        QString author;
        QString link;
        QString content;
        QString image;
        QString feedId;
        QString feedIcon;
        QString feedTitle;
        QString annotations;
        int fresh = 0;
        int freshOR = 0;
        int read = 0;
        int saved = 0;
        int liked = 0;
        int cached = 0;
        int broadcast = 0;
        int publishedAt = 0;
        int createdAt = 0;
        int crawlTime = 0;
        int timestamp = 0;
    };

    struct CacheItem {
        QString id;
        QString origUrl;
        QString finalUrl;
        QString redirectUrl;
        QString baseUrl;
        QString type;
        QString contentType;
        QString entryId;
        QString streamId;
        int date = 0;
        int flag = 0;
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
        UnSetSlowRead = 60,
        SetBroadcast = 71,
        UnSetBroadcast = 70,
        SetListRead = 81,
        UnSetListRead = 80,
        SetListSaved = 91,
        UnSetListSaved = 90,
        SetLiked = 101,
        UnSetLiked = 100
    };

    struct Action {
        ActionsTypes type;
        QString id1;
        QString id2;
        QString id3;
        QString text;
        int date1 = 0;
        int date2 = 0;
        int date3 = 0;
    };

    DatabaseManager(QObject *parent = nullptr);

    Q_INVOKABLE void init();
    Q_INVOKABLE void newInit();

    bool makeBackup();
    bool restoreBackup();

    bool isSynced();

    void cleanDashboards();
    void cleanTabs();
    void cleanModules();
    void cleanStreams();
    void cleanEntries();
    void cleanCache();

    void writeDashboard(const Dashboard &item);
    void writeTab(const Tab &item);
    void writeModule(const Module &item);
    void writeStreamModuleTab(const StreamModuleTab &item);
    void writeStream(const Stream &item);
    void writeEntry(const Entry &item);
    void writeCache(const CacheItem &item);
    void writeAction(const Action &item);
    void updateActionByIdAndType(const QString &oldId1, ActionsTypes oldType, const QString &newId1, const QString &newId2, const QString &newId3, ActionsTypes newType);

    void updateEntriesReadFlagByStream(const QString &id, int flag);
    void updateEntriesReadFlagByDashboard(const QString &id, int flag);
    void updateEntriesSlowReadFlagByDashboard(const QString &id, int flag);
    void updateEntriesReadFlagByTab(const QString &id, int flag);
    void updateEntriesReadFlagByEntry(const QString &id, int flag);
    void updateEntriesSavedFlagByEntry(const QString &id, int flag);
    void updateEntriesCachedFlagByEntry(const QString &id, int cacheDate, int flag);
    void updateEntriesBroadcastFlagByEntry(const QString &id, int flag, const QString &annotations);
    void updateEntriesLikedFlagByEntry(const QString &id, int flag);
    void updateEntriesFreshFlag(int flag);
    void updateEntriesFlag(int flag);
    void updateEntriesSavedFlagByFlagAndDashboard(const QString &id, int flagOld, int flagNew);

    void updateStreamSlowFlagById(const QString &id, int flag);

    bool isDashboardExists();
    bool isCacheExists(const QString &id);
    bool isCacheExistsByFinalUrl(const QString &id);
    bool isCacheExistsByEntryId(const QString &id);

    Dashboard readDashboard(const QString &id);
    QList<Action> readActions();
    QList<Dashboard> readDashboards();
    QList<Tab> readTabsByDashboard(const QString &id);
    QList<Stream> readStreamsByTab(const QString &id);
    QList<QString> readStreamIdsByTab(const QString &id);
    QList<Stream> readStreamsByDashboard(const QString &id);
    QList<QString> readTabIdsByDashboard(const QString &id);
    QList<QString> readStreamIds();
    QString readStreamIdByEntry(const QString &id);
    QList<QString> readModuleIdByStream(const QString &id);
    QMap<QString,QString> readStreamIdsTabIds();
    QList<StreamModuleTab> readStreamModuleTabList();
    QList<StreamModuleTab> readStreamModuleTabListByTab(const QString &id);
    QList<StreamModuleTab> readStreamModuleTabListByDashboard(const QString &id);
    QList<StreamModuleTab> readSlowStreamModuleTabListByDashboard(const QString &id);
    QList<StreamModuleTab> readStreamModuleTabListWithoutDate();

    QString readEntryImageById(const QString &id);
    QString readEntryContentById(const QString &id);

    QString readLatestEntryIdByDashboard(const QString &id);
    QString readLatestEntryIdByTab(const QString &id);
    QString readLatestEntryIdByStream(const QString &id);

    QList<Entry> readEntriesByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesUnreadByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesUnreadAndSavedByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesSlowUnreadByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesSlowUnreadAndSavedByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesSavedByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    //QList<Entry> readEntriesSaved(int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesSlowByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesLikedByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesBroadcastByDashboard(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesByStream(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesUnreadByStream(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesUnreadAndSavedByStream(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesByTab(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesUnreadByTab(const QString &id, int offset, int limit, bool ascOrder = false);
    QList<Entry> readEntriesUnreadAndSavedByTab(const QString &id, int offset, int limit, bool ascOrder = false);
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
    int readLastTimestampByTab(const QString &id);
    int readLastCrawlTimeByTab(const QString &id);
    int readLastLastUpdateByTab(const QString &id);

    int readLastPublishedAtByDashboard(const QString &id);
    int readLastTimestampByDashboard(const QString &id);
    int readLastCrawlTimeByDashboard(const QString &id);
    int readLastLastUpdateByDashboard(const QString &id);

    int readLastPublishedAtByStream(const QString &id);
    int readLastTimestampByStream(const QString &id);
    int readLastCrawlTimeByStream(const QString &id);
    int readLastLastUpdateByStream(const QString &id);

    int readLastPublishedAtSlowByDashboard(const QString &id);
    int readLastTimestampSlowByDashboard(const QString &id);
    int readLastCrawlTimeSlowByDashboard(const QString &id);
    int readLastLastUpdateSlowByDashboard(const QString &id);

    void removeTabById(const QString &id);
    void removeStreamsByStream(const QString &id);
    //void removeEntriesOlderThan(int cacheDate, int limit);
    //void removeEntriesOlderThanByCrawlTime(int cacheDate);
    void removeEntriesByStream(const QString &id, int limit);
    void removeEntriesByFlag(int value);
    void removeActionsById(const QString &id);
    void removeActionsByIdAndType(const QString &id, ActionsTypes type);
    //void removeEntriesBySavedFlag(int flag);
    void removeCacheItems();

    int countEntries();
    int countStreams();
    int countTabs();
    int countEntriesByStream(const QString &id);
    int countEntriesNewerThanByStream(const QString &id, const QDateTime &date);
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
    /*
    500 - DB can not be opened
    501 - Creation of new empty DB failed
    502 - Check DB parameters failed
    511 - The database disk image is malformed
    */
    void error(int code);

    void empty();
    void notEmpty();

    void syncedChanged();

private:
    QSqlDatabase db;
    QString dbFilePath;
    QString backupFilePath;

    void checkError(const QSqlError &error);

    bool openDB();
    bool createDB();
    //bool alterDB_19to22();
    //bool alterDB_20to22();
    //bool alterDB_21to22();
    bool deleteDB();

    bool createStructure();
    bool createDashboardsStructure();
    bool createTabsStructure();
    bool createModulesStructure();
    bool createStreamsStructure();
    bool createEntriesStructure();
    bool createCacheStructure();
    bool createActionsStructure();
    bool checkParameters();
    bool isTableExists(const QString &name);
    void decodeBase64(const QVariant &source, QString &result);
};

Q_DECLARE_METATYPE(DatabaseManager::CacheItem)

#endif // DATABASEMANAGER_H
