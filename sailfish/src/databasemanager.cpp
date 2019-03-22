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

#include <QDebug>
#include <QDateTime>

#include "databasemanager.h"

DatabaseManager* DatabaseManager::m_instance = nullptr;

DatabaseManager* DatabaseManager::instance(QObject *parent)
{
    if (DatabaseManager::m_instance == nullptr) {
        DatabaseManager::m_instance = new DatabaseManager(parent);
    }

    return DatabaseManager::m_instance;
}

DatabaseManager::DatabaseManager(QObject *parent) :
    QObject(parent)
{}

DatabaseManager::~DatabaseManager()
{
    db.close();
    QSqlDatabase::removeDatabase("qt_sql_kaktus_connection");
}

bool DatabaseManager::isSynced()
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT count(*) FROM actions");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
           return false;
        }

        while(query.next()) {
            if (query.value(0).toInt() > 0) {
                return false;
            } else {
                return true;
            }
        }

    } else {
        qWarning() << "DB is not open";
    }

    return false;
}

void DatabaseManager::init()
{
    Settings *s = Settings::instance();

    // Request to reInint DB
    if (s->getReinitDB()) {
        s->setReinitDB(false);
        newInit();
        return;
    }

    if (!s->getSignedIn()) {
        if (!createDB()) {
            qWarning() << "Creation of new empty DB failed";
            emit error(501);
        } else {
            emit empty();
        }
        return;
    } else {
        if (!openDB()) {
            qWarning() << "DB can not be opened";
            emit error(500);
            return;
        }
    }

    if (!checkParameters()) {
        qWarning() << "Check DB parameters failed";
        emit error(502);
        return;
    }
}

void DatabaseManager::newInit()
{
    if (!createDB()) {
        qWarning() << "Creation of new empty DB failed";
        emit error(501);
    } else {
        emit empty();
    }
}

bool DatabaseManager::openDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE","qt_sql_kaktus_connection");
    Settings *s = Settings::instance();

    dbFilePath = s->getSettingsDir();
    //qDebug() << "Connecting to settings DB in " << dbFilePath;
    dbFilePath.append(QDir::separator()).append("settings.db");
    dbFilePath = QDir::toNativeSeparators(dbFilePath);
    db.setDatabaseName(dbFilePath);
    //db.setConnectOptions("QSQLITE_ENABLE_SHARED_CACHE");

    return db.open();
}

bool DatabaseManager::makeBackup()
{
    Settings *s = Settings::instance();

    if (backupFilePath.isEmpty()) {
        backupFilePath = s->getSettingsDir();
        backupFilePath.append(QDir::separator()).append("settings_backup.db");
        backupFilePath = QDir::toNativeSeparators(backupFilePath);
    }

    if (QFile::exists(backupFilePath)) {
        //qDebug() << "DB backup file exists and will be overwrite";
        QFile::remove(backupFilePath);
    }

    return QFile::copy(dbFilePath, backupFilePath);
}

bool DatabaseManager::restoreBackup()
{
    if (!QFile::exists(backupFilePath)) {
        qWarning() << "DB backup file doesn't exist";
        return false;
    }

    if (!deleteDB()) {
        qWarning() << "Current DB file can not be deleted";
        return false;
    }

    if (!QFile::rename(backupFilePath, dbFilePath)) {
        qWarning() << "Can not rename DB backup file";
        return false;
    }

    return openDB();
}

bool DatabaseManager::deleteDB()
{
    db.close();
    QSqlDatabase::removeDatabase("qt_sql_kaktus_connection");

    if (dbFilePath=="") {
        Settings *s = Settings::instance();
        dbFilePath = s->getSettingsDir();
        dbFilePath.append(QDir::separator()).append("settings.db");
        dbFilePath = QDir::toNativeSeparators(dbFilePath);
    }

    return QFile::remove(dbFilePath);
}

bool DatabaseManager::isTableExists(const QString &name)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        if (query.exec(QString("SELECT COUNT(*) FROM sqlite_master "
                               "WHERE type='table' AND name='%1';")
                       .arg(name))) {
            while(query.next()) {
                return query.value(0).toInt() == 1 ? true : false;
            }
        }
    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    qWarning() << "SQL error";
    return false;
}

/*bool DatabaseManager::alterDB_21to22()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("ALTER TABLE entries ADD COLUMN flag INTEGER DEFAULT 0;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
            return ret;
        }

        ret = query.exec("UPDATE parameters SET value='22';");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
            return ret;
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}*/

bool DatabaseManager::checkParameters()
{
    bool createDB = false;

    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        if (isTableExists("parameters")) {
            query.exec("SELECT value FROM parameters WHERE name='version';");
            if (query.first()) {
                int cur_db_ver = query.value(0).toString().toInt();

                //qDebug() << "DB version =" << cur_db_ver;

                if (cur_db_ver != DatabaseManager::version) {
                    qWarning() << "DB version mismatch";

                    /*if (query.value(0).toString() == "1.9") {
                        if (!alterDB_19to22()) {
                            qWarning() << "DB migration 19->22 failed";
                            createDB = true;
                        } else {
                            qDebug() << "DB migration 19->22 succeed";
                        }
                    } else {
                        createDB = true;
                    }*/

                    createDB = true;

                } else {
                    // Check is Dashboard exists
                    if (!isDashboardExists()) {
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
            qWarning() << "Parameters table not exists";
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
        qWarning() << "DB is not opened";
        return false;
    }
}

bool DatabaseManager::createDB()
{
    if (!deleteDB()) {
        qWarning() << "DB can not be deleted";
    }
    if (!openDB()) {
        qWarning() << "DB can not be opened";
        return false;
    }
    if (!createStructure()) {
        qWarning() << "Create DB structure failed";
        return false;
    }
    if (!createActionsStructure()) {
        qWarning() << "Create Actions structure failed";
        return false;
    }
    // New empty DB created
    return true;
}

bool DatabaseManager::createStructure()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS parameters;");
        ret = query.exec("CREATE TABLE IF NOT EXISTS parameters ("
                         "name CHARACTER(10) PRIMARY KEY, "
                         "value VARCHAR(10), "
                         "description TEXT "
                         ");");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        ret = query.exec(QString("INSERT INTO parameters VALUES('%1','%2','%3');")
                         .arg("version")
                         .arg(DatabaseManager::version)
                         .arg("Data structure version"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}

bool DatabaseManager::createActionsStructure()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS actions;");
        ret = query.exec("CREATE TABLE IF NOT EXISTS actions ("
                         "type INTEGER, "
                         "id1 VARCHAR(50), "
                         "id2 VARCHAR(50), "
                         "id3 VARCHAR(50), "
                         "text TEXT, "
                         "date1 TIMESTAMP, "
                         "date2 TIMESTAMP, "
                         "date3 TIMESTAMP"
                         ");");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}

bool DatabaseManager::createTabsStructure()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS tabs;");
        ret = query.exec("CREATE TABLE tabs ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "dashboard_id VARCHAR(50), "
                         "title VARCHAR(100), "
                         "icon VARCHAR(100)"
                         ");");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}

bool DatabaseManager::createCacheStructure()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS cache;");
        ret = query.exec("CREATE TABLE cache ("
                         "id CHAR(32) PRIMARY KEY, "
                         "orig_url CHAR(32), "
                         "final_url CHAR(32), "
                         "base_url TEXT, "
                         "type VARCHAR(50), "
                         "content_type TEXT, "
                         "entry_id VARCHAR(50), "
                         "stream_id VARCHAR(50), "
                         "flag INTEGER DEFAULT 0, "
                         "date TIMESTAMP "
                         ");");
        ret = query.exec("CREATE INDEX IF NOT EXISTS cache_final_url "
                         "ON cache(final_url);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS cache_entry "
                         "ON cache(entry_id);");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}

bool DatabaseManager::createDashboardsStructure()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS dashboards;");
        ret = query.exec("CREATE TABLE dashboards ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "name TEXT, "
                         "title TEXT, "
                         "description TEXT "
                         ");");
        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}

