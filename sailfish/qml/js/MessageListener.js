// Code heavily inspired and partially borrowed from
// harbour-webpirate project (https://github.com/Dax89/harbour-webpirate)

window.Kaktus_MessageListenerObject = function() {
    navigator.qt.onmessage = this.onMessage.bind(this);
};

window.Kaktus_MessageListenerObject.prototype.onMessage = function(message) {
    var obj = JSON.parse(message.data);
    var data = obj.data;

    if(obj.type === "readermodehandler_enable")
        Kaktus_ReaderModeHandler.switchMode(true);
    else if(obj.type === "readermodehandler_disable")
        Kaktus_ReaderModeHandler.switchMode(false);
    else if(obj.type === "readermodehandler_check")
        Kaktus_ReaderModeHandler.check(data);
    else if(obj.type === "readermodehandler_status")
        Kaktus_ReaderModeHandler.status();
    else if(obj.type === "theme_set")
        Kaktus_Theme.set(data.theme);
    else if(obj.type === "theme_update_scale")
        Kaktus_Theme.updateScale();
    else if(obj.type === "theme_apply")
        Kaktus_Theme.apply();
};

window.Kaktus_MessageListener = new window.Kaktus_MessageListenerObject();
