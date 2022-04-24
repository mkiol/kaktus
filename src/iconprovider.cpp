/* Copyright (C) 2015-2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "iconprovider.h"

#include <sailfishapp.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPainter>
#include <mlite5/MGConfItem>

static inline QString pathTo(const QString &dir) {
    return SailfishApp::pathTo(dir).toString(QUrl::RemoveScheme);
}

IconProvider::IconProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap) {
    auto ratio =
        MGConfItem(QStringLiteral("/desktop/sailfish/silica/theme_pixel_ratio"))
            .value()
            .toDouble();
    qDebug() << "device pixel ratio:" << ratio;
    if (ratio == 1.0) {
        themeDir = pathTo(QStringLiteral("images/z1.0"));
    } else if (ratio == 1.25) {
        themeDir = pathTo(QStringLiteral("images/z1.25"));
    } else if (ratio == 1.5) {
        themeDir = pathTo(QStringLiteral("images/z1.5"));
    } else if (ratio == 1.65 || ratio == 1.75 || ratio == 1.8) {
        themeDir = pathTo(QStringLiteral("images/z1.75"));
    } else if (ratio == 2.0) {
        themeDir = pathTo(QStringLiteral("images/z2.0"));
    } else {
        qWarning() << "Unknown pixel ratio so, defaulting to 1.0";
        themeDir = pathTo(QStringLiteral("images/z1.0"));
    }
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size,
                                    const QSize &requestedSize) {
    auto parts = id.split('?');
    auto filepath = QString{"%1/%2.png"}.arg(themeDir, parts.at(0));
    if (!QFile::exists(filepath)) {
        filepath = themeDir + "/icon-m-item.png";
    }

    QPixmap pixmap{filepath};

    if (parts.size() > 1 && QColor::isValidColor(parts.at(1))) {
        QPainter painter{&pixmap};
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(pixmap.rect(), parts.at(1));
        painter.end();
    }

    if (size) *size = pixmap.size();

    if (requestedSize.width() > 0 && requestedSize.height() > 0) {
        return pixmap.scaled(requestedSize.width(), requestedSize.height());
    }

    return pixmap;
}