bool DatabaseManager::createStreamsStructure()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS streams;");
        ret = query.exec("CREATE TABLE streams ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "title TEXT, "
                         "content TEXT, "
                         "link TEXT, "
                         "query TEXT, "
                         "icon TEXT, "
                         "type VARCHAR(50) DEFAULT '', "
                         "unread INTEGER DEFAULT 0, "
                         "read INTEGER DEFAULT 0, "
                         "saved INTEGER DEFAULT 0, "
                         "slow INTEGER DEFAULT 0, "
                         "newest_item_added_at TIMESTAMP, "
                         "update_at TIMESTAMP, "
                         "last_update TIMESTAMP"
                         ");");
        ret = query.exec("CREATE INDEX IF NOT EXISTS streams_id "
                         "ON streams(id DESC);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS slow "
                         "ON streams(slow DESC);");
        /*ret = query.exec("CREATE INDEX IF NOT EXISTS module_id "
                         "ON streams(module_id DESC);");*/

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}


bool DatabaseManager::createModulesStructure()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS modules;");
        ret = query.exec("CREATE TABLE modules ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "tab_id VARCHAR(50), "
                         "widget_id VARCHAR(50), "
                         "page_id VARCHAR(50), "
                         "name TEXT, "
                         "title TEXT, "
                         "status VARCHAR(50), "
                         "icon TEXT "
                         ");");
        ret = query.exec("CREATE INDEX IF NOT EXISTS modules_id "
                         "ON modules(id DESC);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS tabs_id "
                         "ON modules(tab_id DESC);");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        ret = query.exec("DROP TABLE IF EXISTS module_stream;");
        ret = query.exec("CREATE TABLE module_stream ("
                         "module_id VARCHAR(50), "
                         "stream_id VARCHAR(50), "
                         "PRIMARY KEY (module_id, stream_id) "
                         ");");

        ret = query.exec("CREATE INDEX IF NOT EXISTS module_stream_modules "
                         "ON module_stream(module_id DESC);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS module_stream_streams "
                         "ON module_stream(stream_id DESC);");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}

bool DatabaseManager::createEntriesStructure()
{
    bool ret = true;
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.exec("PRAGMA journal_mode = MEMORY");
        query.exec("PRAGMA synchronous = OFF");

        ret = query.exec("DROP TABLE IF EXISTS entries;");
        ret = query.exec("CREATE TABLE entries ("
                         "id VARCHAR(50) PRIMARY KEY, "
                         "stream_id VARCHAR(50), "
                         "title TEXT, "
                         "author TEXT, "
                         "content TEXT, "
                         "link TEXT, "
                         "image TEXT, "
                         "annotations TEXT, "
                         "fresh INTEGER DEFAULT 0, "
                         "fresh_or INTEGER DEFAULT 0, "
                         "read INTEGER DEFAULT 0, "
                         "saved INTEGER DEFAULT 0, "
                         "liked INTEGER DEFAULT 0, "
                         "cached INTEGER DEFAULT 0, "
                         "broadcast INTEGER DEFAULT 0, "
                         "flag INTEGER DEFAULT 0, "
                         "created_at TIMESTAMP, "
                         "published_at TIMESTAMP, "
                         "cached_at TIMESTAMP, "
                         "timestamp TIMESTAMP, "
                         "crawl_time TIMESTAMP,"
                         "last_update TIMESTAMP "
                         ");");

        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_published_at "
                         "ON entries(published_at DESC);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_date_by_stream "
                         "ON entries(stream_id, published_at DESC);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_saved "
                         "ON entries(saved, published_at);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_read_by_stream "
                         "ON entries(stream_id, read, published_at);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_read_and_saved_by_stream "
                         "ON entries(stream_id, read, saved, published_at);");
        ret = query.exec("CREATE INDEX IF NOT EXISTS entries_stream_id "
                         "ON entries(stream_id);");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
        return false;
    }

    return ret;
}


