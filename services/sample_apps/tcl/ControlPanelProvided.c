/******************************************************************************
 * Copyright (c) 2013 - 2014, AllSeen Alliance. All rights reserved.
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

/**
 * Per-module definition of the current module for debug logging.  Must be defined
 * prior to first inclusion of aj_debug.h.
 * The corresponding flag dbgAJSVCAPP is defined in the containing sample app.
 */
#define AJ_MODULE AJSVCAPP
#include <aj_debug.h>

#include "ControlPanelProvided.h"
#include "ControlPanelGenerated.h"
#include "alljoyn/controlpanel/Common/BaseWidget.h"
#include <alljoyn/controlpanel/Definitions.h>

#ifndef snprintf
#include <stdio.h>
#endif

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

static uint16_t uint16Var = 0;
static int16_t int16Var = 0;

static uint32_t uint32Var = 0;
static int32_t int32Var = 0;

static uint64_t uint64Var = 0;
static int64_t int64Var = 0;

static double doubleVar = 0;

static char initialString[100] = "75 F";
static char* stringVar = initialString;

static const char* sampleString = "This is a test";
static const char* sampleUrlString = "www.ControlPanelTest.com";

static DatePropertyValue date = { .fullYear = 2006, .month = 6, .mDay = 13 };
static TimePropertyValue time = { .hour = 18, .minute = 0, .second = 0 };

void addDismissSignal(ExecuteActionContext* context, int32_t dismissSignal)
{
    context->numSignals = 1;
    context->signals[0].signalId = dismissSignal;
    context->signals[0].signalType = SIGNAL_TYPE_DISMISS;
}

const char* getUrlString()
{
    return sampleUrlString;
}

void* getDateProperty()
{
    return &date;
}

void setDateProperty(DatePropertyValue* datePropertyValue)
{
    date.fullYear = datePropertyValue->fullYear;
    date.month = datePropertyValue->month;
    date.mDay = datePropertyValue->mDay;
}

void* getTimeProperty()
{
    return &time;
}
void setTimeProperty(TimePropertyValue* timePropertyValue)
{
    time.hour = timePropertyValue->hour;
    time.minute = timePropertyValue->minute;
    time.second = timePropertyValue->second;
}

uint8_t getEnabledFunc()
{
    return TRUE;
}
uint8_t getWriteableFunc()
{
    return TRUE;
}

const char* getTestString(uint16_t language)
{
    return sampleString;
}

void* getuint16Var()
{
    return &uint16Var;
}

void setuint16Var(uint16_t newuint16Var)
{
    uint16Var = newuint16Var;
}

void* getint16Var()
{
    return &int16Var;
}

void setint16Var(int16_t newint16Var)
{
    int16Var = newint16Var;
}

void* getuint32Var()
{
    return &uint32Var;
}

void setuint32Var(uint32_t newuint32Var)
{
    uint32Var = newuint32Var;
}

void* getint32Var()
{
    return &int32Var;
}

void setint32Var(int32_t newint32Var)
{
    int32Var = newint32Var;
}

void* getuint64Var()
{
    return &uint64Var;
}

void setuint64Var(uint64_t newuint64Var)
{
    uint64Var = newuint64Var;
}

void* getint64Var()
{
    return &int64Var;
}

void setint64Var(int64_t newint64Var)
{
    int64Var = newint64Var;
}

void* getdoubleVar()
{
    return &doubleVar;
}

void setdoubleVar(double newdoubleVar)
{
    doubleVar = newdoubleVar;
}

void* getStringVar()
{
    return &stringVar;
}

void setStringVar(const char* newStringVar)
{
    strncpy(stringVar, newStringVar, 99);
    stringVar[99] = '\0';
}

/* For Oven.XML */

static int temperature = 100;
static int program = 0;
static const char* programString = "Program";

void startOven()
{
    AJ_InfoPrintf(("**************** Starting the Oven ****************\n"));
}

void stopOven()
{
    AJ_InfoPrintf(("**************** Stopping the Oven ****************\n"));
}

void*getTemperature()
{
    return &temperature;
}

void setTemperature(uint16_t t)
{
    temperature = t;
}

void*getProgram()
{
    return &program;
}

void setProgram(uint16_t p)
{
    program = p;
}

const char* getProgramString(uint16_t language)
{
    return programString;
}
