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


function getHumanFriendlyTimeString(date) {
    var delta = Math.floor(Date.now()/1000-date);
    if (delta===0) {
        return "just now";
    }
    if (delta===1) {
        return "1 second ago";
    }
    if (delta<60) {
        return "" + delta + " seconds ago";
    }
    if (delta>=60&&delta<120) {
        return "1 minute ago";
    }
    if (delta<3600) {
        return "" + Math.floor(delta/60) + " minutes ago";
    }
    if (delta>=3600&&delta<7200) {
        return "1 hour ago";
    }
    if (delta<86400) {
        return "" + Math.floor(delta/3600) + " hours ago";
    }
    if (delta>=86400&&delta<172800) {
        return "yesterday";
    }
    if (delta<604800) {
        return "" + Math.floor(delta/86400) + " days ago";
    }
    if (delta>=604800&&delta<1209600) {
        return "1 week ago";
    }
    if (delta<2419200) {
        return "" + Math.floor(delta/604800) + " weeks ago";
    }
    return Qt.formatDateTime(new Date(date*1000),"dddd, d MMMM yy");
}
