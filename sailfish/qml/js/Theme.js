// Code heavily inspired and partially borrowed from
// harbour-webpirate project (https://github.com/Dax89/harbour-webpirate)

window.Kaktus_ThemeObject = function() {
    var result = { type: "theme_init" };
    navigator.qt.postMessage(JSON.stringify(result));
};

window.Kaktus_ThemeObject.prototype.set = function(theme) {
    for(var prop in theme)
        this[prop] = theme[prop];
    this.updateScale();
};

// Hack to fix wrong device Pixel Ratio reported by Webview (thanks to llelectronics)
window.Kaktus_ThemeObject.prototype.getPixelRatio = function() {
    if(window.screen.width <= 540) // Jolla devicePixelRatio: 1.5
        return 1.5
    if(window.screen.width > 540 && screen.width <= 768) // Nexus 4 devicePixelRatio: 2.0
        return 2.0
    if (window.screen.width > 768) // Nexus 5 devicePixelRatio: 3.0
        return 3.0
};

window.Kaktus_ThemeObject.prototype.updateScale = function() {
    var pr = this.getPixelRatio();
    var _scale = this.zoom ? this.zoom * pr : pr;
    var scale = Math.round((_scale <= 0.5 ? 0.5 : _scale ) * 10 ) / 10;
    var viewport_ele = document.querySelector("meta[name='viewport']");
    var content = "width=device-width/" + scale + ", initial-scale=" + scale;

    // console.log("viewport content: " + content);

    if (viewport_ele) {
        viewport_ele.content = content;
    } else {
        var meta_ele = document.createElement("meta");
        var name_att = document.createAttribute("name");
        name_att.value = "viewport";
        var content_att = document.createAttribute("content");
        content_att.value = content;
        meta_ele.setAttributeNode(name_att);
        meta_ele.setAttributeNode(content_att);
        document.head.appendChild(meta_ele);
    }
};

window.Kaktus_ThemeObject.prototype.apply = function() {
    var scale = this.getPixelRatio();
    var pageMargin = Math.floor(this.pageMargin / (scale * this.zoom));
    var pageMarginBottom = Math.floor(this.pageMarginBottom / (scale * this.zoom));
    var fontSize = Math.floor(this.fontSize / scale);
    var fontSizeTitle = Math.floor(this.fontSizeTitle / scale);
    //var maxWidth = Math.floor(window.screen.width / (scale * this.zoom));

    var css = "";

    if (this.theme === "dark") {
        css = "* { font-family: \"" + this.fontFamily + "\";\n" +
              "background-color: " + this.highlightDimmerColor + " !important;\n" +
              "color: " + this.primaryColor + " !important;\n }\n\n";
        css += "select { color: " + this.highlightDimmerColor + " !important; }\n";
        css += "a { color: " + this.highlightColor + " !important; }\n";
    } else if (this.theme === "light") {
        css = "* { font-family: \"" + this.fontFamily + "\";\n" +
              "background-color: " + this.secondaryColor + " !important;\n" +
              "color: " + this.highlightDimmerColor + " !important;\n }\n\n";
        css += "select { color: " + this.highlightColorDark + " !important; }\n";
        css += "a { color: " + this.highlightColorDark + " !important; }\n";
    }

    //css += "body { max-width: " + maxWidth + "px; \n" +
    css += "body { " +
            "margin: 0; \n" +
            "padding: " + pageMargin + "px " + pageMargin + "px " + pageMarginBottom + "px " + pageMargin + "px; \n" +
            "font-size: " + fontSize + "px; }\n";
    css += "img { max-width: 100% !important; max-height:device-height !important; }\n";
    css += "buttom, input, form { max-width: 100% !important; max-height:device-height !important; }\n";
    css += "a, h1, h2, h3, div, p, pre, code { word-wrap: break-word; }\n";
    css += "h1, h1, h3 { font-family: \"" + this.fontFamilyHeading + "\"; }\n";
    //css += "#_kaktus_img { margin-bottom: 10px; }\n";
    css += "#_kaktus_title { font-size: " + fontSizeTitle + "px; font-weight: bold; }";
    //css += "figure { margin: 0; padding: 0; }";

    //console.log(css);

    var style_ele = document.getElementById("_kaktus_style");
    if (style_ele) {
        style_ele.innerHTML = css;
    } else {
        style_ele = document.createElement("style");
        style_ele.id = "_kaktus_style";
        style_ele.type = "text/css";
        style_ele.appendChild(document.createTextNode(css));
        document.head.appendChild(style_ele);
    }

    this.updateScale();
}

window.Kaktus_Theme = new window.Kaktus_ThemeObject();