void DatabaseManager::writeDashboard(const Dashboard &item)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.prepare("INSERT INTO dashboards (id, name, title, description) "
                      "VALUES(?,?,?,?)");

        query.addBindValue(item.id);
        query.addBindValue(item.name);
        query.addBindValue(item.title);
        query.addBindValue(item.description);

        if (!query.exec()) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::writeCache(const CacheItem &item)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.prepare("INSERT OR REPLACE INTO cache (id, orig_url, final_url, base_url, type, content_type, "
                      "entry_id, stream_id, flag, date) VALUES(?,?,?,?,?,?,?,?,?,?)");

        query.addBindValue(item.id);
        query.addBindValue(item.origUrl);
        query.addBindValue(item.finalUrl);
        query.addBindValue(item.baseUrl);
        query.addBindValue(item.type);
        query.addBindValue(item.contentType);
        query.addBindValue(item.entryId);
        query.addBindValue(item.streamId);
        query.addBindValue(item.flag);
        query.addBindValue(item.date);

        if (!query.exec()) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::writeTab(const Tab &item)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.prepare("INSERT INTO tabs (id, dashboard_id, title, icon) "
                      "VALUES(?,?,?,?)");

        query.addBindValue(item.id);
        query.addBindValue(item.dashboardId);
        query.addBindValue(item.title);
        query.addBindValue(item.icon);

        if (!query.exec()) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::writeAction(const Action &item)
{
    if (db.isOpen()) {
        bool synced = isSynced();

        QSqlQuery query(db);

        query.prepare("INSERT INTO actions (type, id1, id2, id3, text, date1, date2, date3) "
                      "VALUES(?,?,?,?,?,?,?,?)");

        query.addBindValue(static_cast<int>(item.type));
        query.addBindValue(item.id1);
        query.addBindValue(item.id2);
        query.addBindValue(item.id3);
        query.addBindValue(item.text);
        query.addBindValue(item.date1);
        query.addBindValue(QDateTime::currentDateTimeUtc().toTime_t());
        query.addBindValue(QDateTime::currentDateTimeUtc().toTime_t());

        if (!query.exec()) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        } else {
            if (synced)
                emit syncedChanged();
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateActionByIdAndType(const QString &oldId1, ActionsTypes oldType, const QString &newId1,
                                              const QString &newId2, const QString &newId3, ActionsTypes newType)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.prepare("UPDATE actions SET type = ?, id1 = ?, id2 = ?, id3 = ? WHERE type = ? AND id1 = ?");

        query.addBindValue(static_cast<int>(newType));
        query.addBindValue(newId1);
        query.addBindValue(newId2);
        query.addBindValue(newId3);
        query.addBindValue(static_cast<int>(oldType));
        query.addBindValue(oldId1);

        if (!query.exec()) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::writeStream(const Stream &item)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.prepare("INSERT INTO streams (id, title, content, link, query, icon, "
                      "type, unread, read, saved, slow, newest_item_added_at, update_at, last_update) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

        query.addBindValue(item.id);
        query.addBindValue(item.title);
        query.addBindValue(item.content);
        query.addBindValue(item.link);
        query.addBindValue(item.query);
        query.addBindValue(item.icon);
        query.addBindValue(item.type);
        query.addBindValue(item.unread);
        query.addBindValue(item.read);
        query.addBindValue(item.saved);
        query.addBindValue(item.slow);
        query.addBindValue(item.newestItemAddedAt);
        query.addBindValue(item.updateAt);
        query.addBindValue(item.lastUpdate);

        if(!query.exec()) {
            QSqlQuery query(db);
            query.prepare("UPDATE streams SET title = ?, newest_item_added_at = ?, update_at = ?, last_update = ?, "
                          "unread = ?, read = ?, saved = ?, slow = ? WHERE id = ?");

            query.addBindValue(item.title);
            query.addBindValue(item.newestItemAddedAt);
            query.addBindValue(item.updateAt);
            query.addBindValue(item.lastUpdate);
            query.addBindValue(item.unread);
            query.addBindValue(item.read);
            query.addBindValue(item.saved);
            query.addBindValue(item.slow);
            query.addBindValue(item.id);

            if (!query.exec()) {
               qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
            }
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::writeModule(const Module &item)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("INSERT INTO modules (id, tab_id, widget_id, page_id, name, title, status, icon) "
                      "VALUES (:id, :tab_id, :widget_id, :page_id, :name, :title, :status, :icon)");
        query.bindValue(":id", item.id);

        query.bindValue(":tab_id", item.tabId);
        query.bindValue(":widget_id", item.widgetId);
        query.bindValue(":page_id", item.pageId);
        query.bindValue(":name", item.name);
        query.bindValue(":title", item.title);
        query.bindValue(":status", item.status);
        query.bindValue(":icon", item.icon);

        if(!query.exec()) {
            query.prepare("UPDATE modules SET status = :status, title = :title, tab_id = :tab_id, icon = :icon, name = :name "
                          "WHERE id = :id");

            query.bindValue(":status", item.status);
            query.bindValue(":title", item.title);
            query.bindValue(":tab_id", item.tabId);
            query.bindValue(":icon", item.icon);
            query.bindValue(":name", item.name);
            query.bindValue(":id", item.id);

            if (!query.exec()) {
               qWarning() << "SQL Error! " << query.executedQuery(); checkError(query.lastError());
            }
        }

        QList<QString>::const_iterator i = item.streamList.begin();
        while (i != item.streamList.end()) {
            bool ret = query.exec(QString("INSERT OR IGNORE INTO module_stream (module_id, stream_id) VALUES('%1','%2');")
                             .arg(item.id)
                             .arg(*i));

            if (!ret) {
               qWarning() << "SQL Error:" << query.lastQuery(); checkError(query.lastError());
            }

            ++i;
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::writeStreamModuleTab(const StreamModuleTab &item)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("INSERT OR IGNORE INTO module_stream (module_id, stream_id) VALUES('%1','%2');")
                         .arg(item.moduleId)
                         .arg(item.streamId));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::writeEntry(const Entry &item)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        query.prepare("INSERT OR REPLACE INTO entries (id, stream_id, title, author, content, link, image, annotations, "
                      "fresh_or, read, saved, liked, broadcast, created_at, published_at, crawl_time, timestamp, "
                      "last_update, fresh, cached) "
                      "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?, "
                      "coalesce((SELECT fresh FROM entries WHERE id = ?),1), "
                      "coalesce((SELECT cached FROM entries WHERE id = ?),0))");

        query.addBindValue(item.id);
        query.addBindValue(item.streamId);
        query.addBindValue(item.title);
        query.addBindValue(item.author);
        query.addBindValue(item.content);
        query.addBindValue(item.link);
        query.addBindValue(item.image);
        query.addBindValue(item.annotations);
        query.addBindValue(item.freshOR);
        query.addBindValue(item.read);
        query.addBindValue(item.saved);
        query.addBindValue(item.liked);
        query.addBindValue(item.broadcast);
        query.addBindValue(item.createdAt);
        query.addBindValue(item.publishedAt);
        query.addBindValue(item.crawlTime);
        query.addBindValue(item.timestamp);
        query.addBindValue(QDateTime::currentDateTimeUtc().toTime_t());
        query.addBindValue(item.id);
        query.addBindValue(item.id);

        if (!query.exec()) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesFreshFlag(int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("UPDATE entries SET fresh=%1;").arg(flag));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesFlag(int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("UPDATE entries SET flag=%1;").arg(flag));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesCachedFlagByEntry(const QString &id, int cacheDate, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET cached=%1, cached_at=%2 WHERE id='%3';")
                         .arg(flag)
                         .arg(cacheDate)
                         .arg(id));
        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesBroadcastFlagByEntry(const QString &id, int flag, const QString &annotations)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET broadcast=%1, annotations='%2' WHERE id='%3';")
                         .arg(flag)
                         .arg(QString(annotations.toUtf8().toBase64()))
                         .arg(id));
        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesLikedFlagByEntry(const QString &id, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET liked=%1 WHERE id='%2';")
                         .arg(flag).arg(id));
        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesReadFlagByEntry(const QString &id, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET read=%1 WHERE id='%2';")
                         .arg(flag)
                         .arg(id));
        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesReadFlagByTab(const QString &id, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET read=%1 "
                                      "WHERE stream_id IN "
                                      "(SELECT ms.stream_id FROM module_stream as ms, modules as m "
                                      "WHERE ms.module_id=m.id AND m.tab_id='%2');")
                         .arg(flag)
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesSavedFlagByEntry(const QString &id, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET saved=%1 WHERE id='%2';")
                         .arg(flag)
                         .arg(id));
        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesReadFlagByStream(const QString &id, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET read=%1 WHERE stream_id='%2';")
                         .arg(flag)
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesReadFlagByDashboard(const QString &id, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET read=%1 "
                                      "WHERE stream_id IN "
                                      "(SELECT s.id FROM streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE s.id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%2');")
                         .arg(flag)
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesSavedFlagByFlagAndDashboard(const QString &id, int flagOld, int flagNew)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET saved=%1 "
                                      "WHERE saved=%2 AND stream_id IN "
                                      "(SELECT s.id FROM streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE s.id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%3');")
                         .arg(flagNew)
                         .arg(flagOld)
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateStreamSlowFlagById(const QString &id, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE streams SET slow=%1 WHERE id='%2';")
                         .arg(flag)
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

void DatabaseManager::updateEntriesSlowReadFlagByDashboard(const QString &id, int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("UPDATE entries SET read=%1 "
                                      "WHERE stream_id IN "
                                      "(SELECT s.id FROM streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE s.slow=1 AND s.id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%2');")
                         .arg(flag)
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not opened";
    }
}

DatabaseManager::Dashboard DatabaseManager::readDashboard(const QString &id)
{
    Dashboard item;

    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("SELECT id, name, title, description FROM dashboards WHERE id='%1';")
                        .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            item.id = query.value(0).toString();
            item.name = query.value(1).toString();
            item.title = query.value(2).toString();
            item.description = query.value(3).toString();
        }

    } else {
        qWarning() << "DB is not opened";
    }

    return item;
}

QList<DatabaseManager::Dashboard> DatabaseManager::readDashboards()
{
    QList<DatabaseManager::Dashboard> list;

    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("SELECT id, name, title, description FROM dashboards LIMIT %1;")
                        .arg(dashboardsLimit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Dashboard item;
            item.id = query.value(0).toString();
            item.name = query.value(1).toString();
            item.title = query.value(2).toString();
            item.description = query.value(3).toString();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Tab> DatabaseManager::readTabsByDashboard(const QString &id)
{
    QList<DatabaseManager::Tab> list;

    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("SELECT id, title, icon FROM tabs WHERE dashboard_id='%1' LIMIT %2;")
                        .arg(id)
                        .arg(tabsLimit));
        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Tab item;
            item.id = query.value(0).toString();
            item.dashboardId = id;
            item.title = query.value(1).toString();
            item.icon = query.value(2).toString();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Stream> DatabaseManager::readStreamsByTab(const QString &id)
{
    QList<DatabaseManager::Stream> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT s.id, ms.module_id, m.title, s.title, m.name, s.content, s.link, s.query, s.icon, "
                                      "s.type, s.unread, s.read, s.saved, s.slow, s.newest_item_added_at, s.update_at, s.last_update "
                                      "FROM streams as s, module_stream as ms, modules as m "
                                      "WHERE ms.stream_id=s.id AND ms.module_id=m.id AND m.tab_id='%1' "
                                      "ORDER BY s.id DESC LIMIT %2;")
                        .arg(id)
                        .arg(streamLimit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Stream item;

            item.id = query.value(0).toString();

            QString name = query.value(4).toString();
            if (name == "RssReader") {
                item.title = query.value(2).toString();
            } else {
                item.title = query.value(3).toString();
            }

            item.content = query.value(5).toString();
            item.link = query.value(6).toString();
            item.query = query.value(7).toString();
            item.icon = query.value(8).toString();
            item.type = query.value(9).toString();
            item.unread = query.value(10).toInt();
            item.read = query.value(11).toInt();
            item.saved = query.value(12).toInt();
            item.slow = query.value(13).toInt();
            item.newestItemAddedAt = query.value(14).toInt();
            item.updateAt = query.value(15).toInt();
            item.lastUpdate = query.value(16).toInt();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<QString> DatabaseManager::readStreamIdsByTab(const QString &id)
{
    QList<QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("SELECT s.id FROM streams as s, module_stream as ms, modules as m "
                                      "WHERE ms.stream_id=s.id AND ms.module_id=m.id AND m.tab_id='%1' "
                                      "LIMIT %2;")
                        .arg(id)
                        .arg(streamLimit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Stream> DatabaseManager::readStreamsByDashboard(const QString &id)
{
    QList<DatabaseManager::Stream> list;

    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec(QString("SELECT s.id, ms.module_id, m.title, s.title, m.name, s.content, s.link, s.query, s.icon, "
                                      "s.type, s.unread, s.read, s.saved, s.slow, s.newest_item_added_at, s.update_at, s.last_update "
                                      "FROM streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE ms.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "ORDER BY s.id DESC LIMIT %2;")
                        .arg(id)
                        .arg(streamLimit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Stream item;

            item.id = query.value(0).toString();

            QString name = query.value(4).toString();
            if (name == "RssReader") {
                item.title = query.value(2).toString();
            } else {
                item.title = query.value(3).toString();
            }

            item.content = query.value(5).toString();
            item.link = query.value(6).toString();
            item.query = query.value(7).toString();
            item.icon = query.value(8).toString();
            item.type = query.value(9).toString();
            item.unread = query.value(10).toInt();
            item.read = query.value(11).toInt();
            item.saved = query.value(12).toInt();
            item.slow = query.value(13).toInt();
            item.newestItemAddedAt = query.value(14).toInt();
            item.updateAt = query.value(15).toInt();
            item.lastUpdate = query.value(16).toInt();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::StreamModuleTab> DatabaseManager::readStreamModuleTabListByTab(const QString &id)
{
    QList<DatabaseManager::StreamModuleTab> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT ms.stream_id, m.id, m.tab_id "
                              "FROM module_stream as ms, modules as m "
                              "WHERE ms.module_id=m.id AND m.tab_id='%1';")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            DatabaseManager::StreamModuleTab smt;
            smt.streamId = query.value(0).toString();
            smt.moduleId = query.value(1).toString();
            smt.tabId = query.value(2).toString();
            smt.date = 0;
            list.append(smt);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::StreamModuleTab> DatabaseManager::readStreamModuleTabListByDashboard(const QString &id)
{
    QList<DatabaseManager::StreamModuleTab> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT ms.stream_id, m.id, m.tab_id "
                              "FROM module_stream as ms, modules as m, tabs as t "
                              "WHERE ms.module_id=m.id AND m.tab_id=t.id AND t.dashboard_id='%1';")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            DatabaseManager::StreamModuleTab smt;
            smt.streamId = query.value(0).toString();
            smt.moduleId = query.value(1).toString();
            smt.tabId = query.value(2).toString();
            smt.date = 0;
            list.append(smt);
            //qDebug() << smt.streamId;
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::StreamModuleTab> DatabaseManager::readSlowStreamModuleTabListByDashboard(const QString &id)
{
    QList<DatabaseManager::StreamModuleTab> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT ms.stream_id, m.id, m.tab_id "
                              "FROM streams as s, module_stream as ms, modules as m, tabs as t "
                              "WHERE s.id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                              "AND t.dashboard_id='%1' AND s.slow=1;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            DatabaseManager::StreamModuleTab smt;
            smt.streamId = query.value(0).toString();
            smt.moduleId = query.value(1).toString();
            smt.tabId = query.value(2).toString();
            smt.date = 0;
            list.append(smt);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<QString> DatabaseManager::readStreamIds()
{
    QList<QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);
        bool ret = query.exec("SELECT id FROM streams;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QString DatabaseManager::readStreamIdByEntry(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT stream_id FROM entries WHERE id='%1';")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toString();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return "";
}

QList<QString> DatabaseManager::readTabIdsByDashboard(const QString &id)
{
    QList<QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id FROM tabs WHERE dashboard_id='%1';")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QString DatabaseManager::readEntryImageById(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT image FROM entries WHERE id='%1';")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toString();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return "";
}

QString DatabaseManager::readEntryContentById(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT content FROM entries WHERE id='%1';")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toString();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return "";
}

QList<QString> DatabaseManager::readModuleIdByStream(const QString &id)
{
    QList<QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT module_id FROM module_stream WHERE module_id='%1';")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<QString> DatabaseManager::readCacheIdsOlderThan(int cacheDate, int limit)
{
    QList<QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id FROM cache WHERE entry_id IN "
                                      "(SELECT id FROM entries WHERE cached_at<%1 AND stream_id IN "
                                      "(SELECT stream_id FROM entries GROUP BY stream_id HAVING count(*)>%2));")
                        .arg(cacheDate).arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

/*QList<QString> DatabaseManager::readCacheIdsOlderThanByCrawlTime(int cacheDate, int limit)
{
    QList<QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id FROM cache WHERE entry_id IN "
                                      "(SELECT id FROM entries WHERE cached_at<%1 AND stream_id IN "
                                      "(SELECT stream_id FROM entries GROUP BY stream_id HAVING count(*)>%2));")
                        .arg(cacheDate).arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}*/

QList<QString> DatabaseManager::readCacheFinalUrlsByStream(const QString &id, int limit)
{
    QList<QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT c.final_url FROM cache as c, entries as e "
            "WHERE c.entry_id=e.id AND e.stream_id='%1' AND e.saved!=1 AND e.broadcast!=1 AND e.liked!=1 AND e.id NOT IN ("
            "SELECT id FROM entries WHERE stream_id='%1' ORDER BY published_at DESC LIMIT %2"
            ");").arg(id).arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

DatabaseManager::CacheItem DatabaseManager::readCacheByOrigUrl(const QString &id)
{
    CacheItem item;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id, orig_url, final_url, base_url, type, content_type, entry_id, stream_id, flag, date "
                                      "FROM cache WHERE orig_url='%1' AND flag=1;")
                        .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.baseUrl = query.value(3).toString();
            item.type = query.value(4).toString();
            item.contentType = query.value(5).toString();
            item.entryId = query.value(6).toString();
            item.streamId = query.value(7).toString();
            item.flag = query.value(8).toInt();
            item.date = query.value(9).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return item;
}

DatabaseManager::CacheItem DatabaseManager::readCacheByEntry(const QString &id)
{
    CacheItem item;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id, orig_url, final_url, base_url, type, content_type, entry_id, stream_id, flag, date "
                                      "FROM cache WHERE entry_id='%1' AND flag=1;")
                        .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.baseUrl = query.value(3).toString();
            item.type = query.value(4).toString();
            item.contentType = query.value(5).toString();
            item.entryId = query.value(6).toString();
            item.streamId = query.value(7).toString();
            item.flag = query.value(8).toInt();
            item.date = query.value(9).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return item;
}

DatabaseManager::CacheItem DatabaseManager::readCacheByCache(const QString &id)
{
    CacheItem item;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id, orig_url, final_url, type, content_type, entry_id, stream_id, flag, date "
                                      "FROM cache WHERE id='%1';")
                        .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.baseUrl = query.value(3).toString();
            item.type = query.value(4).toString();
            item.contentType = query.value(5).toString();
            item.entryId = query.value(6).toString();
            item.streamId = query.value(7).toString();
            item.flag = query.value(8).toInt();
            item.date = query.value(9).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return item;
}

DatabaseManager::CacheItem DatabaseManager::readCacheByFinalUrl(const QString &id)
{
    CacheItem item;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id, orig_url, final_url, base_url, type, content_type, entry_id, stream_id, flag, date "
                                      "FROM cache WHERE final_url='%1';")
                        .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            item.id = query.value(0).toString();
            item.origUrl = query.value(1).toString();
            item.finalUrl = query.value(2).toString();
            item.baseUrl = query.value(3).toString();
            item.type = query.value(4).toString();
            item.contentType = query.value(5).toString();
            item.entryId = query.value(6).toString();
            item.streamId = query.value(7).toString();
            item.flag = query.value(8).toInt();
            item.date = query.value(9).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return item;
}

bool DatabaseManager::isCacheExists(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM cache WHERE id='%1';")
                        .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            if (query.value(0).toInt() > 0) {
                return true;
            }
        }

    } else {
        qWarning() << "DB is not open";
    }

    return false;
}

bool DatabaseManager::isCacheExistsByEntryId(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT count(*) FROM cache WHERE entry_id='%1' AND flag=1;")
                        .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            if (query.value(0).toInt() > 0) {
                return true;
            }
        }

    } else {
        qWarning() << "DB is not open";
    }

    return false;
}

bool DatabaseManager::isCacheExistsByFinalUrl(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT count(*) FROM cache WHERE final_url='%1';")
                        .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            if (query.value(0).toInt() > 0) {
                return true;
            }
        }

    } else {
        qWarning() << "DB is not open";
    }

    return false;
}

