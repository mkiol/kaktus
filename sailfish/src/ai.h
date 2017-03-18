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

#ifndef AI_H
#define AI_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class Ai : public QObject
{
    Q_OBJECT
public:
    explicit Ai(QObject *parent = 0);
    ~Ai();
    Q_INVOKABLE void init();
    Q_INVOKABLE void addEvaluation(const QString &id, const QString &title, int evaluation);
    Q_INVOKABLE int evaluation(const QString &id);
    Q_INVOKABLE int evaluationCount(const int evaluation);

private:
    const static int VERSION = 1;
    QSqlDatabase db;
    QString dbFilePath;

    bool openDB();
    int version();
    void deleteDB();
    void createDB();
};

#endif // AI_H
