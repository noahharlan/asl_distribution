/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
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

#import "AJNAboutPropertyStoreImpl.h"
#import "AJNMessageArgument.h"
#import "AJNConvertUtil.h"

@interface AJNAboutPropertyStoreImpl ()

@property (nonatomic) ajn::services::AboutPropertyStoreImpl *handle;

@end

@implementation AJNAboutPropertyStoreImpl

- (void)dealloc
{
    if (self.handle) {
		delete self.handle;
		self.handle = NULL;
	}
}

- (id)initWithHandleAllocationBlock:(HandleAllocationBlock)Block
{
	self = [super init];
	if (self) {
		self.handle = Block();
	}
	return self;
}

- (id)init
{
	return [self initWithHandleAllocationBlock:
	        ^{ return new ajn::services::AboutPropertyStoreImpl; }];
}

- (QStatus)readAll:(const char *)languageTag
        withFilter:(PFilter)filter
         ajnMsgArg:(AJNMessageArgument **)all
{
	QStatus status;
	ajn::MsgArg *msgArg = new ajn::MsgArg;
    
	status = self.handle->ReadAll(languageTag, (ajn::services::AboutPropertyStoreImpl::Filter)filter, *msgArg);
    
	*all = [[AJNMessageArgument alloc] initWithHandle:msgArg];
    
	return status;
}

- (QStatus)Update:(const char *)name
      languageTag:(const char *)languageTag
        ajnMsgArg:(AJNMessageArgument *)value
{
	return ER_NOT_IMPLEMENTED;
}

- (QStatus)delete:(const char *)name
      languageTag:(const char *)languageTag
{
	return ER_NOT_IMPLEMENTED;
}

- (AJNPropertyStoreProperty *)property:(AJNPropertyStoreKey)propertyKey
{
	ajn::services::PropertyStoreProperty *psp = self.handle->getProperty((ajn::services::PropertyStoreKey)propertyKey);
    
	AJNPropertyStoreProperty *ajnPsp = [[AJNPropertyStoreProperty alloc] initWithHandle:psp];
    
	return ajnPsp;
}

- (AJNPropertyStoreProperty *)property:(AJNPropertyStoreKey)propertyKey
                          withLanguage:(NSString *)language
{
	ajn::services::PropertyStoreProperty *psp = self.handle->getProperty((ajn::services::PropertyStoreKey)propertyKey, [language UTF8String]);
    
	AJNPropertyStoreProperty *ajnPsp = [[AJNPropertyStoreProperty alloc] initWithHandle:psp];
    
	return ajnPsp;
}

- (QStatus)setDeviceId:(NSString *)deviceId
{
	return self.handle->setDeviceId([deviceId UTF8String]);
}

- (QStatus)setDeviceName:(NSString *)deviceName
{
	return self.handle->setDeviceName([deviceName UTF8String]);
}

- (QStatus)setDeviceName:(NSString *)deviceName language:(NSString *)language
{
	return self.handle->setDeviceName([deviceName UTF8String], [language UTF8String]);
}

- (QStatus)setAppId:(NSString *)appId
{
	return self.handle->setAppId([appId UTF8String]);
}

- (QStatus)setAppName:(NSString *)appName
{
	return self.handle->setAppName([appName UTF8String]);
}

- (QStatus)setDefaultLang:(NSString *)defaultLang
{
	return self.handle->setDefaultLang([defaultLang UTF8String]);
}

- (QStatus)setSupportedLangs:(NSArray *)supportedLangs
{
	std::vector <qcc::String> strings;
    
	for (NSString *str in supportedLangs) {
		qcc::String qcc_string = [str UTF8String];
		strings.push_back(qcc_string);
	}
    
	return self.handle->setSupportedLangs(strings);
}

- (QStatus)setDescription:(NSString *)description
                 language:(NSString *)language
{
	return self.handle->setDescription([description UTF8String], [language UTF8String]);
}

- (QStatus)setManufacturer:(NSString *)manufacturer
                  language:(NSString *)language
{
	return self.handle->setManufacturer([manufacturer UTF8String], [language UTF8String]);
}

- (QStatus)setDateOfManufacture:(NSString *)dateOfManufacture
{
	return self.handle->setDateOfManufacture([dateOfManufacture UTF8String]);
}

- (QStatus)setSoftwareVersion:(NSString *)softwareVersion
{
	return self.handle->setSoftwareVersion([softwareVersion UTF8String]);
}

- (QStatus)setAjSoftwareVersion:(NSString *)ajSoftwareVersion
{
	return self.handle->setAjSoftwareVersion([ajSoftwareVersion UTF8String]);
}

- (QStatus)setHardwareVersion:(NSString *)hardwareVersion
{
	return self.handle->setHardwareVersion([hardwareVersion UTF8String]);
}

- (QStatus)setModelNumber:(NSString *)modelNumber
{
	return self.handle->setModelNumber([modelNumber UTF8String]);
}

- (QStatus)setSupportUrl:(NSString *)supportUrl
{
	return self.handle->setSupportUrl([supportUrl UTF8String]);
}

- (NSString *)propertyStoreName:(AJNPropertyStoreKey)propertyStoreKey
{
	qcc::String str = self.handle->getPropertyStoreName((ajn::services::PropertyStoreKey)propertyStoreKey);
    
	return [AJNConvertUtil convertQCCStringtoNSString:str];
}

- (QStatus)reset
{
	return self.handle->Reset();
}

- (QStatus)updatePropertyName:(NSString *)name
               andLanguageTag:(NSString *)languageTag
                     andValue:(NSMutableArray **)value
{
	return ER_NOT_IMPLEMENTED;
}

- (QStatus)deletePropertyName:(NSString *)name
               andLanguageTag:(NSString *)languageTag
{
	return self.handle->Delete([name UTF8String], [languageTag UTF8String]);
}

- (ajn::services::AboutPropertyStoreImpl *)getHandle
{
	return self.handle;
}

@end