bool DatabaseManager::isDashboardExists()
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT count(*) FROM dashboards;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            if (query.value(0).toInt() > 0) {
                return true;
            }
        }
    } else {
        qWarning() << "DB is not open";
    }

    return false;
}

QMap<QString,QString> DatabaseManager::readStreamIdsTabIds()
{
    QMap<QString,QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT ms.stream_id, m.tab_id FROM module_stream as ms, modules as m "
                              "WHERE ms.module_id=m.id;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.insertMulti(query.value(0).toString(), query.value(1).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::StreamModuleTab> DatabaseManager::readStreamModuleTabListWithoutDate()
{
    QList<DatabaseManager::StreamModuleTab> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT ms.stream_id, ms.module_id, m.tab_id "
                              "FROM module_stream as ms, modules as m "
                              "WHERE ms.module_id=m.id AND ms.stream_id IN "
                              "(SELECT stream_id FROM entries GROUP BY stream_id HAVING count(*)>0);");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            DatabaseManager::StreamModuleTab smt;
            smt.streamId = query.value(0).toString();
            smt.moduleId = query.value(1).toString();
            smt.tabId = query.value(2).toString();
            smt.date = 0;
            list.append(smt);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::StreamModuleTab> DatabaseManager::readStreamModuleTabList()
{
    QList<DatabaseManager::StreamModuleTab> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT e.stream_id, m.id, m.tab_id, min(e.published_at) "
                              "FROM entries as e, module_stream as ms, modules as m "
                              "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id "
                              "GROUP BY e.stream_id;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            DatabaseManager::StreamModuleTab smt;
            smt.streamId = query.value(0).toString();
            smt.moduleId = query.value(1).toString();
            smt.tabId = query.value(2).toString();
            smt.date = query.value(3).toInt();
            list.append(smt);
            //qDebug() << smt.streamId << smt.moduleId << smt.tabId << smt.date;
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

int DatabaseManager::readLastUpdateByStream(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT last_update FROM streams "
                                      "WHERE id='%1';").arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastUpdateByTab(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(s.last_update) FROM streams as s, module_stream as ms, modules as m "
                                      "WHERE ms.stream_id=s.id AND ms.module_id=m.id AND m.tab_id='%1';").arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastPublishedAtByTab(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.published_at) "
                                      "FROM entries as e, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastTimestampByTab(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.timestamp) "
                                      "FROM entries as e, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastCrawlTimeByTab(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.crawl_time) "
                                      "FROM entries as e, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastLastUpdateByTab(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.last_update) "
                                      "FROM entries as e, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastPublishedAtByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.published_at) "
                                      "FROM entries as e, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastTimestampByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.timestamp) "
                                      "FROM entries as e, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastCrawlTimeByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.crawl_time) "
                                      "FROM entries as e, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastLastUpdateByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.last_update) "
                                      "FROM entries as e, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastPublishedAtSlowByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.published_at) "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=s.id AND s.id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' AND s.slow=1;")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastTimestampSlowByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.timestamp) "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=s.id AND s.id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' AND s.slow=1;")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastCrawlTimeSlowByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.crawl_time) "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=s.id AND s.id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' AND s.slow=1;")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastLastUpdateSlowByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.last_update) "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=s.id AND s.id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' AND s.slow=1;")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastPublishedAtByStream(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.published_at) "
                                      "FROM entries as e, module_stream as ms "
                                      "WHERE e.stream_id=ms.stream_id "
                                      "AND e.stream_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastTimestampByStream(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.timestamp) "
                                      "FROM entries as e, module_stream as ms "
                                      "WHERE e.stream_id=ms.stream_id "
                                      "AND e.stream_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastCrawlTimeByStream(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.crawl_time) "
                                      "FROM entries as e, module_stream as ms "
                                      "WHERE e.stream_id=ms.stream_id "
                                      "AND e.stream_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastLastUpdateByStream(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(e.last_update) "
                                      "FROM entries as e, module_stream as ms "
                                      "WHERE e.stream_id=ms.stream_id "
                                      "AND e.stream_id='%1';")
                                      .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

int DatabaseManager::readLastUpdateByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT max(s.last_update) FROM streams as s, modules as m, module_stream as ms, tabs as t "
                                      "WHERE ms.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1';").arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return 0;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesByStream(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s "
                                      "WHERE e.stream_id='%1' AND e.stream_id=s.id "
                                      "ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QString DatabaseManager::readLatestEntryIdByStream(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id FROM entries WHERE stream_id='%1' "
                                      "ORDER BY published_at DESC LIMIT 1;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toString();
        }
    } else {
        qWarning() << "DB is not open";
    }

    return "";
}

