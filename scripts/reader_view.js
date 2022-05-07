/* Copyright (C) 2022 Michal Kosciesza <michal@mkiol.net>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

function _reader_view_init() {
    var doc = document.cloneNode(true)
    var article = null

    try {
        article = new Readability(doc).parse()
    } catch (err) {}

    if (!article || article.length === 0) {
        window._reader_view_possible = false
        return false
    }

    var html = `
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Security-Policy" content="img-src data: *; media-src *; object-src 'none'" />
<meta content="text/html; charset=UTF-8" http-equiv="content-type" />
<meta name="viewport" content="width=device-width; user-scalable=0" />
<style type="text/css">
/* Avoid adding ID selector rules in this style sheet, since they could
 * inadvertently match elements in the article content. */
:root {
--body-padding: 32px;
--popup-border: rgba(0, 0, 0, 0.12);
--opaque-popup-border: #e0e0e0;
--popup-shadow: rgba(49, 49, 49, 0.3);
--grey-90-a10: rgba(12, 12, 13, 0.1);
--grey-90-a20: rgba(12, 12, 13, 0.2);
--grey-90-a30: rgba(12, 12, 13, 0.3);
--grey-90-a80: rgba(12, 12, 13, 0.8);
--grey-30: #d7d7db;
--blue-40: #45a1ff;
--blue-40-a30: rgba(69, 161, 255, 0.3);
--blue-60: #0060df;
--active-color: #0B83FF;
--font-size: 12;
--content-width: 22em;
--line-height: 1.6em;
--tooltip-background: var(--grey-90-a80);
--tooltip-foreground: white;
--toolbar-button-hover: var(--grey-90-a10);
--toolbar-button-active: var(--grey-90-a20);
}

body {
--main-background: #fff;
--main-foreground: #333;
--toolbar-border: var(--grey-90-a20);
--toolbar-box-shadow: var(--grey-90-a10);
--popup-button-hover: hsla(0,0%,70%,.4);
--popup-button-active: hsla(240,5%,5%,.15);
--popup-bgcolor: white;
--popup-button: #edecf0;
--selected-background: var(--blue-40-a30);
--selected-border: var(--blue-40);
--popup-line: var(--grey-30);
--font-value-border: var(--grey-30);
--font-color: #000000;
--icon-fill: #3b3b3c;
--icon-disabled-fill: #8080807F;
--link-foreground: var(--blue-60);
--link-selected-foreground: #333;
--visited-link-foreground: #b5007f;
/* light colours */
}

pre {
font-family: inherit;
}

body.sepia {
--main-background: #f4ecd8;
--main-foreground: #5b4636;
--toolbar-border: #5b4636;
}

body.dark {
--main-background: rgb(28, 27, 34);
--main-foreground: #eee;
--toolbar-border: #4a4a4b;
--toolbar-box-shadow: black;
--toolbar-button-hover: var(--grey-90-a30);
--toolbar-button-active: var(--grey-90-a80);
--popup-button-active: hsla(0,0%,70%,.6);
--popup-bgcolor: rgb(66,65,77);
--popup-button: #5c5c61;
--popup-line: rgb(82, 82, 94);
--selected-background: #3E6D9A;
--link-foreground: #45a1ff;
--visited-link-foreground: #e675fd;
--link-selected-foreground: #fff;
--opaque-popup-border: #434146;
--font-value-border: #656468;
--font-color: #fff;
--icon-fill: #fff;
--icon-disabled-fill: #ffffff66;
--tooltip-background: black;
--tooltip-foreground: white;
/* dark colours */
}

body {
margin: 0;
padding: var(--body-padding);
background-color: var(--main-background);
color: var(--main-foreground);
}

body.loaded {
transition: color 0.4s, background-color 0.4s;
}

body.dark *::-moz-selection {
background-color: var(--selected-background);
}

a::-moz-selection {
color: var(--link-selected-foreground);
}

body.sans-serif,
body.sans-serif .remove-button {
font-family: Helvetica, Arial, sans-serif;
}

body.serif,
body.serif .remove-button {
font-family: Georgia, "Times New Roman", serif;
}

.container {
margin: 0 auto;
font-size: var(--font-size);
max-width: var(--content-width);
line-height: var(--line-height);
}

/* Override some controls and content styles based on color scheme */

body.light > .container > .header > .domain {
border-bottom-color: #333333 !important;
}

body.sepia > .container > .header > .domain {
border-bottom-color: #5b4636 !important;
}

body.dark > .container > .header > .domain {
border-bottom-color: #eeeeee !important;
}

body.sepia > .container > .footer {
background-color: #dedad4 !important;
}

body.light blockquote {
border-inline-start: 2px solid #333333 !important;
}

body.sepia blockquote {
border-inline-start: 2px solid #5b4636 !important;
}

body.dark blockquote {
border-inline-start: 2px solid #eeeeee !important;
}

.light-button {
color: #333333;
background-color: #ffffff;
}

.dark-button {
color: #eeeeee;
background-color: #1c1b22;
}

.sepia-button {
color: #5b4636;
background-color: #f4ecd8;
}

.auto-button {
text-align: center;
}

@media (prefers-color-scheme: dark) {
.auto-button {
    background-color: #1c1b22;
    color: #eeeeee;
}
}

@media not (prefers-color-scheme: dark){
.auto-button {
    background-color: #ffffff;
    color: #333333;
}
}

/* Header */

.header {
text-align: start;
}

.domain {
font-size: 0.9em;
line-height: 1.48em;
padding-bottom: 4px;
font-family: Helvetica, Arial, sans-serif;
text-decoration: none;
border-bottom: 1px solid;
color: var(--link-foreground);
}

