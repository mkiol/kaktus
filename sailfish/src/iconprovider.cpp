#include <sailfishapp.h>
#include <QPainter>
#include <mlite5/MGConfItem>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "iconprovider.h"

IconProvider::IconProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    // Getting pixel ratio
    double ratio = MGConfItem("/desktop/sailfish/silica/theme_pixel_ratio").value().toDouble();
    //qDebug() << "ratio:" << ratio;
    if (ratio == 0) {
        qWarning() << "Pixel ratio is 0, defaulting to 1.0.";
        themeDir = SailfishApp::pathTo("images/z1.0").toString(QUrl::RemoveScheme);
    } else if (ratio == 1.0) {
        themeDir = SailfishApp::pathTo("images/z1.0").toString(QUrl::RemoveScheme);
    } else if (ratio == 1.25) {
        themeDir = SailfishApp::pathTo("images/z1.25").toString(QUrl::RemoveScheme);
    } else if (ratio == 1.5) {
        themeDir = SailfishApp::pathTo("images/z1.5").toString(QUrl::RemoveScheme);
    } else if (ratio == 1.75) {
        themeDir = SailfishApp::pathTo("images/z1.75").toString(QUrl::RemoveScheme);
    } else if (ratio == 2.0) {
        themeDir = SailfishApp::pathTo("images/z2.0").toString(QUrl::RemoveScheme);
    }

    if (!QDir(themeDir).exists()) {
        qWarning() << "Theme " + themeDir + " for ratio " + ratio + " doesn't exist!";
        themeDir = SailfishApp::pathTo("images/z1.0").toString(QUrl::RemoveScheme);
    }
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QStringList parts = id.split('?');
    QString filepath = themeDir + "/" + parts.at(0) + ".png";
    if (!QFile::exists(filepath)) {
        // Icon file is not exist -> fallback to default icon
        filepath = themeDir + "/icon-m-item.png";
    }

    QPixmap sourcePixmap(themeDir + "/" + parts.at(0) + ".png");

    if (size)
        *size  = sourcePixmap.size();

    if (parts.length() > 1)
        if (QColor::isValidColor(parts.at(1)))
        {
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
