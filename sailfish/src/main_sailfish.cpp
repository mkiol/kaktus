/*
  Copyright (C) 2014-2016 Michal Kosciesza <michal@mkiol.net>

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

#include <QGuiApplication>
#include <QScopedPointer>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickView>
#include <sailfishapp.h>
#include <QtDebug>
#include <QTranslator>

#include "iconprovider.h"
#include "nviconprovider.h"
#include "databasemanager.h"
#include "downloadmanager.h"
#include "cacheserver.h"
#include "utils.h"
#include "settings.h"
#include "networkaccessmanagerfactory.h"

static const char *APP_NAME = "Kaktus";
static const char *AUTHOR = "Michal Kosciesza <michal@mkiol.net>";
static const char *PAGE = "https://github.com/mkiol/kaktus";
#ifdef KAKTUS_LIGHT
static const char *VERSION = "2.5.0 (light edition)";
#else
static const char *VERSION = "2.5.0";
#endif

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    app->setApplicationDisplayName(APP_NAME);
    app->setApplicationVersion(VERSION);

    view->rootContext()->setContextProperty("APP_NAME", APP_NAME);
    view->rootContext()->setContextProperty("VERSION", VERSION);
    view->rootContext()->setContextProperty("AUTHOR", AUTHOR);
    view->rootContext()->setContextProperty("PAGE", PAGE);

    view->engine()->addImageProvider(QLatin1String("icons"), new IconProvider);
    view->engine()->addImageProvider(QLatin1String("nvicons"), new NvIconProvider);

    qRegisterMetaType<DatabaseManager::CacheItem>("CacheItem");
    //qmlRegisterMetaType<QQmlChangeSet>()

    Settings* settings = Settings::instance();

    QTranslator translator;
    QString locale = settings->getLocale() == "" ? QLocale::system().name() : settings->getLocale();
    if(!translator.load(locale, "kaktus", "_", SailfishApp::pathTo("translations").toLocalFile(), ".qm")) {
        qDebug() << "Couldn't load translation for locale " + locale + " from " + SailfishApp::pathTo("translations").toLocalFile();
    }
    app->installTranslator(&translator);

    settings->view = view.data();
    DatabaseManager db; settings->db = &db;
    DownloadManager dm; settings->dm = &dm;
    CacheServer cache(&db); settings->cache = &cache;

    Utils utils;

    QObject::connect(view->engine(), SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

    NetworkAccessManagerFactory NAMfactory(settings->getDmUserAgent());
    view->engine()->setNetworkAccessManagerFactory(&NAMfactory);

    view->rootContext()->setContextProperty("db", &db);
    view->rootContext()->setContextProperty("utils", &utils);
    view->rootContext()->setContextProperty("dm", &dm);
    view->rootContext()->setContextProperty("cache", &cache);
    view->rootContext()->setContextProperty("settings", settings);

    view->setSource(SailfishApp::pathTo("qml/main.qml"));
    view->show();

    return app->exec();
}
