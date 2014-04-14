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

#include "QDateTime"

#include "databasemanager.h"

const QString DatabaseManager::version = QString("1.0");

DatabaseManager::DatabaseManager(QObject *parent) :
    QObject(parent)
{}

void DatabaseManager::init()
{
    if (!openDB()) {
        qWarning() << "DB can not be opened!";
        emit error();
        return;
    }

    Settings *s = Settings::instance();
    if (!s->getSignedIn()) {
        if (!createDB()) {
            qWarning() << "Creation of new empty DB failed!";
            emit error();
        } else {
            emit empty();
        }
        return;
    }

    if (!checkParameters()) {
        qWarning() << "Check DB parameters failed!";
        emit error();
        return;
    }
}

bool DatabaseManager::openDB()
{
    _db = QSqlDatabase::addDatabase("QSQLITE","qt_sql_kaktus_connection");
    Settings *s = Settings::instance();

    dbFilePath = s->getSettingsDir();
    dbFilePath.append(QDir::separator()).append("settings.db");
    dbFilePath = QDir::toNativeSeparators(dbFilePath);
    _db.setDatabaseName(dbFilePath);
    //_db.setConnectOptions("QSQLITE_ENABLE_SHARED_CACHE");

    return _db.open();
}

QSqlError DatabaseManager::lastError()
{
    return _db.lastError();
}

bool DatabaseManager::deleteDB()
{
    _db.close();
    return QFile::remove(dbFilePath);
}

bool DatabaseManager::isTableExists(const QString &name)
{
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        if (query.exec(QString("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='%1';").arg(name))) {
            while(query.next()) {
                //qDebug() << query.value(0).toInt();
                if (query.value(0).toInt() == 1)
                    return true;
                return false;
            }
        }
    } else {
        qWarning() << "DB is not opened!";
        return false;
    }

    qWarning() << "SQL error!";
    return false;
}

