// Code borrowed from harbour-webpirate project (https://github.com/Dax89/harbour-webpirate)

console.log = function(msg) {
  kaktusPostMessage("console_log", { log: ((typeof msg === "object") ? msg.toString() : msg) });
};

console.error = function(msg) {
  kaktusPostMessage("console_error", { log: ((typeof msg === "object") ? msg.toString() : msg) });
};