QString DatabaseManager::readLatestEntryIdByTab(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id FROM entries as e, streams as s, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id='%1' "
                                      "ORDER BY published_at DESC LIMIT 1;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toString();
        }
    } else {
        qWarning() << "DB is not open";
    }

    return "";
}

QString DatabaseManager::readLatestEntryIdByDashboard(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id AND t.dashboard_id='%1' "
                                      "ORDER BY published_at DESC LIMIT 1;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            return query.value(0).toString();
        }
    } else {
        qWarning() << "DB is not open";
    }

    return "";
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesUnreadByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND e.read=0 ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesUnreadAndSavedByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND (e.read=0 OR e.saved=1) ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesSlowUnreadByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND e.read=0 AND s.slow=1 ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesSlowUnreadAndSavedByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND (e.read=0 OR e.saved=1) AND s.slow=1 ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesByTab(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id "
                                      "AND m.tab_id='%1' "
                                      "ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesUnreadByTab(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id "
                                      "AND m.tab_id='%1' "
                                      "AND e.read=0 ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesUnreadAndSavedByTab(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id "
                                      "AND m.tab_id='%1' "
                                      "AND (e.read=0 OR e.saved=1) ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesSavedByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND e.saved=1 ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

/*QList<DatabaseManager::Entry> DatabaseManager::readEntriesSaved(int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, e.annotations, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e ORDER BY e.published_at %3 LIMIT %1 OFFSET %2;")
                                      .arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            decodeBase64(query.value(2),item.title);
            decodeBase64(query.value(3),item.author);
            decodeBase64(query.value(4),item.content);
            decodeBase64(query.value(5),item.link);
            decodeBase64(query.value(6),item.image);
            decodeBase64(query.value(7),item.feedIcon);
            decodeBase64(query.value(8),item.feedTitle);
            decodeBase64(query.value(9),item.annotations);
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}*/

QList<DatabaseManager::Entry> DatabaseManager::readEntriesSlowByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND s.slow=1 ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesLikedByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND e.liked=1 ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesBroadcastByDashboard(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND e.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND e.broadcast=1 ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesUnreadByStream(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s "
                                      "WHERE e.stream_id='%1' AND e.stream_id=s.id AND e.read=0 "
                                      "ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesUnreadAndSavedByStream(const QString &id, int offset, int limit, bool ascOrder)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT e.id, e.stream_id, e.title, e.author, e.content, e.link, e.image, s.icon, s.title, e.annotations, s.id, "
                                      "e.fresh, e.fresh_or, e.read, e.saved, e.liked, e.cached, e.broadcast, e.created_at, e.published_at, e.timestamp, e.crawl_time, e.last_update "
                                      "FROM entries as e, streams as s "
                                      "WHERE e.stream_id='%1' AND e.stream_id=s.id AND (e.read=0 OR e.saved=1) "
                                      "ORDER BY e.published_at %4 LIMIT %2 OFFSET %3;")
                        .arg(id).arg(limit).arg(offset).arg(ascOrder ? "ASC" : "DESC"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.feedIcon = query.value(7).toString();
            item.feedTitle = query.value(8).toString();
            item.annotations = query.value(9).toString();
            item.feedId = query.value(10).toString();
            item.fresh = query.value(11).toInt();
            item.freshOR = query.value(12).toInt();
            item.read = query.value(13).toInt();
            item.saved = query.value(14).toInt();
            item.liked = query.value(15).toInt();
            item.cached = query.value(16).toInt();
            item.broadcast = query.value(17).toInt();
            item.createdAt = query.value(18).toInt();
            item.publishedAt = query.value(19).toInt();
            item.timestamp = query.value(20).toInt();
            item.crawlTime = query.value(21).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Action> DatabaseManager::readActions()
{
    QList<DatabaseManager::Action> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT type, id1, id2, id3, text, date1, date2, date3 FROM actions ORDER BY date2;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Action item;
            item.type = static_cast<ActionsTypes>(query.value(0).toInt());
            item.id1 = query.value(1).toString();
            item.id2 = query.value(2).toString();
            item.id3 = query.value(3).toString();
            item.text = query.value(4).toString();
            item.date1 = query.value(5).toInt();
            item.date2 = query.value(6).toInt();
            item.date3 = query.value(7).toInt();
            list.append(item);
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<DatabaseManager::Entry> DatabaseManager::readEntriesCachedOlderThan(int cacheDate, int limit)
{
    QList<DatabaseManager::Entry> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT id, stream_id, title, author, content, link, image, "
                                      "fresh, fresh_or, read, saved, liked, cached, created_at, published_at, timestamp, crawl_time, last_update "
                                      "FROM entries "
                                      "WHERE cached_at<%1 AND stream_id IN "
                                      "(SELECT stream_id FROM entries GROUP BY stream_id HAVING count(*)>%2);")
                        .arg(cacheDate).arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            Entry item;
            item.id = query.value(0).toString();
            item.streamId = query.value(1).toString();
            item.title = query.value(2).toString();
            item.author = query.value(3).toString();
            item.content = query.value(4).toString();
            item.link = query.value(5).toString();
            item.image = query.value(6).toString();
            item.fresh = query.value(7).toInt();
            item.freshOR = query.value(8).toInt();
            item.read = query.value(9).toInt();
            item.saved = query.value(10).toInt();
            item.liked = query.value(11).toInt();
            item.cached = query.value(12).toInt();
            item.createdAt = query.value(13).toInt();
            item.publishedAt = query.value(14).toInt();
            item.timestamp = query.value(15).toInt();
            item.crawlTime = query.value(16).toInt();
            list.append(item);
        }
    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

QList<QString> DatabaseManager::readCacheFinalUrlOlderThan(int cacheDate, int limit)
{
    QList<QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT final_url FROM cache "
                                      "WHERE entry_id IN "
                                      "(SELECT id FROM entries WHERE saved!=1 AND cached_at<%1 AND stream_id IN "
                                      "(SELECT stream_id FROM entries GROUP BY stream_id HAVING count(*)>%2));")
                        .arg(cacheDate).arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.append(query.value(0).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

void DatabaseManager::removeCacheItems()
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("DELETE FROM cache;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        ret = query.exec("UPDATE entries SET cached=0;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    }  else {
        qWarning() << "DB is not open";
    }
}

void DatabaseManager::removeStreamsByStream(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("DELETE FROM entries WHERE stream_id='%1';")
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }


        ret = query.exec(QString("DELETE FROM streams WHERE id='%1';")
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        ret = query.exec(QString("DELETE FROM module_stream WHERE stream_id='%1';")
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        // Removing empty modules
        ret = query.exec(QString("DELETE FROM modules WHERE id IN "
                                 "(SELECT module_id FROM module_stream "
                                 "GROUP BY stream_id HAVING count(*)=0);"));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        // Removing empty chache
        // TODO

    } else {
        qWarning() << "DB is not open";
    }
}

void DatabaseManager::removeTabById(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("DELETE FROM tabs WHERE id='%1';")
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not open";
    }
}

/*void DatabaseManager::removeEntriesOlderThan(int cacheDate, int limit)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN "
                                      "(SELECT id FROM entries WHERE saved!=1 AND liked!=1 AND broadcast!=1 AND cached_at<%1 AND stream_id IN "
                                      "(SELECT stream_id FROM entries GROUP BY stream_id HAVING count(*)>%2));")
                         .arg(cacheDate).arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        ret = query.exec(QString("DELETE FROM entries WHERE saved!=1 AND liked!=1 AND broadcast!=1 AND cached_at<%1 AND stream_id IN "
                                 "(SELECT stream_id FROM entries GROUP BY stream_id HAVING count(*)>%2);")
                         .arg(cacheDate).arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not open";
    }
}*/

void DatabaseManager::removeEntriesByFlag(int value)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        /*bool ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN "
                                      "(SELECT id FROM entries WHERE saved!=1 AND liked!=1 AND broadcast!=1 AND flag=%1);")
                              .arg(value));*/

        bool ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN "
                                      "(SELECT id FROM entries WHERE flag=%1);")
                              .arg(value));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        /*ret = query.exec(QString("DELETE FROM entries WHERE saved!=1 AND liked!=1 AND broadcast!=1 AND flag=%1;")
                         .arg(value));*/

        ret = query.exec(QString("DELETE FROM entries WHERE flag=%1;")
                         .arg(value));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not open";
    }
}