.header > h1 {
font-size: 1.6em;
line-height: 1.25em;
width: 100%;
margin: 30px 0;
padding: 0;
}

.header > .credits {
font-size: 0.9em;
line-height: 1.48em;
margin: 0 0 10px;
padding: 0;
font-style: italic;
}

.header > .meta-data {
font-size: 0.65em;
margin: 0 0 15px;
}

.reader-estimated-time {
text-align: match-parent;
}

/*======= Article content =======*/

/* Note that any class names from the original article that we want to match on
 * must be added to CLASSES_TO_PRESERVE in ReaderMode.jsm, so that
 * Readability.js doesnt strip them out */

.moz-reader-content {
font-size: 1em;
}

.moz-reader-content h1,
.moz-reader-content h2,
.moz-reader-content h3 {
font-weight: bold;
}

.moz-reader-content h1 {
font-size: 1.6em;
line-height: 1.25em;
}

.moz-reader-content h2 {
font-size: 1.2em;
line-height: 1.51em;
}

.moz-reader-content h3 {
font-size: 1em;
line-height: 1.66em;
}

.moz-reader-content a:link {
text-decoration: underline;
font-weight: normal;
}

.moz-reader-content a:link,
.moz-reader-content a:link:hover,
.moz-reader-content a:link:active {
color: var(--link-foreground);
}

.moz-reader-content a:visited {
color: var(--visited-link-foreground);
}

.moz-reader-content * {
max-width: 100%;
height: auto;
}

.moz-reader-content p,
.moz-reader-content p,
.moz-reader-content code,
.moz-reader-content pre,
.moz-reader-content blockquote,
.moz-reader-content ul,
.moz-reader-content ol,
.moz-reader-content li,
.moz-reader-content figure,
.moz-reader-content .wp-caption {
margin: -10px -10px 20px;
padding: 10px;
border-radius: 5px;
}

.moz-reader-content li {
margin-bottom: 0;
}

.moz-reader-content li > ul,
.moz-reader-content li > ol {
margin-bottom: -10px;
}

.moz-reader-content p > img:only-child,
.moz-reader-content p > a:only-child > img:only-child,
.moz-reader-content .wp-caption img,
.moz-reader-content figure img {
display: block;
}

.moz-reader-content img[moz-reader-center] {
margin-inline: auto;
}

.moz-reader-content .caption,
.moz-reader-content .wp-caption-text
.moz-reader-content figcaption {
font-size: 0.9em;
line-height: 1.48em;
font-style: italic;
}

.moz-reader-content code,
.moz-reader-content pre {
white-space: pre-wrap;
}

.moz-reader-content blockquote {
padding: 0;
padding-inline-start: 16px;
}

.moz-reader-content ul,
.moz-reader-content ol {
padding: 0;
}

.moz-reader-content ul {
padding-inline-start: 30px;
list-style: disc;
}

.moz-reader-content ol {
padding-inline-start: 30px;
}

table,
th,
td {
border: 1px solid currentColor;
border-collapse: collapse;
padding: 6px;
vertical-align: top;
}

table {
margin: 5px;
}

/* Visually hide (but dont display: none) screen reader elements */
.moz-reader-content .visually-hidden,
.moz-reader-content .visuallyhidden,
.moz-reader-content .sr-only {
display: inline-block;
width: 1px;
height: 1px;
margin: -1px;
overflow: hidden;
padding: 0;
border-width: 0;
}

/* Hide elements with common "hidden" class names */
.moz-reader-content .hidden,
.moz-reader-content .invisible {
display: none;
}

/* Enforce wordpress and similar emoji/smileys arent sized to be full-width,
 * see bug 1399616 for context. */
.moz-reader-content img.wp-smiley,
.moz-reader-content img.emoji {
display: inline-block;
border-width: 0;
/* height: auto is implied from .moz-reader-content * rule. */
width: 1em;
margin: 0 .07em;
padding: 0;
}

.reader-show-element {
display: initial;
}

/* Provide extra spacing for images that may be aided with accompanying element such as <figcaption> */
.moz-reader-block-img:not(:last-child) {
margin-block-end: 12px;
}

.moz-reader-wide-table {
overflow-x: auto;
display: block;
}
</style>
</head>
<body class="light">
<div class="container">
    <div class="header reader-header">
    <p id="_reader_view_site" class="domain reader-domain"></p>
    <div class="domain-border"></div>
    <h1 id="_reader_view_title" class="reader-title"></h1>
    <div class="credits reader-credits"></div>
    <div class="meta-data">
        <div class="reader-estimated-time"></div>
    </div>
    </div>
    <hr>
    <div class="content">
    <div id="_reader_view_content" class="moz-reader-content"></div>
    </div>
</div>
</body>
</html>`
    var readmode_doc = new DOMParser().parseFromString(html, 'text/html')
    var content_tag = readmode_doc.getElementById('_reader_view_content')
    var title_tag = readmode_doc.getElementById('_reader_view_title')
    var site_tag = readmode_doc.getElementById('_reader_view_site')

    content_tag.innerHTML = article.content
    title_tag.innerHTML = article.title
    site_tag.innerHTML = article.siteName

    window._reader_view_doc = readmode_doc.documentElement
    window._reader_view_origdoc = document.documentElement
    window._reader_view_possible = true
    return true
}

function _reader_view_set(enabled) {
    if (!window._reader_view_possible) return false
    if (enabled) {
        document.replaceChild(window._reader_view_doc, document.documentElement);
    } else {
        document.replaceChild(window._reader_view_origdoc, document.documentElement);
    }
    return true
}

window._reader_view_possible = false
window._reader_view_doc = null
window._reader_view_origdoc = null
window._reader_view_set = _reader_view_set
