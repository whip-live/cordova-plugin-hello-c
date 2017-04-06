/*global cordova, module*/

module.exports = {
    getArch: function (successCallback, errorCallback) {
        cordova.exec(successCallback, errorCallback, "HelloCPlugin", "getArch", []);
    },
    hello: function (name, successCallback, errorCallback) {
        cordova.exec(successCallback, errorCallback, "HelloCPlugin", "hello", [name]);
    }
};
