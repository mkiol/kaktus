// Code borrowed from harbour-webpirate project (https://github.com/Dax89/harbour-webpirate)

console.log = function(msg) {
    var data = { type: "console_log",
                 data: { log: ((typeof msg === "object") ? msg.toString() : msg) } };

    navigator.qt.postMessage(JSON.stringify(data));
}

console.error = function(msg) {
    var data = { type: "console_error",
                 data: { log: ((typeof msg === "object") ? msg.toString() : msg) } };

    navigator.qt.postMessage(JSON.stringify(data));
}

window.open = function(url) { // Popup Blocker
    var data = { type: "window_open",
                 data: { url: url } };

    navigator.qt.postMessage(JSON.stringify(data));
}

window.onerror = function(errmsg, url, line) { // Print Javascript Errors
    if((url !== undefined) && url.length)
        console.log(url + "(" + line + "): " + errmsg);

    return false; // Ignore other errors
}
