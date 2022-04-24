/* Copyright (C) 2015-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NVICONPROVIDER_H
#define NVICONPROVIDER_H

#include <QPixmap>
#include <QQuickImageProvider>
#include <QRect>
#include <QSize>
#include <QString>

class NvIconProvider : public QQuickImageProvider {
   public:
    NvIconProvider() : QQuickImageProvider{QQuickImageProvider::Pixmap} {}
    QPixmap requestPixmap(const QString &id, QSize *size,
                          const QSize &requestedSize);

   private:
    static const QString availableColors[6];
    static const QString spriteMap[5][10];

    static int getOffsetByColor(const QString &color);
    static QRect getPosition(const QString &icon, const QString &color);
};

#endif  // NVICONPROVIDER_H
