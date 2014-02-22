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

#ifndef ENTRYMODEL_H
#define ENTRYMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QModelIndex>
#include <QTextDocument>
#include <QChar>
#include <QDateTime>

#include "listmodel.h"
#include "feedmodel.h"
#include "databasemanager.h"

class EntryItem : public ListItem
{
    Q_OBJECT

public:
    enum Roles {
        UidRole = Qt::UserRole+1,
        TitleRole = Qt::DisplayRole,
        AuthorRole,
        ContentRole,
        LinkRole,
        ReadRole,
        ReadLaterRole,
        DateRole
    };

public:
    EntryItem(QObject *parent = 0): ListItem(parent) {}
    explicit EntryItem(const QString &uid,
                      const QString &title,
                      const QString &author,
                      const QString &content,
                      const QString &link,
                      const int read,
                      const int readlater,
                      const int date,
                      QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;
    inline QString id() const { return m_uid; }
    inline QString uid() const { return m_uid; }
    Q_INVOKABLE inline QString title() const { return m_title; }
    Q_INVOKABLE inline QString author() const { return m_author; }
    Q_INVOKABLE inline QString content() const { return m_content; }
    Q_INVOKABLE inline QString link() const { return m_link; }
    Q_INVOKABLE inline int read() const { return m_read; }
    Q_INVOKABLE inline int readlater() const { return m_readlater; }
    Q_INVOKABLE inline int date() const { return m_date; }

    void setReadlater(int value);
    void setRead(int value);

private:
    QString m_uid;
    QString m_title;
    QString m_author;
    QString m_content;
    QString m_link;
    int m_read;
    int m_readlater;
    int m_date;
};

class EntryModel : public ListModel
{
    Q_OBJECT

public:
    bool reInit; // if true init existing model on setEntryModel

    explicit EntryModel(DatabaseManager* db, QObject *parent = 0);
    Q_INVOKABLE void init(const QString &feedId);
    Q_INVOKABLE void init();
    Q_INVOKABLE int count();
    Q_INVOKABLE QObject* get(int i);
    Q_INVOKABLE void setData(int row, const QString &fieldName, QVariant newValue);

private:
   DatabaseManager* _db;
   QString _feedId;

   void createItems(const QString &feedId);
   void sort();
};

#endif // ENTRYMODEL_H
