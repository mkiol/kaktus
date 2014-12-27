/*
  Copyright (C) 2014 Michal Kosciesza <michal@mkiol.net>

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

#ifndef STANDARDWEBLISTITEM_H_
#define STANDARDWEBLISTITEM_H_

#include <bb/cascades/StandardListItem>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QUrl>

using namespace bb::cascades;

class StandardWebListItem: public bb::cascades::StandardListItem
{
    Q_OBJECT
    Q_PROPERTY (QUrl url READ getUrl WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY (float loading READ getLoading NOTIFY loadingChanged)

public:
    StandardWebListItem();
    virtual ~StandardWebListItem();
    const QUrl& getUrl() const;
    double getLoading() const;

public Q_SLOTS:
    void setUrl(const QUrl& url);
    void clearCache();

private Q_SLOTS:
    void imageLoaded();
    void dowloadProgressed(qint64,qint64);

signals:
    void urlChanged();
    void loadingChanged();

private:
    static QNetworkAccessManager * nm;
    static QNetworkDiskCache * nc;
    QUrl url;
    float loading;

    bool isARedirectedUrl(QNetworkReply *reply);
    void setURLToRedirectedUrl(QNetworkReply *reply);
};

#endif /* STANDARDWEBLISTITEM_H_ */
