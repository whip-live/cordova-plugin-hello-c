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
    NSString* name = [[command arguments] objectAtIndex:0];
    NSString* msg = [NSString stringWithFormat: @"Hello %@. iOS says: %s", name, c_hello()];

    CDVPluginResult* result = [CDVPluginResult
                               resultWithStatus:CDVCommandStatus_OK
                               messageAsString:msg];

    [self.commandDelegate sendPluginResult:result callbackId:command.callbackId];
}

@end