/*void DatabaseManager::removeEntriesOlderThanByCrawlTime(int cacheDate)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN "
                                      "(SELECT id FROM entries WHERE saved!=1 AND liked!=1 AND broadcast!=1 AND crawl_time<%1);")
                         .arg(cacheDate));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        ret = query.exec(QString("DELETE FROM entries WHERE saved!=1 AND liked!=1 AND broadcast!=1 AND crawl_time<%1);")
                         .arg(cacheDate));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    } else {
        qWarning() << "DB is not open";
    }
}*/

void DatabaseManager::removeEntriesByStream(const QString &id, int limit)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN ("
                                      "SELECT id FROM entries WHERE stream_id='%1' AND saved!=1 AND id NOT IN ("
                                      "SELECT id FROM entries WHERE stream_id='%1' ORDER BY published_at DESC LIMIT %2"
                                      "));")
                              .arg(id)
                              .arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        ret = query.exec(QString("DELETE FROM entries WHERE stream_id='%1' AND saved!=1 AND id NOT IN ("
                                 "SELECT id FROM entries WHERE stream_id='%1' ORDER BY published_at DESC LIMIT %2"
                                 ");")
                         .arg(id)
                         .arg(limit));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

    }
}



/*void DatabaseManager::removeEntriesBySavedFlag(int flag)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("DELETE FROM cache WHERE entry_id IN ("
                                      "SELECT id FROM entries WHERE saved=%1);")
                              .arg(flag));

        if (!ret) {
            qWarning() << "SQL error!" << query.lastError().text();
        }

        ret = query.exec(QString("DELETE FROM entries WHERE saved=%1;")
                         .arg(flag));

        if (!ret) {
            qWarning() << "SQL error!" << query.lastError().text();
        }

    }
}*/

