#import "HelloCPlugin.h"
#include "c_getArch.h"
#include "hello.h"
@implementation HelloCPlugin

- (void)getArch:(CDVInvokedUrlCommand*)command
{
    NSString* msg = [NSString stringWithFormat: @"iOS %s", getCPUArch()];

    CDVPluginResult* result = [CDVPluginResult
                               resultWithStatus:CDVCommandStatus_OK
                               messageAsString:msg];

    [self.commandDelegate sendPluginResult:result callbackId:command.callbackId];
}

- (void)hello:(CDVInvokedUrlCommand*)command
{
    //c_hello();
    NSString* input = [[command arguments] objectAtIndex:0];
    char* c_input = strdup([input UTF8String]);
    NSString* output = [NSString stringWithFormat: @"iOS says: %s", c_hello(c_input)];

    CDVPluginResult* result = [CDVPluginResult
                               resultWithStatus:CDVCommandStatus_OK
                               messageAsString:output];

    [self.commandDelegate sendPluginResult:result callbackId:command.callbackId];
}

@end
