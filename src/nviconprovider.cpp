/* Copyright (C) 2015-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "nviconprovider.h"

#include <sailfishapp.h>

#include <QColor>
#include <QDebug>
#include <QPainter>
#include <QRect>
#include <QStringList>

const QString NvIconProvider::availableColors[6] = {"green", "blue", "orange",
                                                    "pink",  "grey", "purple"};
const QString NvIconProvider::spriteMap[5][10] = {
    {"plus", "home", "label-2", "star", "label", "pin", "sheet", "power",
     "diamond", "folder"},
    {"enveloppe", "happy-face", "rss", "calc", "clock", "pen", "bug",
     "label-box", "yen", "snail"},
    {"cloud", "cog", "vbar", "pie", "table", "line", "magnifier", "potion",
     "pound", "euro"},
    {"lightbulb", "movie", "note", "camera", "mobile", "computer", "heart",
     "bubbles", "dollars"},
    {"alert", "bill", "funnel", "eye", "bubble", "calendar", "check", "crown",
     "plane"}};

QPixmap NvIconProvider::requestPixmap(const QString &id, QSize *size,
                                      const QSize &requestedSize) {
    auto parts = id.split('?');
    if (parts.size() < 2) parts = QStringList{"plus", "green"};

    QPixmap iconsPixmap{SailfishApp::pathTo("qml/sprite-icons.png")
                            .toString(QUrl::RemoveScheme)};
    auto iconPixmap = iconsPixmap.copy(getPosition(parts.at(0), parts.at(1)));

    if (size) *size = iconPixmap.size();

    if (requestedSize.width() > 0 && requestedSize.height() > 0) {
        return iconPixmap.scaled(requestedSize.width(), requestedSize.height(),
                                 Qt::IgnoreAspectRatio);
    }

    return iconPixmap;
}

QRect NvIconProvider::getPosition(const QString &icon, const QString &color) {
    static const int rows = 5;
    static const int cols = 10;
    static const int size = 40;
    static const int x_off = 30;
    static const int y_off = 28;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (spriteMap[i][j] == icon) {
                return {x_off + (6 * size * i) + getOffsetByColor(color),
                        y_off + (j * size), size, size};
            }
        }
    }

    return {x_off, y_off, size, size};
}

int NvIconProvider::getOffsetByColor(const QString &color) {
    int index = 0;

    for (int i = 0; i < 6; ++i) {
        if (availableColors[i] == color) {
            index = i;
            break;
        }
    }

    return index * 40;
}
