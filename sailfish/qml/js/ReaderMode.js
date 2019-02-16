// Code heavily inspired and partially borrowed from
// harbour-webpirate project (https://github.com/Dax89/harbour-webpirate)

window.KaktusReaderModeObject = function() {
    this.enabled = false;
    this.orginalDoc = null;
    this.readabilityDoc = null;
    this.readabilityPossible = false;
    kaktusPostMessage("readability_" + (this.enabled ? "enabled" : "disabled"));
};

window.KaktusReaderModeObject.prototype.applyFixups = function(doc) {
    var elements, i;

    // target attributess
    elements = doc.querySelectorAll("[target]");
    for (i = 0; i < elements.length; i++) {
        elements[i].removeAttribute("target");
    }

    // reddit fixup
    var host = doc.location.host;
    if (host.indexOf("reddit.com") !== -1) {
        var bar = document.querySelector("div.side");
        if (bar)
            bar.parentNode.removeChild(bar)
    }
};

window.KaktusReaderModeObject.prototype.applyFiltering = function(doc, insert) {
    var elements, i;
    
    try {
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
	
	    // width, height, target, class attributes
	    elements = doc.querySelectorAll("[max-width],[min-width],[width],[height],[target],[class]");
	    for (i = 0; i < elements.length; i++) {
	    	elements[i].removeAttribute("max-width");
	    	elements[i].removeAttribute("min-width");
	        elements[i].removeAttribute("width");
	        elements[i].removeAttribute("height");
	        elements[i].removeAttribute("target");
	        elements[i].removeAttribute("class");
	    }
	
	    // img max-width
	    //elements = doc.getElementsByTagName("img");
	    //for (i = 0; i < elements.length; i++)
	    //    elements[i].style.maxWidth = "100%";
    } catch (err) {
    	console.error("Exception in KaktusReaderMode.applyFiltering: " + err);
    }
};

window.KaktusReaderModeObject.prototype.check = function(data) {
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
    } catch (err) {
    	console.error("Exception in KaktusReaderMode.check: " + err);
    }

    this.readabilityPossible = article && article.length > 0 ? true : false;

    //console.log("Readability possible: " + this.readabilityPossible);

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

    kaktusPostMessage("readability_result", { possible: this.readabilityPossible, enabled: this.enabled });
};

window.KaktusReaderModeObject.prototype.disable = function() {
    this.enabled = false;
    document.documentElement.innerHTML = this.orginalDoc;
    kaktusPostMessage("readability_disabled");
    window.KaktusTheme.updateScale();
};

window.KaktusReaderModeObject.prototype.enable = function() {
    this.enabled = true;
    document.documentElement.innerHTML = this.readabilityDoc;
    window.KaktusTheme.apply();
    kaktusPostMessage("readability_enabled");
};

window.KaktusReaderModeObject.prototype.status = function() {
    kaktusPostMessage("readability_status", { enabled: this.enabled });
};

window.KaktusReaderModeObject.prototype.switchMode = function(enabled) {
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

window.KaktusReaderMode = new window.KaktusReaderModeObject();