bool DatabaseManager::checkParameters()
{
    bool createDB = false;

    if (_db.isOpen()) {
        QSqlQuery query(_db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        if (isTableExists("parameters")) {
            // Table parameters exists
            //qDebug() << "DB parameters table exists!";
            query.exec("SELECT value FROM parameters WHERE name='version';");
            if (query.first()) {
                //qDebug() << "DB version=" << query.value(0).toString();
                if (query.value(0).toString() != version) {
                    qWarning() << "DB version mismatch!";
                    createDB = true;
                } else {
                    //qDebug() << "DB version ok!";
                    // Check is Dashboard exists
                    if (!isDashborardExists()) {
                        emit empty();
                    } else {
                        emit notEmpty();
                    }

                    return true;
                }
            }
            else {
                createDB = true;
            }
        } else {
            //qDebug() << "Parameters table not exists!";
            createDB = true;
        }

        if (createDB) {
            if (!this->createDB())
                return false;
            emit empty();
        } else {
            emit notEmpty();
        }

        return true;

    } else {
        qWarning() << "DB is not opened!";
        return false;
    }
}

bool DatabaseManager::createDB()
{
    if (!deleteDB()) {
        qWarning() << "DB can not be deleted!";
        //return false;
    }
    if (!openDB()) {
        qWarning() << "DB can not be opened!";
        return false;
    }
    if (!createStructure()) {
        qWarning() << "Create DB structure faild!";
        return false;
    }
    if (!createActionsStructure()) {
        qWarning() << "Create Actions structure faild!";
        return false;
    }
    // New empty DB created
    //qDebug() << "New empty DB created!";
    return true;
}

bool DatabaseManager::createStructure()
{
    bool ret = true;
    if (_db.isOpen()) {
        QSqlQuery query(_db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS parameters;");
        ret = query.exec("CREATE TABLE IF NOT EXISTS parameters ("
                         "name CHARACTER(10) PRIMARY KEY, "
                         "value VARCHAR(10), "
                         "description TEXT "
                         ");");
        ret = query.exec(QString("INSERT INTO parameters VALUES('%1','%2','%3');")
                         .arg("version").arg(DatabaseManager::version).arg("Data structure version"));
    } else {
        qWarning() << "DB is not opened!";
        return false;
    }

    return ret;
}

bool DatabaseManager::createActionsStructure()
{
    bool ret = true;
    if (_db.isOpen()) {
        QSqlQuery query(_db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS actions;");
        ret = query.exec("CREATE TABLE IF NOT EXISTS actions ("
                         "type INTEGER, "
                         "feed_id VARCHAR(50), "
                         "entry_id VARCHAR(50), "
                         "older_date TIMESTAMP, "
                         "date TIMESTAMP"
                         ");");
    } else {
        qWarning() << "DB is not opened!";
        return false;
    }

    return ret;
}

bool DatabaseManager::createTabsStructure()
{
    bool ret = true;
    if (_db.isOpen()) {
        QSqlQuery query(_db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS tabs;");
        ret = query.exec("CREATE TABLE tabs ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "dashboard_id VARCHAR(50), "
                         "title VARCHAR(100), "
                         "icon VARCHAR(100) "
                         ");");
    } else {
        qWarning() << "DB is not opened!";
        return false;
    }

    return ret;
}

bool DatabaseManager::createCacheStructure()
{
    bool ret = true;
    if (_db.isOpen()) {
        QSqlQuery query(_db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS cache;");
        ret = query.exec("CREATE TABLE cache ("
                         "id CHAR(32) PRIMARY KEY, "
                         "orig_url CHAR(32), "
                         "final_url CHAR(32), "
                         "type VARCHAR(50), "
                         "content_type TEXT, "
                         "entry_id VARCHAR(50), "
                         "feed_id VARCHAR(50), "
                         "cached INTEGER DEFAULT 0, "
                         "cached_date TIMESTAMP "
                         ");");
        ret = query.exec("CREATE INDEX IF NOT EXISTS cache_final_url "
                         "ON cache(final_url);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS cache_entry "
                         "ON cache(entry_id);");
    } else {
        qWarning() << "DB is not opened!";
        return false;
    }

    return ret;
}

bool DatabaseManager::createDashboardsStructure()
{
    bool ret = true;
    if (_db.isOpen()) {
        QSqlQuery query(_db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS dashboards;");
        ret = query.exec("CREATE TABLE dashboards ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "name TEXT, "
                         "title TEXT, "
                         "description TEXT "
                         ");");
    } else {
        qWarning() << "DB is not opened!";
        return false;
    }

    return ret;
}

bool DatabaseManager::createFeedsStructure()
{
    bool ret = true;
    if (_db.isOpen()) {
        QSqlQuery query(_db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS feeds;");
        ret = query.exec("CREATE TABLE feeds ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "tab_id VARCHAR(50), "
                         "title TEXT, "
                         "content TEXT, "
                         "link TEXT, "
                         "url TEXT, "
                         "icon TEXT, "
                         "stream_id VARCHAR(50), "
                         "unread INTEGER DEFAULT 0, "
                         "read INTEGER DEFAULT 0, "
                         "readlater INTEGER DEFAULT 0, "
                         "item_count INTEGER, "
                         "new_item_count INTEGER DEFAULT 0, "
                         "error_code INTEGER DEFAULT 0, "
                         "last_parsed TIMESTAMP, "
                         "next_update TIMESTAMP, "
                         "last_update TIMESTAMP "
                         ");");
    } else {
        qWarning() << "DB is not opened!";
        return false;
    }

    return ret;
}

bool DatabaseManager::createEntriesStructure()
{
    bool ret = true;
    if (_db.isOpen()) {
        QSqlQuery query(_db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS entries;");
        ret = query.exec("CREATE TABLE entries ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "feed_id VARCHAR(50), "
                         "title TEXT, "
                         "author TEXT, "
                         "content TEXT, "
                         "link TEXT, "
                         "read INTEGER DEFAULT 0, "
                         "readlater INTEGER DEFAULT 0, "
                         "created_at TIMESTAMP, "
                         "updated_at TIMESTAMP, "
                         "date TIMESTAMP, "
                         "cached INTEGER DEFAULT 0, "
                         "cached_date TIMESTAMP "
                         ");");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_date "
                         "ON entries(date DESC);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_date_by_feed "
                         "ON entries(feed_id,date DESC);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_readlater "
                         "ON entries(readlater, date);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_read_by_feed "
                         "ON entries(feed_id, read, date);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_feed_id "
                         "ON entries(feed_id);");
    } else {
        qWarning() << "DB is not opened!";
        return false;
    }

    return ret;
}


bool DatabaseManager::writeDashboard(const Dashboard &dashboard)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("INSERT INTO dashboards VALUES('%1','%2','%3','%4');")
                         .arg(dashboard.id).arg(dashboard.name)
                         .arg(QString(dashboard.title.toUtf8().toBase64()))
                         .arg(QString(dashboard.description.toUtf8().toBase64())));
    }

    return ret;
}

bool DatabaseManager::writeCache(const CacheItem &item, int cacheDate, int flag)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("INSERT INTO cache (id, orig_url, final_url, type, content_type, entry_id, feed_id, cached, cached_date) VALUES('%1','%2','%3','%4','%5','%6','%7',%8,'%9');")
                         .arg(item.id)
                         .arg(item.origUrl).arg(item.finalUrl)
                         .arg(item.type)
                         .arg(QString(item.contentType.toUtf8().toBase64()))
                         .arg(item.entryId).arg(item.feedId)
                         .arg(flag)
                         .arg(cacheDate)
                         );
    }

    return ret;
}

bool DatabaseManager::writeTab(const QString &dashboardId, const Tab &tab)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("INSERT INTO tabs VALUES('%1','%2','%3','%4');")
                         .arg(tab.id).arg(dashboardId)
                         .arg(QString(tab.title.toUtf8().toBase64()))
                         .arg(tab.icon));
    }

    return ret;
}

