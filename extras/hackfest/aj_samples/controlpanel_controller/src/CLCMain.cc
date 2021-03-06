/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#include <iostream>
#include <sstream>
#include <cstdio>
#include <signal.h>
#include "CommandLineController.h"

CommandLineController* commandLineController = 0;

void exitApp(int32_t signum)
{
    std::cout << "Exiting..." << std::endl;

    if(commandLineController != NULL)
        commandLineController->shutdown();    
    
    std::cout << "Peace!" << std::endl;
    exit(signum);
}

int main()
{
    // Allow CTRL+C to end application
    signal(SIGINT, exitApp);
    std::cout << "====================================================\n";
    std::cout << "Control Panel Controller \n";
    std::cout << "(Press CTRL+C to end application) \n";

    commandLineController = new CommandLineController();
    commandLineController->initialize();
}

