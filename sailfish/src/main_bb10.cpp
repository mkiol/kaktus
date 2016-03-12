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

#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/device/DisplayInfo>

#include <Qt/qdeclarativedebug.h>
#include <QtGui/QFileSystemModel>
#include <QLocale>
#include <QTranslator>
#include <QTimer>
#include <QDebug>

#include "databasemanager.h"
#include "downloadmanager.h"
#include "cacheserver.h"
#include "utils.h"
#include "settings.h"
#include "networkaccessmanagerfactory.h"
#include "abstractitemmodel.hpp"
#include "webimageview.h"

using namespace bb::cascades;

#ifdef KAKTUS_LIGHT
static const char *VERSION = "2.3 (light edition)";
#else
static const char *VERSION = "2.3";
#endif
static const char *AUTHOR = "Michal Kosciesza <michal@mkiol.net>";
static const char *PAGE = "https://github.com/mkiol/kaktus";
static const char *APP_NAME = "Kaktus";

Q_DECL_EXPORT int main(int argc, char **argv)
{
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterType<QTimer>("net.mkiol.kaktus", 1, 0, "QTimer");
    qmlRegisterType <AbstractItemModel> ("com.kdab.components", 1, 0, "AbstractItemModel");
    qmlRegisterType <WebImageView> ("org.labsquare", 1, 0, "WebImageView");
    qRegisterMetaType < DatabaseManager::CacheItem > ("CacheItem");

    Application app(argc, argv);

    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(VERSION);

    Settings* settings = Settings::instance();

    QTranslator translator;
#ifdef KAKTUS_LIGHT
    const QString filename = QString::fromLatin1("kaktuslight_%1").arg(
            settings->getLocale()=="" ? QLocale().name() : settings->getLocale());
#else
    const QString filename = QString::fromLatin1("kaktus_%1").arg(
            settings->getLocale()=="" ? QLocale().name() : settings->getLocale());
#endif
    if (translator.load(filename, "app/native/qm"))
        app.installTranslator(&translator);

    settings->qml = QmlDocument::create("asset:///main.qml");

    settings->qml->documentContext()->setContextProperty("APP_NAME", APP_NAME);
    settings->qml->documentContext()->setContextProperty("VERSION", VERSION);
    settings->qml->documentContext()->setContextProperty("AUTHOR", AUTHOR);
    settings->qml->documentContext()->setContextProperty("PAGE", PAGE);

    NetworkAccessManagerFactory factory(settings->getDmUserAgent());
    settings->qml->defaultDeclarativeEngine()->setNetworkAccessManagerFactory(&factory);

    DatabaseManager db;
    settings->db = &db;
    DownloadManager dm;
    settings->dm = &dm;
    CacheServer cache(&db);
    settings->cache = &cache;
    Utils utils;
    bb::device::DisplayInfo display;

    QFileSystemModel *model = new QFileSystemModel(&app);
    model->setRootPath("app/");

    settings->qml->setContextProperty("db", &db);
    settings->qml->setContextProperty("utils", &utils);
    settings->qml->setContextProperty("dm", &dm);
    settings->qml->setContextProperty("cache", &cache);
    settings->qml->setContextProperty("settings", settings);
    settings->qml->setContextProperty("_fileSystemModel", model);
    settings->qml->setContextProperty("display", &display);

    QObject::connect(settings->qml->defaultDeclarativeEngine(), SIGNAL(quit()),
            QCoreApplication::instance(), SLOT(quit()));
    AbstractPane *root = settings->qml->createRootObject<AbstractPane>();
    Application::instance()->setScene(root);

    return Application::exec();
}
