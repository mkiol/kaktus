/* Copyright (C) 2015-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QPixmap>
#include <QQuickImageProvider>
#include <QSize>
#include <QString>

class IconProvider : public QQuickImageProvider {
   public:
    IconProvider();
    QPixmap requestPixmap(const QString &id, QSize *size,
                          const QSize &requestedSize);

   private:
    QString themeDir;
};

#endif  // ICONPROVIDER_H
