/*
  Copyright (C) 2015 Michal Kosciesza <michal@mkiol.net>

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

#ifdef SAILFISH
#include <sailfishapp.h>
#endif
#include <QPainter>
#include <QRect>
#include <QColor>
#include <QDebug>
#include <QStringList>

#include "nviconprovider.h"

const QString NvIconProvider::availableColors[6] = {"green", "blue", "orange", "pink", "grey", "purple"};
const QString NvIconProvider::spriteMap[5][10] = {
    {"plus","home","label-2","star","label","pin","sheet","power","diamond","folder"},
    {"enveloppe","happy-face","rss","calc","clock","pen","bug","label-box","yen","snail"},
    {"cloud","cog","vbar","pie","table","line","magnifier","potion","pound","euro"},
    {"lightbulb","movie","note","camera","mobile","computer","heart","bubbles","dollars"},
    {"alert","bill","funnel","eye","bubble","calendar","check","crown","plane"}
};

NvIconProvider::NvIconProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap NvIconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    auto parts = id.split('?');
    if (parts.size() < 2)
        parts = QStringList{"plus", "green"};

    QString spriteImagePath;
#ifdef SAILFISH
    spriteImagePath = SailfishApp::pathTo("qml/sprite-icons.png").toString(QUrl::RemoveScheme);
#endif
#ifdef ANDROID
    spriteImagePath = ":/images/sprite-icons.png";
#endif

    QPixmap iconsPixmap{spriteImagePath};
    QPixmap iconPixmap = iconsPixmap.copy(getPosition(parts.at(0), parts.at(1)));

    if (size)
        *size  = iconPixmap.size();

    if (requestedSize.width() > 0 && requestedSize.height() > 0)
        return iconPixmap.scaled(requestedSize.width(), requestedSize.height(), Qt::IgnoreAspectRatio);
    else
        return iconPixmap;
}

QRect NvIconProvider::getPosition(const QString &icon, const QString &color)
{
    const int size = 40;
    const int x_off = 30;
    const int y_off = 28;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (spriteMap[i][j] == icon) {
                return QRect{x_off + (6 * size * i) + getOffsetByColor(color),
                             y_off + (j * size),
                             size, size};
            }
        }
    }

    return QRect{x_off, y_off, size, size};
}

int NvIconProvider::getOffsetByColor(const QString &color)
{
    int index = 0;

    for (int i = 0; i < 6; ++i) {
        if (availableColors[i] == color) {
            index = i;
            break;
        }
    }

    return index * 40;
}


