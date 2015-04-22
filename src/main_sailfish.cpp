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
#include "netvibesfetcher.h"
#include "utils.h"
#include "settings.h"
#include "proxy.h"

static const char *APP_NAME = "Kaktus";
static const char *AUTHOR = "Michal Kosciesza <michal@mkiol.net>";
static const char *PAGE = "https://github.com/mkiol/kaktus";
#ifdef KAKTUS_LIGHT
static const char *VERSION = "1.4 (light edition)";
#else
static const char *VERSION = "1.4";
#endif

int main(int argc, char *argv[])
{
    //qmlRegisterType<Proxy>("harbour.net.mkiol.kaktus", 1, 0, "Proxy");

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

    Settings* settings = Settings::instance();

    QTranslator *appTranslator = new QTranslator;
    if (settings->getLocale() == "")
        appTranslator->load(":/i18n/kaktus_" + QLocale::system().name() + ".qm");
    else
        appTranslator->load(":/i18n/kaktus_" + settings->getLocale() + ".qm");
    app->installTranslator(appTranslator);

    settings->view = view.data();
    DatabaseManager db; settings->db = &db;
    DownloadManager dm; settings->dm = &dm;
    CacheServer cache(&db); settings->cache = &cache;
    NetvibesFetcher fetcher; settings->fetcher = &fetcher;
    Utils utils;

    QObject::connect(view->engine(), SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

    view->rootContext()->setContextProperty("db", &db);
    view->rootContext()->setContextProperty("fetcher", &fetcher);
    view->rootContext()->setContextProperty("utils", &utils);
    view->rootContext()->setContextProperty("dm", &dm);
    view->rootContext()->setContextProperty("cache", &cache);
    view->rootContext()->setContextProperty("settings", settings);

    view->setSource(SailfishApp::pathTo("qml/sailfish/main.qml"));
    view->show();

    return app->exec();
}
