/* Copyright (C) 2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

function _zoom_init() {
    window._zoom_possible = true
    return true
}

function _zoom_set(zoom) {
    if (!window._zoom_possible) return false
    var style_o = window._reader_view_possible ?
                window._reader_view_origdoc.getElementsByClassName('_zoom_class').item(0) :
                document.documentElement.getElementsByClassName('_zoom_class').item(0)
    var style_r = window._reader_view_possible && window._reader_view_doc ?
                window._reader_view_doc.getElementsByClassName('_zoom_class').item(0) : null
    if (zoom === '100%') {
        if (style_o) style_o.innerHTML = ''
        if (style_r) style_r.innerHTML = ''
    } else {
        var css = 'html *:not(h1) { font-size: ' + zoom + ' !important; }'
        if (style_o) {
            style_o.innerHTML = css
            if (style_r) style_r.innerHTML = css
        } else {
            style = document.createElement('style')
            style.className = '_zoom_class'
            style.type = 'text/css'
            style.appendChild(document.createTextNode(css))
            var head_o = window._reader_view_possible ?
                        window._reader_view_origdoc.getElementsByTagName('head').item(0) : document.head
            var head_r = window._reader_view_possible ?
                        window._reader_view_doc.getElementsByTagName('head').item(0) : null
            if (head_o) head_o.appendChild(style)
            if (head_r) head_r.appendChild(style.cloneNode(true))
        }
    }
    return true
}

window._zoom_possible = false
window._zoom_set = _zoom_set
