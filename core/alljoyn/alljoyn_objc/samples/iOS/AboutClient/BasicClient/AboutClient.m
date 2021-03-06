////////////////////////////////////////////////////////////////////////////////
// Copyright AllSeen Alliance. All rights reserved.
//
//    Contributed by Qualcomm Connected Experiences, Inc.,
//    with authorization from the AllSeen Alliance, Inc.
//    
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//    
//        http://www.apache.org/licenses/LICENSE-2.0
//    
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//    
//    Pursuant to Section 1 of the License, the work of authorship constituting
//    a Work and any Contribution incorporated in the Work shall mean only that
//    Contributor's code submissions authored by that Contributor.  Any rights
//    granted under the License are conditioned upon acceptance of these
//    clarifications.
////////////////////////////////////////////////////////////////////////////////

#import "AboutClient.h"
#import "AJNBusAttachment.h"
#import "AJNInterfaceDescription.h"
#import "AJNSessionOptions.h"
#import "AJNVersion.h"
#import "BasicObject.h"
#import "AJNAboutProxy.h"
#import "AJNStatus.h"
#import "AJNAboutListener.h"

////////////////////////////////////////////////////////////////////////////////
//
// Constants
//

static NSString* kAboutClientInterfaceName = @"com.example.about.feature.interface.sample";
static NSString* kAboutClientServiceName = @"com.example.about.feature.interface.sample";
static NSString* kAboutClientServicePath = @"/example/path";
static const AJNSessionPort kAboutClientServicePort = 900;

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// About Client private interface
//

@interface AboutClient() <AJNBusListener, AJNSessionListener, AJNAboutListener>

@property (nonatomic, strong) NSCondition *receivedAnnounce;
@property (nonatomic, strong) AJNBusAttachment *bus;
@property (nonatomic, strong) NSCondition *joinedSessionCondition;
@property (nonatomic) AJNSessionId sessionId;
@property (nonatomic, strong) NSString *foundServiceName;
@property (nonatomic, strong) BasicObjectProxy *basicObjectProxy;
@property (nonatomic, strong) AJNAboutProxy *aboutProxy;
@property BOOL wasNameAlreadyFound;

- (void)run;

@end

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Basic Client implementation
//

@implementation AboutClient

@synthesize bus = _bus;
@synthesize joinedSessionCondition = _joinedSessionCondition;
@synthesize sessionId = _sessionId;
@synthesize foundServiceName = _foundServiceName;
@synthesize basicObjectProxy = _basicObjectProxy;
@synthesize wasNameAlreadyFound = _wasNameAlreadyFound;
@synthesize delegate = _delegate;
@synthesize receivedAnnounce = _receivedAnnounce;

- (void)sendHelloMessage
{
    dispatch_queue_t clientQueue = dispatch_queue_create("org.alljoyn.basic-service.clientQueue",NULL);
    dispatch_async( clientQueue, ^{
        [self run];
    });
}

