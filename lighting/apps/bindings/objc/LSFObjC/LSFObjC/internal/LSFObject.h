/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
 *
 *    Contributed by Qualcomm Connected Experiences, Inc.,
 *    with authorization from the AllSeen Alliance, Inc.
 *    
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *    
 *        http://www.apache.org/licenses/LICENSE-2.0
 *    
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *    
 *    Pursuant to Section 1 of the License, the work of authorship constituting
 *    a Work and any Contribution incorporated in the Work shall mean only that
 *    Contributor's code submissions authored by that Contributor.  Any rights
 *    granted under the License are conditioned upon acceptance of these
 *    clarifications.
 ******************************************************************************/

#import <Foundation/Foundation.h>
#import "LSFHandle.h"

/**
 *  The base class for all AllJoyn API objects
 */
@interface LSFObject : NSObject<LSFHandle>

/** A handle to the C++ API object associated with this objective-c class */
@property (nonatomic, assign) LSFHandle handle;

/** 
 * Initialize the API object
 *
 * @param handle The handle to the C++ API object associated with this objective-c API object
 */
-(id)initWithHandle: (LSFHandle)handle;

/** 
 * Initialize the API object
 *
 * @param handle The handle to the C++ API object associated with this objective-c API object.
 * @param deletionFlag A flag indicating whether or not the objective-c class should call delete on the handle when dealloc is called.
 */
-(id)initWithHandle: (LSFHandle)handle shouldDeleteHandleOnDealloc: (BOOL)deletionFlag;

@end