bool DatabaseManager::writeAction(Action &action)
{
    bool ret = false;
    if (_db.isOpen()) {

        // finding reverse action type
        /*DatabaseManager::ActionsTypes rtype, ftype;
        switch (action.type) {
        case DatabaseManager::SetRead:
            rtype = DatabaseManager::UnSetRead;
            break;
        case DatabaseManager::UnSetRead:
            rtype = DatabaseManager::SetRead;
            break;
        case DatabaseManager::UnSetReadlater:
            rtype = DatabaseManager::SetReadlater;
            break;
        case DatabaseManager::SetReadlater:
            rtype = DatabaseManager::UnSetReadlater;
            break;
        case DatabaseManager::SetReadAll:
            rtype = DatabaseManager::UnSetReadAll;
            break;
        case DatabaseManager::UnSetReadAll:
            rtype = DatabaseManager::SetReadAll;
            break;
        }

        if (action.date==0)
            action.date = QDateTime::currentDateTimeUtc().toTime_t();

        QString sql;
        if (action.type==DatabaseManager::SetReadAll||action.type==DatabaseManager::UnSetReadAll)
            sql = QString("SELECT rowid, type FROM actions WHERE feed_id='%1' AND (type=%2 OR type=%3) ORDER BY date;")
                    .arg(action.entryId)
                    .arg(static_cast<int>(action.type))
                    .arg(static_cast<int>(rtype));
        else
            sql = QString("SELECT rowid, type FROM actions WHERE entry_id='%1' AND (type=%2 OR type=%3) ORDER BY date;")
                    .arg(action.entryId)
                    .arg(static_cast<int>(action.type))
                    .arg(static_cast<int>(rtype));


        QSqlQuery query(sql,_db);
        bool empty = true; int rowid;
        while(query.next()) {
            empty = false;
            ftype = static_cast<ActionsTypes>(query.value(1).toInt());
            rowid = query.value(0).toInt();
        }

        if (!empty && ftype!=action.type) {
            ret = query.exec(QString("UPDATE actions SET type=%1, date=%2 WHERE rowid=%3;")
                             .arg(static_cast<int>(action.type))
                             .arg(action.date)
                             .arg(rowid));
        }

        if (empty) {
            ret = query.exec(QString("INSERT INTO actions (type, feed_id, entry_id, date) VALUES(%1,'%2','%3',%5);")
                             .arg(static_cast<int>(action.type))
                             .arg(action.feedId)
                             .arg(action.entryId)
                             .arg(action.date));
        }*/

        QSqlQuery query(_db);
        ret = query.exec(QString("INSERT INTO actions (type, feed_id, entry_id, older_date, date) VALUES(%1,'%2','%3',%4,%5);")
                         .arg(static_cast<int>(action.type))
                         .arg(action.feedId)
                         .arg(action.entryId)
                         .arg(action.olderDate)
                         .arg(QDateTime::currentDateTimeUtc().toTime_t()));

    } else {
        qWarning() << "DB is not open!";
    }

    return ret;
}

bool DatabaseManager::writeFeed(const QString &tabId, const Feed &feed)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        //qDebug() << "writeFeed, " << feed.id << tabId << feed.title;
        ret = query.exec(QString("INSERT INTO feeds (id, tab_id, title, content, link, url, icon, stream_id, unread, read, readlater, last_update) VALUES('%1','%2','%3','%4','%5','%6','%7','%8',%9,%10,%11,'%12');")
                         .arg(feed.id)
                         .arg(tabId)
                         .arg(QString(feed.title.toUtf8().toBase64()))
                         .arg(QString(feed.content.toUtf8().toBase64()))
                         .arg(feed.link)
                         .arg(feed.url)
                         .arg(feed.icon)
                         .arg(feed.streamId)
                         .arg(feed.unread)
                         .arg(feed.read)
                         .arg(feed.readlater)
                         .arg(feed.lastUpdate));
        //qDebug() << "ret1" << ret;
        if(!ret) {
            ret = query.exec(QString("UPDATE feeds SET last_update='%1', unread=%2, read=%3, readlater=%4 WHERE id='%5';")
                             .arg(feed.lastUpdate)
                             .arg(feed.unread)
                             .arg(feed.read)
                             .arg(feed.readlater)
                             .arg(feed.id));
        }
        //qDebug() << "ret2" << ret;
    }

    return ret;
}

