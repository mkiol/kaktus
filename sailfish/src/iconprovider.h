/*
  Copyright (C) 2014-2019 Michal Kosciesza <michal@mkiol.net>

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

#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QSize>
#include <QPixmap>
#include <QQuickImageProvider>
#include <QString>

class IconProvider : public QQuickImageProvider
{
public:
    static QString themeDirPath();

    IconProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QString themeDir;
};

#endif // ICONPROVIDER_H
