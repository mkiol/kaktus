/*
  Copyright (C) 2017 Michal Kosciesza <michal@mkiol.net>

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

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSqlQuery>

#include "ai.h"
#include "settings.h"

Ai::Ai(QObject *parent) : QObject(parent)
{
}

Ai::~Ai()
{
    db.close();
    QSqlDatabase::removeDatabase("qt_sql_kaktusai_connection");
}

void Ai::init()
{
    if (!openDB()) {
        qWarning() << "Error when trying to open AI-DB.";
        return;
    }

    int ver = version();
    if (Ai::VERSION != ver) {
        if (ver != 0) {
            qWarning() << "AI-DB version mismatch. Exising version is"
                     << ver << ", but required is" << Ai::VERSION;
            deleteDB();

            init();
            if (!openDB()) {
                qWarning() << "Error when trying to open AI-DB.";
                return;
            }
        } else {
            qWarning() << "AI-DB file doesn't exist.";
        }

        createDB();
    }
}

bool Ai::openDB()
{
    Settings *s = Settings::instance();

    db = QSqlDatabase::addDatabase("QSQLITE","qt_sql_kaktusai_connection");
    dbFilePath = s->getSettingsDir();
    dbFilePath.append(QDir::separator()).append("ai.db");
    dbFilePath = QDir::toNativeSeparators(dbFilePath);
    db.setDatabaseName(dbFilePath);

    return db.open();
}

void Ai::deleteDB()
{
    db.close();
    QSqlDatabase::removeDatabase("qt_sql_kaktusai_connection");

    if (!QFile::exists(dbFilePath)) {
        qWarning() << "AI-DB file doesn't exist.";
        return;
    }

    QFile::remove(dbFilePath);
}

int Ai::version()
{
    QSqlQuery query(db);
    query.exec("PRAGMA user_version");
    query.first();
    return query.value(0).toInt();
}

void Ai::createDB()
{
    QSqlQuery query(db);
    query.exec("PRAGMA journal_mode = MEMORY");
    query.exec("PRAGMA synchronous = OFF");
    query.exec(QString("PRAGMA user_version = %1").arg(Ai::VERSION));

    query.exec("CREATE TABLE IF NOT EXISTS entries ("
                     "id VARCHAR(50) PRIMARY KEY, "
                     "title TEXT, "
                     "evaluation INTEGER DEFAULT 0, "
                     "status INTEGER DEFAULT 1, "
                     "last_update TIMESTAMP "
                     ");");
    query.exec("CREATE INDEX IF NOT EXISTS entries_evaluation "
                     "ON entries(id, evaluation);");
}

void Ai::addEvaluation(const QString &id, const QString &title, int evaluation)
{
    if (!db.isOpen()) {
        qWarning() << "AI-DB is not open.";
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO entries (id, title, evaluation, last_update) VALUES (?,?,?,?)");
    query.addBindValue(id);
    query.addBindValue(title);
    query.addBindValue(evaluation);
    query.addBindValue(QDateTime::currentDateTimeUtc().toTime_t());

    if (!query.exec())
       qWarning() << "SQL error:" << query.lastQuery();
}

int Ai::evaluationCount(const int evaluation)
{
    if (!db.isOpen()) {
        qWarning() << "AI-DB is not open.";
        return 0;
    }

    QSqlQuery query(db);
    query.prepare("SELECT count(*) FROM entries WHERE evaluation = ?");
    query.addBindValue(evaluation);

    if (!query.exec()) {
       qWarning() << "SQL error:" << query.lastQuery();
       return 0;
    }

    while (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

int Ai::evaluation(const QString &id)
{
    if (!db.isOpen()) {
        qWarning() << "AI-DB is not open.";
        return 0;
    }

    QSqlQuery query(db);
    query.prepare("SELECT evaluation FROM entries WHERE id = ? LIMIT 1");
    query.addBindValue(id);

    if (!query.exec()) {
       qWarning() << "SQL error:" << query.lastQuery();
       return 0;
    }

    while (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}
