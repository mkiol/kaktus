/* Copyright (C) 2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

function _zoom_init() {
    var vcontent = 'width=device-width, initial-scale=1.0'
    var viewport_o = window._reader_view_possible ?
                window._reader_view_origdoc.querySelector("meta[name='viewport']") :
                document.querySelector("meta[name='viewport']")
    var viewport_r = window._reader_view_possible && window._reader_view_doc ?
                window._reader_view_doc.querySelector("meta[name='viewport']") :
                null
    if (viewport_o) {
        if (viewport_o) viewport_o.content = vcontent
        if (viewport_r) viewport_r.content = vcontent
    } else {
        var meta = document.createElement("meta")
        var name = document.createAttribute("name")
        name.value = "viewport"
        var content = document.createAttribute("content")
        content.value = vcontent
        meta.setAttributeNode(name)
        meta.setAttributeNode(content)
        var head_o = window._reader_view_possible ?
                    window._reader_view_origdoc.getElementsByTagName('head').item(0) : document.head
        var head_r = window._reader_view_possible ?
                    window._reader_view_doc.getElementsByTagName('head').item(0) : null
        if (head_o) head_o.appendChild(meta)
        if (head_r) head_r.appendChild(meta.cloneNode(true))
    }

    window._zoom_possible = true
    return true
}

function _zoom_set(zoom, init) {
    if (!window._zoom_possible) return false

    var vcontent = `initial-scale=${zoom}`
    if (init) vcontent += `, maximum-scale=${zoom}`

    var viewport_o = window._reader_view_possible ?
                window._reader_view_origdoc.querySelector("meta[name='viewport']") :
                document.querySelector("meta[name='viewport']")
    var viewport_r = window._reader_view_possible && window._reader_view_doc ?
                window._reader_view_doc.querySelector("meta[name='viewport']") :
                null

    if (viewport_o) viewport_o.content = vcontent
    if (viewport_r) viewport_r.content = vcontent

    return true
}

function _zoom_set2(zoom) {
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
        var css = `html *:not(h1) { font-size: ${zoom} !important; }`
        if (style_o) {
            style_o.innerHTML = css
            if (style_r) style_r.innerHTML = css
        } else {
            var style = document.createElement('style')
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
