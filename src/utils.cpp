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

//#include <QtDBus/QtDBus>
//#include <QDateTime>
#include <QtCore/qmath.h>

#include "utils.h"

Utils::Utils(QObject *parent) :
    QObject(parent)
{
    dashboardModel = NULL;
    entryModel = NULL;
    tabModel = NULL;
    feedModel = NULL;
}

void Utils::copyToClipboard(const QString &text)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QClipboard *clipboard = QGuiApplication::clipboard();
#else
    QClipboard *clipboard = QApplication::clipboard();
#endif
    clipboard->setText(text);
}

void Utils::setTabModel(const QString &dashboardId)
{
    TabModel *oldTabModel = tabModel;
    Settings *s = Settings::instance();

    tabModel = new TabModel(s->db);
    tabModel->init(dashboardId);

    s->view->rootContext()->setContextProperty("tabModel", tabModel);

    if (oldTabModel != NULL)
        delete oldTabModel;
}

void Utils::setFeedModel(const QString &tabId)
{
    FeedModel* oldFeedModel = feedModel;
    Settings *s = Settings::instance();

    feedModel = new FeedModel(s->db);
    feedModel->init(tabId);

    s->view->rootContext()->setContextProperty("feedModel", feedModel);

    if (oldFeedModel != NULL)
        delete oldFeedModel;
}

void Utils::setEntryModel(const QString &feedId)
{
    EntryModel* oldEntryModel = entryModel;
    Settings *s = Settings::instance();

    entryModel = new EntryModel(s->db);
    entryModel->init(feedId);

    s->view->rootContext()->setContextProperty("entryModel", entryModel);

    if (oldEntryModel != NULL)
        delete oldEntryModel;
}

void Utils::setDashboardModel()
{
    DashboardModel* oldDashboardModel = dashboardModel;
    Settings *s = Settings::instance();

    dashboardModel = new DashboardModel(s->db);
    dashboardModel->init();

    s->view->rootContext()->setContextProperty("dashboardModel", dashboardModel);

    if (oldDashboardModel != NULL)
        delete oldDashboardModel;
}

void Utils::updateModels()
{
    if (dashboardModel != NULL)
        dashboardModel->init();

    if (tabModel != NULL)
        tabModel->init();

    if (feedModel != NULL)
        feedModel->init();

    if (entryModel != NULL)
        entryModel->init();
}

Utils::~Utils()
{
    if (entryModel != NULL)
        delete entryModel;

    if (feedModel != NULL)
        delete feedModel;

    if (tabModel != NULL)
        delete tabModel;

    if (dashboardModel != NULL)
        delete dashboardModel;
}

QList<QString> Utils::dashboards()
{
    Settings *s = Settings::instance();
    QList<QString> simpleList;
    QList<DatabaseManager::Dashboard> list = s->db->readDashboards();
    QList<DatabaseManager::Dashboard>::iterator i = list.begin();
    while (i != list.end()) {
        simpleList.append((*i).title);
        ++i;
    }
    return simpleList;
}

QString Utils::defaultDashboardName()
{
    Settings *s = Settings::instance();
    DatabaseManager::Dashboard d = s->db->readDashboard(s->getNetvibesDefaultDashboard());
    return d.title;
}

/*bool Utils::showNotification(const QString previewSummary,
                             const QString previewBody,
                             const QString icon)
{
    QVariantMap hints;
    hints.insert("category", "net.mkiol.kaktus.notification");
    hints.insert("x-nemo-timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
    hints.insert("x-nemo-preview-body", previewBody);
    hints.insert("x-nemo-preview-summary", previewSummary);
    hints.insert("x-nemo-item-count", 1);

    QList<QVariant> argumentList;
    argumentList << "Kaktus";
    argumentList << (uint)0;
    argumentList << icon;
    argumentList << "";
    argumentList << "";
    argumentList << QStringList();
    argumentList << hints;
    argumentList << (int)5000;

    static QDBusInterface notifyApp("org.freedesktop.Notifications",
                                    "/org/freedesktop/Notifications",
                                    "org.freedesktop.Notifications");
    QDBusMessage reply = notifyApp.callWithArgumentList(QDBus::AutoDetect,
                                                        "Notify", argumentList);

    if(reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "D-Bus Error:" << reply.errorMessage();
        return false;
    }
    return true;
}*/

QString Utils::getHumanFriendlyTimeString(int date)
{
    int delta = QDateTime::currentDateTime().toTime_t()-date;

    if (delta==0) {
        return tr("just now");
    }
    if (delta==1) {
        return tr("1 second ago");
    }
    if (delta<5) {
        return tr("%1 seconds ago","less than 5 seconds").arg(delta);
    }
    if (delta<60) {
        return tr("%1 seconds ago","more or equal 5 seconds").arg(delta);
    }
    if (delta<120) {
        return tr("1 minute ago");
    }
    if (delta<300) {
        return tr("%1 minutes ago","less than 5 minutes").arg(qFloor(delta/60));
    }
    if (delta<3600) {
        return tr("%1 minutes ago","more or equal 5 minutes").arg(qFloor(delta/60));
    }
    if (delta<7200) {
        return tr("1 hour ago");
    }
    if (delta<18000) {
        return tr("%1 hours ago","less than 5 hours").arg(qFloor(delta/3600));
    }
    if (delta<86400) {
        return tr("%1 hours ago","more or equal 5 hours").arg(qFloor(delta/3600));
    }
    if (delta<172800) {
        return tr("yesterday");
    }
    if (delta<432000) {
        return tr("%1 days ago","less than 5 days").arg(qFloor(delta/86400));
    }
    if (delta<604800) {
        return tr("%1 days ago","more or equal 5 days").arg(qFloor(delta/86400));
    }
    if (delta<1209600) {
        return tr("1 week ago");
    }
    if (delta<2419200) {
        return tr("%1 weeks ago").arg(qFloor(delta/604800));
    }
    QDateTime d; d.setTime_t(date);
    return d.toString("dddd, d MMMM yy");
}
