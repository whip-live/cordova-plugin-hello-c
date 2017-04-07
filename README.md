cordova-plugin-hello-c
======================

A simple example of a Cordova plugin that uses pure C code.

It illustrates how to use platform-specific (either Android or iOS) C code and how to share C code cross-platform (between Android and iOS).

For Android it utilizes the Android NDK to compile architecture-specific libraries and a JNI wrapper to expose the C functions to the Java plugin API.

For iOS it uses the pure C source code in place alongside the Objective-C plugin wrapper.

# usage example

- Clone the [test project](https://github.com/dpa99c/cordova-plugin-hello-c-test)
- Add Android and iOS platforms: `cordova platform add android && cordova platform add ios`
- Run: `cordova run android` / `cordova run ios`

# Android

## Recompiling

If you modify the C source files, be sure to re-build using the ndk-build script to update the `libhelloc.so` binaries.

To do so:

- Install Android NDK as [instructed here](https://developer.android.com/ndk/guides/index.html)
- Add the NDK install path to your path environment variable
    - By default it's installed under $ANDROID_SDK_HOME/ndk-bundle
    - e.g. `export PATH=$PATH;$ANDROID_SDK_HOME/ndk-bundle`
- Set the ANDROID_NDK_HOME environment variable to your NDK install path
    - e.g. `export ANDROID_NDK_HOME=$ANDROID_SDK_HOME/ndk-bundle`
- Open terminal in plugin root folder
- Run `compile-android` (`compile-android.cmd` on Windows)

If you are editing the C source code of the plugin in place in the example project:

- Modify the C source in `plugins/cordova-plugin-hello-c/src/android/jni` or `plugins/cordova-plugin-hello-c/src/common`
- Open terminal in `plugins/cordova-plugin-hello-c`
- Run `compile-android` (`compile-android.cmd` on Windows)
- From the project root, remove and re-add the android platform to apply the plugin changes to the project
    `cordova platform rm android && cordova platform add android`

## Debugging C/C++ in Android Studio

- The Android NDK enables C/C++ source code to be debugged in Android Studio alongside Java.
- To do so, the source code must be included but **not** the compiled libraries.
- To debug this plugin in Android Studio do the following:
    - Edit `plugin.xml` and comment out the source-file lines in the PRODUCTION block which include the compiled libraries
    - Remove/re-add the plugin or Android platform in your project to update the plugin files in the Android platform project
    - Open the Android platform project (`platforms/android`) in Android Studio
    - Connect an Android device for debugging
    - Use the Project Explorer to find and open one of the `.c` source files
    - Place a breakpoint, for example on a `return` statement
    - Select "Run" > "Debug ..." from the menu