- (void)run
{
    NSLog(@"AllJoyn Library version: %@", AJNVersion.versionInformation);
    NSLog(@"AllJoyn Library build info: %@\n", AJNVersion.buildInformation);
    [self.delegate didReceiveStatusUpdateMessage:AJNVersion.versionInformation];
    [self.delegate didReceiveStatusUpdateMessage:AJNVersion.buildInformation];

    NSLog(@"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    NSLog(@"+ Creating bus attachment                                                                 +");
    NSLog(@"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    
    QStatus status;
    
    // create the message bus
    //
    self.bus = [[AJNBusAttachment alloc] initWithApplicationName:@"BasicClient" allowRemoteMessages:YES];
    
    // create an interface description
    //
    AJNInterfaceDescription *interfaceDescription = [self.bus createInterfaceWithName:@"com.example.about.feature.interface.sample" enableSecurity:NO];
    
    // add the methods to the interface description
    //
    status = [interfaceDescription addMethodWithName:@"cat" inputSignature:@"ss" outputSignature:@"s" argumentNames:[NSArray arrayWithObjects:@"str1",@"str2",@"outStr", nil]];
    
    if (status != ER_OK && status != ER_BUS_MEMBER_ALREADY_EXISTS) {
        @throw [NSException exceptionWithName:@"BusObjectInitFailed" reason:@"Unable to add method to interface: cat" userInfo:nil];
    }
    
    [interfaceDescription activate];
    
    self.receivedAnnounce = [[NSCondition alloc] init];
    [self.receivedAnnounce lock];

    [self.bus registerAboutListener:self];
    // register a bus listener in order to receive discovery notifications
    //
    [self.bus registerBusListener:self];

    
    // start the message bus
    //
    status =  [self.bus start];

    if (ER_OK != status) {
        NSLog(@"Bus start failed.");
        [self.delegate didReceiveStatusUpdateMessage:@"BusAttachment::Start failed\n"];
    }
    else {
        [self.delegate didReceiveStatusUpdateMessage:@"BusAttachment started.\n"];
    }

    
    // connect to the message bus
    //
    status = [self.bus connectWithArguments:@"null:"];
    
    if (ER_OK != status) {
        NSLog(@"Bus connect failed.");
        [self.delegate didReceiveStatusUpdateMessage:@"BusAttachment::Connect(\"null:\") failed\n"];
    }
    else {
        [self.delegate didReceiveStatusUpdateMessage:@"BusAttachement connected to null:\n"];
    }

    
    // begin discovery of the well known name of the service to be called
    //
    [self.bus whoImplementsInterface:kAboutClientServiceName];
    
    [self.delegate didReceiveStatusUpdateMessage:@"Waiting to discover service...\n"];

    [self.receivedAnnounce waitUntilDate:[NSDate dateWithTimeIntervalSinceNow:60]];
    
    [self.receivedAnnounce unlock];

    [self.bus disconnectWithArguments:@"null"];
    [self.bus stop];
    
    NSLog(@"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    NSLog(@"+ Destroying bus attachment                                                               +");
    NSLog(@"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

    // deallocate the bus
    //
    self.bus = nil;
    
    [self.delegate didReceiveStatusUpdateMessage:@"Bus deallocated\n"];
}


- (void)ping
{
    if (self.bus == NULL) {
        return;
    }
    QStatus status = [self.bus pingPeer:kAboutClientServiceName withTimeout:5];
    if (status == ER_OK) {
        [self.delegate didReceiveStatusUpdateMessage:@"Ping returned Successfully"];
    } else {
        [self.delegate didReceiveStatusUpdateMessage:@"Ping Failed"];
    }
}

#pragma mark - AJNBusListener delegate methods

- (void)didReceiveAnnounceOnBus:(NSString *)busName withVersion:(uint16_t)version withSessionPort:(AJNSessionPort)port withObjectDescription:(AJNMessageArgument *)objectDescriptionArg withAboutDataArg:(AJNMessageArgument *)aboutDataArg
{
    [self.delegate didReceiveStatusUpdateMessage:@"---------------------------\n"];
    [self.delegate didReceiveStatusUpdateMessage:@"Received Annouce Signal\n"];
    [self.delegate didReceiveStatusUpdateMessage:@"---------------------------\n"];

    AJNSessionOptions *sessionOptions = [[AJNSessionOptions alloc] initWithTrafficType:kAJNTrafficMessages supportsMultipoint:NO proximity:kAJNProximityAny transportMask:kAJNTransportMaskAny];
    [self.bus enableConcurrentCallbacks];
    AJNSessionId sessionId = [self.bus joinSessionWithName:busName onPort:port withDelegate:self options:sessionOptions];
    
    // Create AboutProxy
    AJNAboutProxy *aboutProxy = [[AJNAboutProxy alloc] initWithBusAttachment:self.bus busName:busName sessionId:sessionId];
    
    // Make a call to GetAboutData and GetVersion
    uint16_t aboutVersion;
    NSMutableDictionary *aboutData;
    [aboutProxy getVersion:&aboutVersion];
    [aboutProxy getAboutDataForLanguage:@"en" usingDictionary:&aboutData];
    NSLog(@"Version %d", version);
    [self.receivedAnnounce signal];
    

}

- (void)listenerDidRegisterWithBus:(AJNBusAttachment*)busAttachment
{
    NSLog(@"AJNBusListener::listenerDidRegisterWithBus:%@",busAttachment);
}

- (void)listenerDidUnregisterWithBus:(AJNBusAttachment*)busAttachment
{
    NSLog(@"AJNBusListener::listenerDidUnregisterWithBus:%@",busAttachment);
}

- (void)didFindAdvertisedName:(NSString*)name withTransportMask:(AJNTransportMask)transport namePrefix:(NSString*)namePrefix
{
    NSLog(@"AJNBusListener::didFindAdvertisedName:%@ withTransportMask:%u namePrefix:%@", name, transport, namePrefix);
    [self.delegate didReceiveStatusUpdateMessage:@"Found advertised name\n"];

    if ([namePrefix compare:kAboutClientServiceName] == NSOrderedSame) {
        
        BOOL shouldReturn;
        @synchronized(self) {
            shouldReturn = self.wasNameAlreadyFound;
            self.wasNameAlreadyFound = true;
        }
        
        if (shouldReturn) {
            NSLog(@"Already found an advertised name, ignoring this name %@...", name);
            return;
        }
        
        // Since we are in a callback we must enable concurrent callbacks before calling a synchronous method.
        //
        [self.bus enableConcurrentCallbacks];

        self.sessionId = [self.bus joinSessionWithName:name onPort:kAboutClientServicePort withDelegate:self options:[[AJNSessionOptions alloc] initWithTrafficType:kAJNTrafficMessages supportsMultipoint:YES proximity:kAJNProximityAny transportMask:kAJNTransportMaskAny]];
        
        if (self.sessionId) {
            self.foundServiceName = name;
            
            NSLog(@"Client joined session %d", self.sessionId);
            [self.delegate didReceiveStatusUpdateMessage:[NSString stringWithFormat:@"JoinSession SUCCESS (Session id=%d)\n", self.sessionId]];
        }
        else {
            [self.delegate didReceiveStatusUpdateMessage:@"JoinSession failed\n"];            
        }
        
        [self.joinedSessionCondition signal];
    }
}

- (void)didLoseAdvertisedName:(NSString*)name withTransportMask:(AJNTransportMask)transport namePrefix:(NSString*)namePrefix
{
    NSLog(@"AJNBusListener::listenerDidUnregisterWithBus:%@ withTransportMask:%u namePrefix:%@",name,transport,namePrefix);
}

- (void)nameOwnerChanged:(NSString*)name to:(NSString*)newOwner from:(NSString*)previousOwner
{
    NSLog(@"AJNBusListener::nameOwnerChanged:%@ to:%@ from:%@", name, newOwner, previousOwner);
    [self.delegate didReceiveStatusUpdateMessage:[NSString stringWithFormat:@"NameOwnerChanged: name=%@, oldOwner=%@, newOwner=%@\n", name, previousOwner, newOwner]];    
}

- (void)busWillStop
{
    NSLog(@"AJNBusListener::busWillStop");
}

- (void)busDidDisconnect
{
    NSLog(@"AJNBusListener::busDidDisconnect");
}

#pragma mark - AJNSessionListener methods

- (void)sessionWasLost:(AJNSessionId)sessionId forReason:(AJNSessionLostReason)reason
{
    NSLog(@"AJNBusListener::sessionWasLost %u forReason:%u", sessionId, reason);
}

- (void)didAddMemberNamed:(NSString*)memberName toSession:(AJNSessionId)sessionId
{
    NSLog(@"AJNBusListener::didAddMemberNamed:%@ toSession:%u", memberName, sessionId);
}

- (void)didRemoveMemberNamed:(NSString*)memberName fromSession:(AJNSessionId)sessionId
{
    NSLog(@"AJNBusListener::didRemoveMemberNamed:%@ fromSession:%u", memberName, sessionId);
}


@end
