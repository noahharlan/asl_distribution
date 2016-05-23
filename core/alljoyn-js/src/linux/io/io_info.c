/**
 * @file
 */
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

#include "../../ajs.h"
#include "../../ajs_io.h"


static const AJS_IO_Info info[] = {
    {  AJS_IO_FUNCTION_DIGITAL_OUT, 1,  "LED_1",   "",  "Red LED" },
    {  AJS_IO_FUNCTION_DIGITAL_OUT, 2,  "LED_2",   "",  "Green LED" },
    {  AJS_IO_FUNCTION_DIGITAL_OUT, 3,  "LED_3",   "",  "Yellow LED" },
    {  AJS_IO_FUNCTION_DIGITAL_OUT, 4,  "LED_4",   "",  "Blue LED" },
    {  AJS_IO_FUNCTION_DIGITAL_IN,  5,  "GPIO_1",  "",  "Digital input 1" },
    {  AJS_IO_FUNCTION_DIGITAL_IN,  6,  "GPIO_2",  "",  "Digital input 2" },
    {  AJS_IO_FUNCTION_DIGITAL_IN,  7,  "GPIO_3",  "",  "Digital input 3" },
    {  AJS_IO_FUNCTION_DIGITAL_IN,  8,  "GPIO_4",  "",  "Digital input 4" },
    {  AJS_IO_FUNCTION_DIGITAL_IN,  9,  "PB_A",    "",  "Push button A" },
    {  AJS_IO_FUNCTION_DIGITAL_IN, 10,  "PB_B",    "",  "Push button B" },
    {  AJS_IO_FUNCTION_DIGITAL_IN, 11,  "PB_C",    "",  "Push button C" },
    {  AJS_IO_FUNCTION_DIGITAL_IN, 12,  "PB_D",    "",  "Push button D" }
};

uint16_t AJS_TargetIO_GetNumPins()
{
    return ArraySize(info);
}

const AJS_IO_Info* AJS_TargetIO_GetInfo(uint16_t pin)
{
    if (pin < ArraySize(info)) {
        return &info[pin];
    } else {
        return NULL;
    }
}