void DatabaseManager::removeActionsById(const QString &id)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("DELETE FROM actions WHERE id1='%1';")
                         .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        } else {
            emit syncedChanged();
        }

    } else {
        qWarning() << "DB is not open";
    }
}

void DatabaseManager::removeActionsByIdAndType(const QString &id, ActionsTypes type)
{
    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("DELETE FROM actions WHERE id1='%1' AND type=%2;")
                         .arg(id).arg(static_cast<int>(type)));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        } else {
            emit syncedChanged();
        }

    } else {
        qWarning() << "DB is not open";
    }
}


QMap<QString,QString> DatabaseManager::readNotCachedEntries()
{
    QMap<QString,QString> list;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT id, link FROM entries WHERE cached=0;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            list.insert(query.value(0).toString(), query.value(1).toString());
        }

    } else {
        qWarning() << "DB is not open";
    }

    return list;
}

int DatabaseManager::countEntriesNotCached()
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT count(*) FROM entries WHERE cached=0;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

void DatabaseManager::cleanDashboards()
{
    createDashboardsStructure();
}

void DatabaseManager::cleanTabs()
{
    createTabsStructure();
}

void DatabaseManager::cleanModules()
{
    createModulesStructure();
}

void DatabaseManager::cleanStreams()
{
    createStreamsStructure();
}

