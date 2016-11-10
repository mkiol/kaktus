// Code heavily inspired and partially borrowed from
// harbour-webpirate project (https://github.com/Dax89/harbour-webpirate)

window.Kaktus_ReaderModeHandlerObject = function() {
    this.enabled = false;

    this.orginalDoc = null;
    this.readabilityDoc = null;
    this.readabilityPossible = false;
};

window.Kaktus_ReaderModeHandlerObject.prototype.applyFiltering = function(doc, insert) {
    var elements, i;

    // kaktus img
    var img = doc.getElementById("_kaktus_img");

    // article element
    elements = doc.getElementsByTagName("article");
    if (elements.length > 0) {
        var newBody = "" + insert + (img ? img.outerHTML : "");
        for (i = 0; i < elements.length; i++) {
            newBody += "<article>" + elements[i].innerHTML + "</article>";
        }
        doc.body.innerHTML = newBody;
    }

    // width, height, target attributes
    elements = doc.querySelectorAll("[width],[height],[target],[class]");
    for (i = 0; i < elements.length; i++) {
        elements[i].removeAttribute("width");
        elements[i].removeAttribute("height");
        elements[i].removeAttribute("target");
        elements[i].removeAttribute("class");
    }

    // img max-width
    //elements = doc.getElementsByTagName("img");
    //for (i = 0; i < elements.length; i++)
    //    elements[i].style.maxWidth = "100%";


};

window.Kaktus_ReaderModeHandlerObject.prototype.check = function(data) {
    var loc = document.location;
    var uri = { "spec": loc.href,
                "host": loc.host,
                "prePath": loc.protocol + "//" + loc.host,
                "scheme": loc.protocol.substr(0, loc.protocol.indexOf(":")),
                "pathBase": loc.protocol + "//" + loc.host + loc.pathname.substr(0, loc.pathname.lastIndexOf("/") + 1) };

    var newDoc1 = document.implementation.createDocument('http://www.w3.org/1999/xhtml', 'html', null);
    var newBody1 = document.createElementNS('http://www.w3.org/1999/xhtml', 'body');
    newBody1.innerHTML = document.body.innerHTML;
    newDoc1.documentElement.appendChild(newBody1);

    var article = null;
    try {
        article = new window.Readability(uri, newDoc1).parse();
    } catch (Exception) {}

    this.readabilityPossible = article && article.length > 0 ? true : false;

    //console.log("Readability possible: " + this.readabilityPossible)

    if (this.readabilityPossible) {
        var title = article.title !== "" ? article.title : data.title;
        var insert = title === "" ? "" : "<h1 id='_kaktus_title'>" + title + "</h1>";

        var newDoc2 = document.implementation.createDocument('http://www.w3.org/1999/xhtml', 'html', null);
        var newBody2 = document.createElementNS('http://www.w3.org/1999/xhtml', 'body');
        newBody2.innerHTML = insert + article.content;
        newDoc2.documentElement.appendChild(newBody2);

        this.applyFiltering(newDoc2, insert);
        this.readabilityDoc = newDoc2.documentElement.innerHTML;
        this.orginalDoc = document.documentElement.innerHTML;

        //console.log("Readability: " + this.readabilityDoc);
    }

    var result = { type: "readability_result", data: { possible: this.readabilityPossible, enabled: this.enabled } };
    navigator.qt.postMessage(JSON.stringify(result));
};

window.Kaktus_ReaderModeHandlerObject.prototype.disable = function() {
    this.enabled = false;
    document.documentElement.innerHTML = this.orginalDoc;

    var result = { type: "readability_disabled" };
    navigator.qt.postMessage(JSON.stringify(result));
    window.Kaktus_Theme.updateScale();
};

window.Kaktus_ReaderModeHandlerObject.prototype.enable = function() {
    this.enabled = true;
    document.documentElement.innerHTML = this.readabilityDoc;
    window.Kaktus_Theme.apply();

    var result = { type: "readability_enabled" };
    navigator.qt.postMessage(JSON.stringify(result));
};

window.Kaktus_ReaderModeHandlerObject.prototype.status = function() {
    var result = { type: "readability_status", data: { enabled: this.enabled } };
    navigator.qt.postMessage(JSON.stringify(result));
};

window.Kaktus_ReaderModeHandlerObject.prototype.switchMode = function(enabled) {
    if (this.enabled === enabled)
        return;

    if (this.enabled) {
        this.disable();
        return;
    }

    if (!this.readabilityPossible) {
        return;
    }

    this.enable();
};

window.Kaktus_ReaderModeHandler = new window.Kaktus_ReaderModeHandlerObject();
