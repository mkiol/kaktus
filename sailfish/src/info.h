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

#ifndef INFO_H
#define INFO_H

namespace Kaktus {
static const char* APP_NAME = "Kaktus";
#ifdef SAILFISH
static const char* APP_VERSION = "3.0.0";
#elif ANDROID
static const char* APP_VERSION = "3.0.0";
#endif
static const char* AUTHOR = "Michal Kosciesza";
static const char* COPYRIGHT_YEAR = "2014-2019";
static const char* AUTHOR1 = "Renaud Casenave-Péré";
static const char* COPYRIGHT_YEAR1 = "2019";
static const char* SUPPORT_EMAIL = "kaktus@mkiol.net";
static const char* PAGE = "https://github.com/mkiol/kaktus";
static const char* LICENSE = "GNU General Public Licence version 3";
static const char* LICENSE_URL = "https://www.gnu.org/licenses/gpl-3.0.html";
}

#endif // INFO_H
