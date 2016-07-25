#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QSize>
#include <QPixmap>
#include <QQuickImageProvider>
#include <QString>

class IconProvider : public QQuickImageProvider
{
public:
    IconProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QString themeDir;
};

#endif // ICONPROVIDER_H
