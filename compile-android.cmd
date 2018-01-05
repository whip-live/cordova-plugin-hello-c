@echo off
cd src\android
cp jni\Android.mk.windows jni\Android.mk
ndk-build.cmd