bool DatabaseManager::removeFeed(const QString &feedId)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN (SELECT id FROM entries WHERE feed_id='%1');").arg(feedId));
        ret = query.exec(QString("DELETE FROM entries WHERE feed_id='%1'").arg(feedId));
        ret = query.exec(QString("DELETE FROM feeds WHERE id='%1'").arg(feedId));

    }

    return ret;
}

/*bool DatabaseManager::removeCacheItems(const QString &entryId)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("DELETE FROM cache WHERE entry_id='%1'").arg(entryId));
    }

    return ret;
}*/

bool DatabaseManager::writeEntry(const QString &feedId, const Entry &entry)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        //qDebug() << "new, feedId=" << feedId << "readlater=" << entry.readlater;
        ret = query.exec(QString("INSERT INTO entries (id, feed_id, title, author, content, link, read, readlater, date) VALUES('%1','%2','%3','%4','%5','%6',%7,%8,'%9');")
                         .arg(entry.id)
                         .arg(feedId)
                         .arg(QString(entry.title.toUtf8().toBase64()))
                         .arg(QString(entry.author.toUtf8().toBase64()))
                         .arg(QString(entry.content.toUtf8().toBase64()))
                         .arg(entry.link)
                         .arg(entry.read).arg(entry.readlater)
                         .arg(entry.date));
        if(!ret) {
            ret = query.exec(QString("UPDATE entries SET read=%1, readlater=%2 WHERE id='%3';")
                             .arg(entry.read)
                             .arg(entry.readlater)
                             .arg(entry.id));
        }
    }

    return ret;
}

bool DatabaseManager::updateEntryCache(const QString &entryId, int cacheDate, int flag)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("UPDATE entries SET cached=%1, cached_date='%2' WHERE id='%3';")
                         .arg(flag)
                         .arg(cacheDate)
                         .arg(entryId));
    }

    return ret;
}

bool DatabaseManager::updateEntryReadFlag(const QString &entryId, int read)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("UPDATE entries SET read=%1 WHERE id='%2';")
                         .arg(read)
                         .arg(entryId));
    }

    return ret;
}

bool DatabaseManager::updateEntryReadlaterFlag(const QString &entryId, int readlater)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("UPDATE entries SET readlater=%1 WHERE id='%2';")
                         .arg(readlater)
                         .arg(entryId));
    }

    return ret;
}

bool DatabaseManager::updateEntriesReadFlag(const QString &feedId, int read)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("UPDATE entries SET read=%1 WHERE feed_id='%2';")
                         .arg(read)
                         .arg(feedId));
    }

    return ret;
}

bool DatabaseManager::updateFeedReadFlag(const QString &feedId, int unread, int read)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("UPDATE feeds SET unread=%1, read=%2 WHERE id='%3';")
                         .arg(unread)
                         .arg(read)
                         .arg(feedId));
    }

    return ret;
}

bool DatabaseManager::updateFeedReadlaterFlag(const QString &feedId, int readlater)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("UPDATE feeds SET readlater=%1 WHERE id='%2';")
                         .arg(readlater)
                         .arg(feedId));
    }

    return ret;
}

DatabaseManager::Dashboard DatabaseManager::readDashboard(const QString &dashboardId)
{
    Dashboard d;
    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, name, title, description FROM dashboards WHERE id='%1';")
                        .arg(dashboardId),_db);
        while(query.next()) {
            d.id = query.value(0).toString();
            d.name = query.value(1).toString();
            d.title = QString(QByteArray::fromBase64(query.value(2).toByteArray()));
            d.description = QString(QByteArray::fromBase64(query.value(3).toByteArray()));
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return d;
}

