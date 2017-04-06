cordova-plugin-hello-c
======================

A simple example of a Cordova plugin that utilizes the Android NDK

This plugin combines don/Cordova-plugin-hello and the hello-jni android NDK example to create a very simple example of a Cordova plugin that uses the android NDK.

# usage example

- Clone the [test project](https://github.com/dpa99c/cordova-plugin-hello-c-test)
- Add Android and iOS platforms: `cordova platform add android && cordova platform add ios`
- Run: `cordova run android` / `cordova run ios`

# recompiling for Android

If you modify the C source files, be sure to re-build using the ndk-build script to update the `libhelloc.so` binaries.

To do so:

- Install Android NDK as [instructed here](https://developer.android.com/ndk/guides/index.html)
- Add the NDK install path to your path environment variable
    - By default it's installed under $ANDROID_SDK_HOME/ndk-bundle
    - e.g. `export PATH=$PATH;$ANDROID_SDK_HOME/ndk-bundle`
- Set the ANDROID_NDK_HOME environment variable to your NDK install path
    - e.g. `export ANDROID_NDK_HOME=$ANDROID_SDK_HOME/ndk-bundle`
- Open terminal in `cordova-plugin-hello-c/src/android`
- Run `ndk-build` (`ndk-build.cmd` on Windows)

If you are editing the C source code of the plugin in place in the example project:

- Modify the C source in `plugins/cordova-plugin-hello-c/src/android/jni` or `plugins/cordova-plugin-hello-c/src/common`
- Open terminal in `plugins/cordova-plugin-hello-c/src/android`
- Run `ndk-build` (`ndk-build.cmd` on Windows)
- From the project root, remove and re-add the android platform to apply the plugin changes to the project
    `cordova platform rm android && cordova platform add android`

