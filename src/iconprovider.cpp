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

#include <sailfishapp.h>
#include <mlite5/MGConfItem>
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "iconprovider.h"

IconProvider::IconProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    this->themeDir = IconProvider::themeDirPath();
}

QString IconProvider::themeDirPath()
{
    QString themeDir;
    double ratio = MGConfItem("/desktop/sailfish/silica/theme_pixel_ratio").value().toDouble();
    if (ratio == 0) {
        qWarning() << "Pixel ratio is 0, defaulting to 1.0.";
        themeDir = SailfishApp::pathTo("images/z1.0").toString(QUrl::RemoveScheme);
    } else if (ratio == 1.0) {
        themeDir = SailfishApp::pathTo("images/z1.0").toString(QUrl::RemoveScheme);
    } else if (ratio == 1.25) {
        themeDir = SailfishApp::pathTo("images/z1.25").toString(QUrl::RemoveScheme);
    } else if (ratio == 1.5) {
        themeDir = SailfishApp::pathTo("images/z1.5").toString(QUrl::RemoveScheme);
    } else if (ratio == 1.75 || ratio == 1.8) {
        themeDir = SailfishApp::pathTo("images/z1.75").toString(QUrl::RemoveScheme);
    } else if (ratio == 2.0) {
        themeDir = SailfishApp::pathTo("images/z2.0").toString(QUrl::RemoveScheme);
    } else {
        themeDir = SailfishApp::pathTo("images/z1.0").toString(QUrl::RemoveScheme);
    }

    if (!QDir(themeDir).exists()) {
        qWarning() << "Theme" << themeDir << "for ratio" << ratio << "doesn't exist";
        themeDir = SailfishApp::pathTo("images/z1.0").toString(QUrl::RemoveScheme);
    }
    return themeDir;
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QStringList parts = id.split('?');

    QString filepath = themeDir + "/" + parts.at(0) + ".png";
    if (!QFile::exists(filepath)) {
        // Icon file is not exist -> fallback to default icon
        filepath = themeDir + "/icon-m-item.png";
    }

    QPixmap sourcePixmap(filepath);

    if (size)
        *size  = sourcePixmap.size();

    if (parts.length() > 1)
        if (QColor::isValidColor(parts.at(1))) {
            QPainter painter(&sourcePixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(sourcePixmap.rect(), parts.at(1));
            painter.end();
        }

    if (requestedSize.width() > 0 && requestedSize.height() > 0)
        return sourcePixmap.scaled(requestedSize.width(), requestedSize.height(), Qt::IgnoreAspectRatio);
    else
        return sourcePixmap;
}