void DatabaseManager::cleanEntries()
{
    createEntriesStructure();
}

void DatabaseManager::cleanCache()
{
    createCacheStructure();
}

int DatabaseManager::countEntries()
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT COUNT(*) FROM entries;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countStreams()
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT COUNT(*) FROM streams;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countTabs()
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec("SELECT COUNT(*) FROM tabs;");

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesByStream(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries WHERE stream_id='%1';")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesNewerThanByStream(const QString &id, const QDateTime &date)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries WHERE stream_id='%1' AND published_at>=%2;")
                              .arg(id)
                              .arg(date.toTime_t()));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesUnreadByStream(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        /*qDebug() << QString("SELECT COUNT(*) FROM entries WHERE stream_id='%1' AND read=0;")
                    .arg(id);*/

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries WHERE stream_id='%1' AND read=0;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesReadByDashboard(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=s.id AND ms.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND e.read>0;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesSlowReadByDashboard(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=s.id AND ms.stream_id=s.id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND s.slow=1 AND e.read>0;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesUnreadByDashboard(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries as e, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND e.read=0;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesSlowUnreadByDashboard(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT count(*) FROM entries as e, streams as s, module_stream as ms, modules as m, tabs as t "
                                      "WHERE e.stream_id=ms.stream_id AND s.id=e.stream_id AND ms.module_id=m.id AND m.tab_id=t.id "
                                      "AND t.dashboard_id='%1' "
                                      "AND e.read=0 AND s.slow=1;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesUnreadByTab(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        /*qDebug() << QString("SELECT COUNT(*) FROM entries as e, module_stream as ms, modules as m "
                            "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id "
                            "AND m.tab_id='%1' AND e.read=0;")
                    .arg(id);*/

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries as e, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id "
                                      "AND m.tab_id='%1' AND e.read=0;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesReadByStream(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries WHERE stream_id='%1' AND read>0;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesReadByTab(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        /*qDebug() << QString("SELECT COUNT(*) FROM entries as e, module_stream as ms, modules as m "
                            "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id "
                            "AND m.tab_id='%1' AND e.read>0;")
                    .arg(id);*/

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries as e, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id "
                                      "AND m.tab_id='%1' AND e.read>0;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesFreshByStream(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries WHERE stream_id='%1' AND fresh=1;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

int DatabaseManager::countEntriesFreshByTab(const QString &id)
{
    int count = 0;

    if (db.isOpen()) {
        QSqlQuery query(db);

        bool ret = query.exec(QString("SELECT COUNT(*) FROM entries as e, module_stream as ms, modules as m "
                                      "WHERE e.stream_id=ms.stream_id AND ms.module_id=m.id "
                                      "AND m.tab_id='%1' AND e.fresh=1;")
                              .arg(id));

        if (!ret) {
           qWarning() << "SQL Error:" << query.lastQuery();checkError(query.lastError());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

    } else {
        qWarning() << "DB is not open";
    }

    return count;
}

void DatabaseManager::decodeBase64(const QVariant &source, QString &result)
{
    QByteArray str = QByteArray::fromBase64(source.toByteArray());
    result = QString::fromUtf8(str.data(),str.size());
}

void DatabaseManager::checkError(const QSqlError &error)
{
    if (error.type()!=0) {
        qWarning() << "SQL error: number=" << error.number() << "type=" << error.type() << "text=" << error.text();

        // The database file is malformed
        if (error.number() == 11) {
            Settings *s = Settings::instance();
            if (!s->getReinitDB()) {
                s->setReinitDB(true);
                emit this->error(511);
            }
        }
    }

}
