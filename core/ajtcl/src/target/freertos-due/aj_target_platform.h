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

/******************************************************************************
 * Any time in this file there is a comment including:
    nvm_***, sysclk_***, board_***, stdio_***

 * note that the API associated with it may be subject to this Atmel license:
 * (information about it is also at www.atmel.com/asf)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN
 * NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef AJ_TARGET_PLATFORM_H_
#define AJ_TARGET_PLATFORM_H_
/*
 * Platform specific functions
 * Similar to target.h but specific to the DUE
 */
#include <FreeRTOSConfig.h>
#include "efc.h"
#include "flash_efc.h"
#include "common_nvm.h"
#include <ajtcl/aj_status.h>
#include "ioport.h"
#include "pio.h"
#include "spi.h"
#include <board.h>
#include <sysclk.h>
#include <stdio_serial.h>
#include <conf_board.h>
#include <spi.h>
#include <pio_handler.h>
#include "dmac.h"
#include "rstc.h"


// assign the WSL ISR to the interrupt slot for the correct SPI device
#define AJ_WSL_SPI_ISR SPI0_Handler

#define AJ_WSL_SPI_DEVICE SPI0
#define AJ_WSL_SPI_DEVICE_ID ID_SPI0
#define AJ_WSL_SPI_DEVICE_NPCS 0
#define AJ_WSL_SPI_PCS 0b1110
#define AJ_WSL_SPI_CHIP_PWD_PIN PIO_PB25_IDX  /* currently connected to the Reset pin on the ICSP header*/
#define AJ_WSL_SPI_CHIP_SPI_INT_PIN PIO_PC23_IDX  /* pin D7 on the Arduino Due */
#define AJ_WSL_SPI_CHIP_SPI_INT_BIT PIO_PC23
#define AJ_WSL_SPI_CHIP_POWER_PIN PIO_PC28_IDX  /* pin D3 on the Arduino Due */
#define AJ_WSL_STACK_SIZE   2000

typedef spi_status_t aj_spi_status;

#ifndef NDEBUG
#define AJ_Printf(fmat, ...) \
    do { \
        AJ_EnterCriticalRegion(); \
        printf(fmat, ## __VA_ARGS__); \
        AJ_LeaveCriticalRegion(); \
    } while (0)
#else
#define AJ_Printf(fmat, ...) \
    do { printf(fmat, ## __VA_ARGS__); } while (0)
#endif
void ASSERT(int i);


#endif /* AJ_TARGET_PLATFORM_H_ */