QList<DatabaseManager::Dashboard> DatabaseManager::readDashboards()
{
    QList<DatabaseManager::Dashboard> list;

    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, name, title, description FROM dashboards LIMIT %1;")
                        .arg(dashboardsLimit),_db);
        while(query.next()) {
            Dashboard d;
            d.id = query.value(0).toString();
            d.name = query.value(1).toString();
            d.title = QString(QByteArray::fromBase64(query.value(2).toByteArray()));
            d.description = QString(QByteArray::fromBase64(query.value(3).toByteArray()));
            list.append(d);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<DatabaseManager::Tab> DatabaseManager::readTabs(const QString &dashboardId)
{
    QList<DatabaseManager::Tab> list;

    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, title, icon FROM tabs WHERE dashboard_id='%1' LIMIT %2;")
                        .arg(dashboardId)
                        .arg(tabsLimit),_db);
        while(query.next()) {
            //qDebug() << "readTabs, " << query.value(1).toString();
            Tab t;
            t.id = query.value(0).toString();
            t.title = QString(QByteArray::fromBase64(query.value(1).toByteArray()));
            t.icon = query.value(2).toString();
            list.append(t);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<DatabaseManager::Feed> DatabaseManager::readFeeds(const QString &tabId)
{
    QList<DatabaseManager::Feed> list;

    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, title, content, link, url, icon, stream_id, unread, read, readlater, last_update FROM feeds WHERE tab_id='%1' LIMIT %2;")
                        .arg(tabId)
                        .arg(feedsLimit),_db);
        while(query.next()) {
            //qDebug() << "readFeeds, " << query.value(1).toString();
            Feed f;
            f.id = query.value(0).toString();
            f.title = QString(QByteArray::fromBase64(query.value(1).toByteArray()));
            f.content = QString(QByteArray::fromBase64(query.value(2).toByteArray()));
            f.link = query.value(3).toString();
            f.url = query.value(4).toString();
            f.icon = query.value(5).toString();
            f.streamId = query.value(6).toString();
            f.unread = query.value(7).toInt();
            f.read = query.value(8).toInt();
            f.readlater = query.value(9).toInt();
            f.lastUpdate = query.value(10).toInt();
            list.append(f);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<QString> DatabaseManager::readAllFeedIds()
{
    QList<QString> list;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT id FROM feeds;",_db);
        while(query.next())
            list.append(query.value(0).toString());
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QString DatabaseManager::readFeedId(const QString &entryId)
{
    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT feed_id FROM entries WHERE id='%1';").arg(entryId),_db);
        while(query.next()) {
            return query.value(0).toString();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return "";
}

/*QList<DatabaseManager::CacheItem> DatabaseManager::readCacheItems()
{
    QList<DatabaseManager::CacheItem> list;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT id, orig_url, final_url, type, content_type, entry_id, feed_id FROM cache WHERE cached>0;",_db);
        while(query.next()) {
            CacheItem item;
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.type = query.value(3).toString();
            item.contentType = QString(QByteArray::fromBase64(query.value(4).toByteArray()));
            item.entryId = query.value(5).toString();
            item.feedId = query.value(6).toString();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}*/

QList<QString> DatabaseManager::readCacheIdsOlderThan(int cacheDate, int limit)
{
    QList<QString> list;

    if (_db.isOpen()) {

        QSqlQuery query(QString("SELECT id FROM cache WHERE entry_id IN (SELECT id FROM entries WHERE cached_date<%1 AND feed_id IN (SELECT feed_id FROM entries GROUP BY feed_id HAVING count(*)>%2));")
                        .arg(cacheDate).arg(limit), _db);
        while(query.next()) {
            list.append(query.value(0).toString());
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<QString> DatabaseManager::readCacheFinalUrlsByLimit(const QString &feedId, int limit)
{
    QList<QString> list;

    if (_db.isOpen()) {

        QSqlQuery query(_db);

        if (!query.exec(QString("SELECT c.final_url, e.readlater FROM cache as c, entries as e "
            "WHERE c.entry_id==e.id AND e.feed_id='%1' AND e.readlater!=1 AND e.id NOT IN ("
            "SELECT id FROM entries WHERE feed_id='%1' ORDER BY date DESC LIMIT %2"
            ");").arg(feedId).arg(limit)))
            qWarning() << "SQL error!";
        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

bool DatabaseManager::removeEntriesByLimit(const QString &feedId, int limit)
{
    bool ret = false;

    if (_db.isOpen()) {

        QSqlQuery query(_db);

        ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN ("
        "SELECT id FROM entries WHERE feed_id='%1' AND readlater!=1 AND id NOT IN ("
        "SELECT id FROM entries WHERE feed_id='%1' ORDER BY date DESC LIMIT %2"
        "));").arg(feedId).arg(limit));

        ret = query.exec(QString("DELETE FROM entries WHERE feed_id='%1' AND readlater!=1 AND id NOT IN ("
        "SELECT id FROM entries WHERE feed_id='%1' ORDER BY date DESC LIMIT %2"
        ");").arg(feedId).arg(limit));

        if (!ret)
            qWarning() << "SQL error!";
    }

    return ret;
}

/*bool DatabaseManager::removeCacheItemsOlderThan(int cacheDate, int limit)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN (SELECT id FROM entries WHERE cached_date<%1 AND feed_id IN (SELECT feed_id FROM entries GROUP BY feed_id HAVING count(*)>%2));")
                        .arg(cacheDate).arg(limit));
    } else {
        qWarning() << "DB is not open!";
    }

    return ret;
}*/


DatabaseManager::CacheItem DatabaseManager::readCacheItemFromOrigUrl(const QString &origUrl)
{
    CacheItem item;
    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, orig_url, final_url, type, content_type, entry_id, feed_id FROM cache WHERE orig_url='%1' AND cached=1;")
                        .arg(origUrl),_db);
        //qDebug() << "size=" << query.size() << query.lastError() << origUrl;
        while(query.next()) {
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.type = query.value(3).toString();
            item.contentType = QString(QByteArray::fromBase64(query.value(4).toByteArray()));
            item.entryId = query.value(5).toString();
            item.feedId = query.value(6).toString();
            //qDebug() << "item.contentType=" << item.contentType;
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return item;
}

DatabaseManager::CacheItem DatabaseManager::readCacheItemFromEntryId(const QString &entryId)
{
    CacheItem item;
    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, orig_url, final_url, type, content_type, entry_id, feed_id FROM cache WHERE entry_id='%1' AND cached=1;")
                        .arg(entryId),_db);
        while(query.next()) {
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.type = query.value(3).toString();
            item.contentType = QString(QByteArray::fromBase64(query.value(4).toByteArray()));
            item.entryId = query.value(5).toString();
            item.feedId = query.value(6).toString();
            //qDebug() << "item.contentType=" << item.contentType;
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return item;
}

DatabaseManager::CacheItem DatabaseManager::readCacheItem(const QString &cacheId)
{
    CacheItem item;
    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, orig_url, final_url, type, content_type, entry_id, feed_id FROM cache WHERE id='%1';")
                        .arg(cacheId),_db);
        while(query.next()) {
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.type = query.value(3).toString();
            item.contentType = QString(QByteArray::fromBase64(query.value(4).toByteArray()));
            item.entryId = query.value(5).toString();
            item.feedId = query.value(6).toString();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return item;
}

DatabaseManager::CacheItem DatabaseManager::readCacheItemFromFinalUrl(const QString &finalUrl)
{
    CacheItem item;
    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, orig_url, final_url, type, content_type, entry_id, feed_id FROM cache WHERE final_url='%1';")
                        .arg(finalUrl),_db);
        while(query.next()) {
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.type = query.value(3).toString();
            item.contentType = QString(QByteArray::fromBase64(query.value(4).toByteArray()));
            item.entryId = query.value(5).toString();
            item.feedId = query.value(6).toString();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return item;
}

bool DatabaseManager::isCacheItemExists(const QString &cacheId)
{
    if (_db.isOpen()) {
        //qDebug() << QString("SELECT COUNT(*) FROM cache WHERE id='%1';").arg(cacheId);
        QSqlQuery query(QString("SELECT COUNT(*) FROM cache WHERE id='%1';")
                        .arg(cacheId),_db);
        while(query.next()) {
            //qDebug() << query.value(0).toInt();
            if (query.value(0).toInt() > 0) {
                return true;
            }
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return false;
}

bool DatabaseManager::isCacheItemExistsByEntryId(const QString &entryId)
{
    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT COUNT(*) FROM cache WHERE entry_id='%1' AND cached=1;")
                        .arg(entryId),_db);
        while(query.next()) {
            if (query.value(0).toInt() > 0) {
                return true;
            }
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return false;
}

bool DatabaseManager::isCacheItemExistsByFinalUrl(const QString &finalUrl)
{
    if (_db.isOpen()) {
        //qDebug() << QString("SELECT COUNT(*) FROM cache WHERE id='%1';").arg(cacheId);
        QSqlQuery query(QString("SELECT COUNT(*) FROM cache WHERE final_url='%1';")
                        .arg(finalUrl),_db);
        while(query.next()) {
            //qDebug() << query.value(0).toInt();
            if (query.value(0).toInt() > 0) {
                return true;
            }
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return false;
}

bool DatabaseManager::isDashborardExists()
{
    if (_db.isOpen()) {
        QSqlQuery query("SELECT COUNT(*) FROM dashboards;",_db);
        while(query.next()) {
            if (query.value(0).toInt() > 0) {
                return true;
            }
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return false;
}

QMap<QString,int> DatabaseManager::readFeedsLastUpdate()
{
    QMap<QString,int> list;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT id, last_update FROM feeds;",_db);
        while(query.next()) {
            list.insert(query.value(0).toString(), query.value(1).toInt());
            //qDebug() << "is: " << query.value(0).toString() << "timestamp: " << query.value(1).toInt();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QMap<QString,int> DatabaseManager::readFeedsFirstUpdate()
{
    QMap<QString,int> list;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT feed_id, min(date) FROM entries GROUP BY feed_id;",_db);
        while(query.next()) {
            list.insert(query.value(0).toString(), query.value(1).toInt());
            //qDebug() << "is: " << query.value(0).toString() << "timestamp: " << query.value(1).toInt();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

int DatabaseManager::readLatestEntryDateByFeedId(const QString &feedId)
{
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        query.exec(QString("SELECT max(date) FROM entries WHERE feed_id='%1';").arg(feedId));
        while(query.next()) {
            return query.value(0).toInt();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return 0;
}

int DatabaseManager::readFeedLastUpadate(const QString &feedId)
{
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        query.exec(QString("SELECT last_update FROM feeds WHERE id='%1';").arg(feedId));
        while(query.next()) {
            return query.value(0).toInt();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return 0;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntries(const QString &feedId)
{
    QList<DatabaseManager::Entry> list;

    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, title, author, content, link, read, readlater, date FROM entries WHERE feed_id='%1' ORDER BY date DESC LIMIT %2;")
                        .arg(feedId)
                        .arg(entriesLimit),_db);
        while(query.next()) {
            //qDebug() << "readEntries, " << query.value(1).toString();
            Entry e;
            e.id = query.value(0).toString();
            e.title = QString(QByteArray::fromBase64(query.value(1).toByteArray()));
            e.author = QString(QByteArray::fromBase64(query.value(2).toByteArray()));
            e.content = QString(QByteArray::fromBase64(query.value(3).toByteArray()));
            e.link = query.value(4).toString();
            e.read = query.value(5).toInt();
            e.readlater= query.value(6).toInt();
            e.date = query.value(7).toInt();
            list.append(e);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesReadlater(const QString &dashboardId)
{
    QList<DatabaseManager::Entry> list;

    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT e.id, e.title, e.author, e.content, e.link, e.read, e.readlater, e.date "
                                "FROM entries as e, feeds as f, tabs as t "
                                "WHERE e.feed_id=f.id AND f.tab_id=t.id AND t.dashboard_id=%1 AND e.readlater=1 "
                                "ORDER BY date DESC LIMIT %2;")
                        .arg(dashboardId)
                        .arg(entriesLimit),_db);

        while(query.next()) {
            Entry e;
            e.id = query.value(0).toString();
            e.title = QString(QByteArray::fromBase64(query.value(1).toByteArray()));
            e.author = QString(QByteArray::fromBase64(query.value(2).toByteArray()));
            e.content = QString(QByteArray::fromBase64(query.value(3).toByteArray()));
            e.link = query.value(4).toString();
            e.read = query.value(5).toInt();
            e.readlater= query.value(6).toInt();
            e.date = query.value(7).toInt();
            list.append(e);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesUnread(const QString &feedId)
{
    QList<DatabaseManager::Entry> list;

    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, title, author, content, link, read, readlater, date FROM entries WHERE read=0 AND feed_id='%1' ORDER BY date DESC LIMIT %2;")
                        .arg(feedId)
                        .arg(entriesLimit),_db);
        while(query.next()) {
            Entry e;
            e.id = query.value(0).toString();
            e.title = QString(QByteArray::fromBase64(query.value(1).toByteArray()));
            e.author = QString(QByteArray::fromBase64(query.value(2).toByteArray()));
            e.content = QString(QByteArray::fromBase64(query.value(3).toByteArray()));
            e.link = query.value(4).toString();
            e.read = query.value(5).toInt();
            e.readlater= query.value(6).toInt();
            e.date = query.value(7).toInt();
            list.append(e);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntries()
{
    QList<DatabaseManager::Entry> list;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT id, title, author, content, link, read, readlater, date FROM entries ORDER BY date DESC;",_db);
        while(query.next()) {
            Entry e;
            e.id = query.value(0).toString();
            e.title = QString(QByteArray::fromBase64(query.value(1).toByteArray()));
            e.author = QString(QByteArray::fromBase64(query.value(2).toByteArray()));
            e.content = QString(QByteArray::fromBase64(query.value(3).toByteArray()));
            e.link = query.value(4).toString();
            e.read = query.value(5).toInt();
            e.readlater= query.value(6).toInt();
            e.date = query.value(7).toInt();
            list.append(e);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<DatabaseManager::Action> DatabaseManager::readActions()
{
    QList<DatabaseManager::Action> list;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT type, feed_id, entry_id, older_date, date FROM actions ORDER BY date;",_db);
        while(query.next()) {
            Action a;
            a.type = static_cast<ActionsTypes>(query.value(0).toInt());
            a.feedId = query.value(1).toString();
            a.entryId = query.value(2).toString();
            a.olderDate = query.value(3).toInt();
            a.date = query.value(4).toInt();
            list.append(a);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesCachedOlderThan(int cacheDate, int limit)
{
    QList<DatabaseManager::Entry> list;

    if (_db.isOpen()) {
        QSqlQuery query(QString("SELECT id, title, author, content, link, read, readlater, date FROM entries WHERE cached_date<%1 AND feed_id IN (SELECT feed_id FROM entries GROUP BY feed_id HAVING count(*)>%2);")
                        .arg(cacheDate).arg(limit), _db);
        while(query.next()) {
            Entry e;
            e.id = query.value(0).toString();
            e.title = QString(QByteArray::fromBase64(query.value(1).toByteArray()));
            e.author = QString(QByteArray::fromBase64(query.value(2).toByteArray()));
            e.content = QString(QByteArray::fromBase64(query.value(3).toByteArray()));
            e.link = query.value(4).toString();
            e.read = query.value(5).toInt();
            e.readlater= query.value(6).toInt();
            e.date = query.value(7).toInt();
            list.append(e);
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

QList<QString> DatabaseManager::readCacheFinalUrlOlderThan(int cacheDate, int limit)
{
    QList<QString> list;
    //bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        /*qDebug() << QString("SELECT final_url FROM cache WHERE entry_id IN (SELECT id FROM entries WHERE readlater!=1 AND cached_date<%1 AND feed_id IN (SELECT feed_id FROM entries GROUP BY feed_id HAVING count(*)>%2));")
                    .arg(cacheDate).arg(limit);*/

        query.exec(QString("SELECT final_url FROM cache WHERE entry_id IN (SELECT id FROM entries WHERE readlater!=1 AND cached_date<%1 AND feed_id IN (SELECT feed_id FROM entries GROUP BY feed_id HAVING count(*)>%2));")
                        .arg(cacheDate).arg(limit));
        while(query.next()) {
            list.append(query.value(0).toString());
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

bool DatabaseManager::removeEntriesOlderThan(int cacheDate, int limit)
{
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN (SELECT id FROM entries WHERE readlater!=1 AND cached_date<%1 AND feed_id IN (SELECT feed_id FROM entries GROUP BY feed_id HAVING count(*)>%2));")
                         .arg(cacheDate).arg(limit));

        ret = query.exec(QString("DELETE FROM entries WHERE readlater!=1 AND cached_date<%1 AND feed_id IN (SELECT feed_id FROM entries GROUP BY feed_id HAVING count(*)>%2);")
                         .arg(cacheDate).arg(limit));
    }

    return ret;
}

bool DatabaseManager::removeAction(const QString &entryId)
{
    /// @todo fix it!
    bool ret = false;
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        ret = query.exec(QString("DELETE FROM actions WHERE entry_id='%1';")
                         .arg(entryId));
    }

    return ret;
}

QMap<QString,QString> DatabaseManager::readNotCachedEntries()
{
    QMap<QString,QString> list;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT id, link FROM entries WHERE cached=0;",_db);
        while(query.next()) {
            list.insert(query.value(0).toString(), query.value(1).toString());
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return list;
}

int DatabaseManager::readNotCachedEntriesCount()
{
    int count = 0;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT count(*) FROM entries WHERE cached=0;",_db);
        while(query.next()) {
            count = query.value(0).toInt();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return count;
}

bool DatabaseManager::cleanDashboards()
{
    return createDashboardsStructure();
}

bool DatabaseManager::cleanTabs()
{
    return createTabsStructure();
}

bool DatabaseManager::cleanFeeds()
{
    return createFeedsStructure();
}

bool DatabaseManager::cleanEntries()
{
    return createEntriesStructure();
}

bool DatabaseManager::cleanCache()
{
    return createCacheStructure();
}

int DatabaseManager::readEntriesCount()
{
    int count = 0;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT count(*) FROM entries;",_db);
        while(query.next()) {
            count = query.value(0).toInt();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return count;
}

int DatabaseManager::readFeedsCount()
{
    int count = 0;

    if (_db.isOpen()) {
        QSqlQuery query("SELECT count(*) FROM feeds;",_db);
        while(query.next()) {
            count = query.value(0).toInt();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return count;
}

DatabaseManager::Flags DatabaseManager::readTabFlags(const QString &tabId)
{
    Flags flags = {0,0,0};

    if (_db.isOpen()) {
        QSqlQuery query(_db);
        query.exec(QString("SELECT sum(unread) as unread, sum(read) as read, sum(readlater) as readlater FROM feeds WHERE tab_id='%1';")
                   .arg(tabId));
        while(query.next()) {

            /*qDebug() << tabId;
            qDebug() << "unread" << query.value(0).toInt();
            qDebug() << "read" << query.value(1).toInt();
            qDebug() << "readlater" << query.value(2).toInt();*/

            flags.unread = query.value(0).toInt();
            flags.read = query.value(1).toInt();
            flags.readlater = query.value(2).toInt();

        }
    } else {
        qWarning() << "DB is not open!";
    }

    return flags;
}

int DatabaseManager::readUnreadCount(const QString &dashboardId)
{
    if (_db.isOpen()) {
        QSqlQuery query(_db);
        query.exec(QString("SELECT sum(f.unread) FROM feeds as f, tabs as t WHERE f.tab_id=t.id AND t.dashboard_id='%1';")
                   .arg(dashboardId));
        while(query.next()) {
            return query.value(0).toInt();
        }
    } else {
        qWarning() << "DB is not open!";
    }

    return 0;
}

