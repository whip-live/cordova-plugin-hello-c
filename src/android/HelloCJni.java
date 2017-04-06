package com.example;

public class HelloCJni {

    // C-function interface
    public static native String hello(String input);
    public static native String getArch();

    // load library
    static {
        System.loadLibrary("helloc");
    }
}
