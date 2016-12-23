// Code heavily inspired and partially borrowed from
// harbour-webpirate project (https://github.com/Dax89/harbour-webpirate)

window.KaktusMessageListenerObject = function() {
    navigator.qt.onmessage = this.onMessage.bind(this);
};

window.KaktusMessageListenerObject.prototype.onMessage = function(message) {
    //console.log("onMessage: " + message);
    try {
   
        var obj = JSON.parse(message.data);
        var data = obj.data;

	    if(obj.type === "readability_enable")
	    	KaktusReaderMode.switchMode(true);
	    else if(obj.type === "readability_disable")
	    	KaktusReaderMode.switchMode(false);
	    else if(obj.type === "readability_check")
	    	KaktusReaderMode.check(data);
	    else if(obj.type === "readability_status")
	    	KaktusReaderMode.status();
	    else if(obj.type === "readability_apply_fixups")
	    	KaktusReaderMode.applyFixups(document);
	    else if(obj.type === "theme_set")
	    	KaktusTheme.set(data.theme);
	    else if(obj.type === "theme_update_scale")
	    	KaktusTheme.updateScale();
	    else if(obj.type === "theme_apply")
	    	KaktusTheme.apply();
        else if(obj.type === "nightmode_disable")
	    	KaktusNightMode.switchMode(false);
	    else if(obj.type === "nightmode_enable")
            KaktusNightMode.switchMode(true);
    
    } catch (err) {
    	console.error("Exception in KaktusMessageListener.onMessage: " + err);;
    }
};

window.KaktusMessageListener = new window.KaktusMessageListenerObject();
