/* Copyright (C) 2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

function _theme_init(settings) {
    window._theme_settings = settings

    var vcontent = 'width=device-width; user-scalable=0'
    var viewport = document.querySelector("meta[name='viewport']")
    if (viewport) {
        viewport.content = vcontent
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

    window._theme_possible = true
    return true
}

function _theme_set(enabled) {
    if (!window._theme_possible) return false
    var style_o = window._reader_view_possible ?
                window._reader_view_origdoc.getElementsByClassName('_theme_class').item(0) :
                document.documentElement.getElementsByClassName('_theme_class').item(0)
    var style_r = window._reader_view_possible && window._reader_view_doc ?
                window._reader_view_doc.getElementsByClassName('_theme_class').item(0) : null
    var s = window._theme_settings
    var css = enabled ? `
* {
font-family: "${s.fontFamily}"
background-color: ${s.bgColor} !important;
color: ${s.primaryColor} !important;
}
select { color: ${s.bgColor} !important; }
a { color: ${s.highlightColor} !important; }
body {
margin: 0;
background-color: ${s.bgColor} !important;
padding: ${s.pageMargin}px ${s.pageMargin}px ${s.pageMarginBottom}px ${s.pageMargin}px;
}
img { max-width: 100% !important; max-height: device-height !important; }
buttom, input, form { max-width: 100% !important; max-height: device-height !important; }
a, h1, h2, h3, div, p, pre, code { word-wrap: break-word; }
h1, h1, h3 { font-family: "${s.fontFamilyHeading}"; }
#_kaktus_img { margin-bottom: 10px; }` : ''
    if (style_o) {
        style_o.innerHTML = css
        if (style_r) style_r.innerHTML = css
    } else {
        style = document.createElement('style')
        style.className = '_theme_class'
        style.type = 'text/css'
        style.appendChild(document.createTextNode(css))
        var head_o = window._reader_view_possible ?
                    window._reader_view_origdoc.getElementsByTagName('head').item(0) : document.head
        var head_r = window._reader_view_possible ?
                    window._reader_view_doc.getElementsByTagName('head').item(0) : null
        if (head_o) head_o.appendChild(style)
        if (head_r) head_r.appendChild(style.cloneNode(true))
    }

    return true
}

window._theme_possible = false
window._theme_settings = null
window._theme_set = _theme_set
