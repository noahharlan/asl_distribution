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

#ifdef LSF_BINDINGS
#include <lsf/controllerservice/DeviceIcon.h>
#else
#include <DeviceIcon.h>
#endif

namespace lsf {

OPTIONAL_NAMESPACE_CONTROLLER_SERVICE

const qcc::String DeviceIconURL = "";

const qcc::String DeviceIconMimeType = "image/png";

uint8_t DeviceIcon[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44
    , 0x52, 0x00, 0x00, 0x00, 0x4A, 0x00, 0x00, 0x00, 0x75, 0x08, 0x06, 0x00, 0x00, 0x00, 0xE9
    , 0xFC, 0x77, 0x09, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6F, 0x66, 0x74
    , 0x77, 0x61, 0x72, 0x65, 0x00, 0x41, 0x64, 0x6F, 0x62, 0x65, 0x20, 0x49, 0x6D, 0x61, 0x67
    , 0x65, 0x52, 0x65, 0x61, 0x64, 0x79, 0x71, 0xC9, 0x65, 0x3C, 0x00, 0x00, 0x03, 0x66, 0x69
    , 0x54, 0x58, 0x74, 0x58, 0x4D, 0x4C, 0x3A, 0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x64, 0x6F, 0x62
    , 0x65, 0x2E, 0x78, 0x6D, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x3F, 0x78, 0x70, 0x61
    , 0x63, 0x6B, 0x65, 0x74, 0x20, 0x62, 0x65, 0x67, 0x69, 0x6E, 0x3D, 0x22, 0xEF, 0xBB, 0xBF
    , 0x22, 0x20, 0x69, 0x64, 0x3D, 0x22, 0x57, 0x35, 0x4D, 0x30, 0x4D, 0x70, 0x43, 0x65, 0x68
    , 0x69, 0x48, 0x7A, 0x72, 0x65, 0x53, 0x7A, 0x4E, 0x54, 0x63, 0x7A, 0x6B, 0x63, 0x39, 0x64
    , 0x22, 0x3F, 0x3E, 0x20, 0x3C, 0x78, 0x3A, 0x78, 0x6D, 0x70, 0x6D, 0x65, 0x74, 0x61, 0x20
    , 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x78, 0x3D, 0x22, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x3A
    , 0x6E, 0x73, 0x3A, 0x6D, 0x65, 0x74, 0x61, 0x2F, 0x22, 0x20, 0x78, 0x3A, 0x78, 0x6D, 0x70
    , 0x74, 0x6B, 0x3D, 0x22, 0x41, 0x64, 0x6F, 0x62, 0x65, 0x20, 0x58, 0x4D, 0x50, 0x20, 0x43
    , 0x6F, 0x72, 0x65, 0x20, 0x35, 0x2E, 0x35, 0x2D, 0x63, 0x30, 0x32, 0x31, 0x20, 0x37, 0x39
    , 0x2E, 0x31, 0x35, 0x35, 0x37, 0x37, 0x32, 0x2C, 0x20, 0x32, 0x30, 0x31, 0x34, 0x2F, 0x30
    , 0x31, 0x2F, 0x31, 0x33, 0x2D, 0x31, 0x39, 0x3A, 0x34, 0x34, 0x3A, 0x30, 0x30, 0x20, 0x20
    , 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x3E, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x52
    , 0x44, 0x46, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x72, 0x64, 0x66, 0x3D, 0x22, 0x68
    , 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x77, 0x33, 0x2E, 0x6F, 0x72
    , 0x67, 0x2F, 0x31, 0x39, 0x39, 0x39, 0x2F, 0x30, 0x32, 0x2F, 0x32, 0x32, 0x2D, 0x72, 0x64
    , 0x66, 0x2D, 0x73, 0x79, 0x6E, 0x74, 0x61, 0x78, 0x2D, 0x6E, 0x73, 0x23, 0x22, 0x3E, 0x20
    , 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F
    , 0x6E, 0x20, 0x72, 0x64, 0x66, 0x3A, 0x61, 0x62, 0x6F, 0x75, 0x74, 0x3D, 0x22, 0x22, 0x20
    , 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3D, 0x22, 0x68, 0x74
    , 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63
    , 0x6F, 0x6D, 0x2F, 0x78, 0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x6D, 0x6D, 0x2F, 0x22
    , 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x73, 0x74, 0x52, 0x65, 0x66, 0x3D, 0x22, 0x68
    , 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E
    , 0x63, 0x6F, 0x6D, 0x2F, 0x78, 0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x73, 0x54, 0x79
    , 0x70, 0x65, 0x2F, 0x52, 0x65, 0x73, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x52, 0x65, 0x66, 0x23
    , 0x22, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x78, 0x6D, 0x70, 0x3D, 0x22, 0x68, 0x74
    , 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63
    , 0x6F, 0x6D, 0x2F, 0x78, 0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x22, 0x20, 0x78, 0x6D
    , 0x70, 0x4D, 0x4D, 0x3A, 0x4F, 0x72, 0x69, 0x67, 0x69, 0x6E, 0x61, 0x6C, 0x44, 0x6F, 0x63
    , 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x49, 0x44, 0x3D, 0x22, 0x78, 0x6D, 0x70, 0x2E, 0x64, 0x69
    , 0x64, 0x3A, 0x46, 0x37, 0x37, 0x46, 0x31, 0x31, 0x37, 0x34, 0x30, 0x37, 0x32, 0x30, 0x36
    , 0x38, 0x31, 0x31, 0x38, 0x46, 0x44, 0x39, 0x42, 0x36, 0x37, 0x35, 0x36, 0x38, 0x36, 0x34
    , 0x35, 0x34, 0x36, 0x41, 0x22, 0x20, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3A, 0x44, 0x6F, 0x63
    , 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x49, 0x44, 0x3D, 0x22, 0x78, 0x6D, 0x70, 0x2E, 0x64, 0x69
    , 0x64, 0x3A, 0x32, 0x41, 0x36, 0x34, 0x45, 0x32, 0x41, 0x43, 0x46, 0x41, 0x33, 0x34, 0x31
    , 0x31, 0x45, 0x33, 0x38, 0x31, 0x41, 0x35, 0x42, 0x36, 0x30, 0x34, 0x38, 0x31, 0x42, 0x45
    , 0x44, 0x33, 0x32, 0x39, 0x22, 0x20, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3A, 0x49, 0x6E, 0x73
    , 0x74, 0x61, 0x6E, 0x63, 0x65, 0x49, 0x44, 0x3D, 0x22, 0x78, 0x6D, 0x70, 0x2E, 0x69, 0x69
    , 0x64, 0x3A, 0x32, 0x41, 0x36, 0x34, 0x45, 0x32, 0x41, 0x42, 0x46, 0x41, 0x33, 0x34, 0x31
    , 0x31, 0x45, 0x33, 0x38, 0x31, 0x41, 0x35, 0x42, 0x36, 0x30, 0x34, 0x38, 0x31, 0x42, 0x45
    , 0x44, 0x33, 0x32, 0x39, 0x22, 0x20, 0x78, 0x6D, 0x70, 0x3A, 0x43, 0x72, 0x65, 0x61, 0x74
    , 0x6F, 0x72, 0x54, 0x6F, 0x6F, 0x6C, 0x3D, 0x22, 0x41, 0x64, 0x6F, 0x62, 0x65, 0x20, 0x50
    , 0x68, 0x6F, 0x74, 0x6F, 0x73, 0x68, 0x6F, 0x70, 0x20, 0x43, 0x53, 0x35, 0x20, 0x4D, 0x61
    , 0x63, 0x69, 0x6E, 0x74, 0x6F, 0x73, 0x68, 0x22, 0x3E, 0x20, 0x3C, 0x78, 0x6D, 0x70, 0x4D
    , 0x4D, 0x3A, 0x44, 0x65, 0x72, 0x69, 0x76, 0x65, 0x64, 0x46, 0x72, 0x6F, 0x6D, 0x20, 0x73
    , 0x74, 0x52, 0x65, 0x66, 0x3A, 0x69, 0x6E, 0x73, 0x74, 0x61, 0x6E, 0x63, 0x65, 0x49, 0x44
    , 0x3D, 0x22, 0x78, 0x6D, 0x70, 0x2E, 0x69, 0x69, 0x64, 0x3A, 0x31, 0x32, 0x45, 0x43, 0x37
    , 0x36, 0x36, 0x38, 0x33, 0x38, 0x41, 0x38, 0x31, 0x31, 0x45, 0x33, 0x42, 0x31, 0x37, 0x46
    , 0x38, 0x44, 0x36, 0x39, 0x32, 0x31, 0x42, 0x33, 0x45, 0x33, 0x45, 0x39, 0x22, 0x20, 0x73
    , 0x74, 0x52, 0x65, 0x66, 0x3A, 0x64, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x49, 0x44
    , 0x3D, 0x22, 0x78, 0x6D, 0x70, 0x2E, 0x64, 0x69, 0x64, 0x3A, 0x31, 0x32, 0x45, 0x43, 0x37
    , 0x36, 0x36, 0x39, 0x33, 0x38, 0x41, 0x38, 0x31, 0x31, 0x45, 0x33, 0x42, 0x31, 0x37, 0x46
    , 0x38, 0x44, 0x36, 0x39, 0x32, 0x31, 0x42, 0x33, 0x45, 0x33, 0x45, 0x39, 0x22, 0x2F, 0x3E
    , 0x20, 0x3C, 0x2F, 0x72, 0x64, 0x66, 0x3A, 0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74
    , 0x69, 0x6F, 0x6E, 0x3E, 0x20, 0x3C, 0x2F, 0x72, 0x64, 0x66, 0x3A, 0x52, 0x44, 0x46, 0x3E
    , 0x20, 0x3C, 0x2F, 0x78, 0x3A, 0x78, 0x6D, 0x70, 0x6D, 0x65, 0x74, 0x61, 0x3E, 0x20, 0x3C
    , 0x3F, 0x78, 0x70, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x20, 0x65, 0x6E, 0x64, 0x3D, 0x22, 0x72
    , 0x22, 0x3F, 0x3E, 0x91, 0xDD, 0xBE, 0xEF, 0x00, 0x00, 0x09, 0x5E, 0x49, 0x44, 0x41, 0x54
    , 0x78, 0xDA, 0xEC, 0x5D, 0x0D, 0x6C, 0x14, 0x45, 0x14, 0x9E, 0xDB, 0x16, 0x5A, 0xFE, 0xA4
    , 0x45, 0x90, 0x6A, 0xAA, 0x12, 0x35, 0x45, 0x51, 0xA3, 0xD5, 0x20, 0x0A, 0x25, 0xFE, 0xE1
    , 0x4F, 0x10, 0x94, 0x58, 0x0D, 0x2A, 0x09, 0x82, 0xD0, 0x68, 0x4C, 0x44, 0x5B, 0x12, 0x68
    , 0x63, 0x54, 0x34, 0xFE, 0x15, 0x50, 0x14, 0x82, 0x51, 0x14, 0xAD, 0x3F, 0x68, 0x88, 0x4A
    , 0x10, 0x35, 0x80, 0x5A, 0x8A, 0xFC, 0x23, 0x88, 0x14, 0x83, 0x0A, 0x22, 0x44, 0x7E, 0x0A
    , 0x48, 0xA1, 0xC5, 0x82, 0x6D, 0xED, 0xB5, 0xE7, 0x7B, 0xEE, 0x5B, 0xD9, 0x6E, 0xF6, 0xEE
    , 0x66, 0x77, 0x67, 0xE6, 0xE6, 0xAE, 0xBE, 0xE4, 0x0B, 0xB4, 0x37, 0xF7, 0x66, 0xFA, 0xED
    , 0xCC, 0x9B, 0x37, 0xEF, 0xCD, 0xCE, 0x84, 0x22, 0x91, 0x08, 0xD3, 0x44, 0x7A, 0x00, 0xFA
    , 0x01, 0xDA, 0xE8, 0xE7, 0x10, 0xA0, 0x11, 0xF0, 0x9B, 0x0E, 0x8D, 0x4B, 0x57, 0x5C, 0x5F
    , 0x16, 0xE0, 0x2C, 0xC0, 0x15, 0x80, 0x0B, 0x01, 0x17, 0xD1, 0xCF, 0x5D, 0x00, 0x9D, 0x08
    , 0x76, 0x41, 0xD2, 0x9A, 0x01, 0xF8, 0x34, 0xF7, 0x01, 0x7E, 0x06, 0xEC, 0x05, 0x6C, 0x05
    , 0xEC, 0x02, 0xD4, 0xD1, 0xE7, 0xD2, 0x25, 0xA4, 0xA0, 0x47, 0x5D, 0x00, 0x18, 0x0C, 0x18
    , 0x0A, 0xB8, 0x1A, 0x70, 0x89, 0x20, 0xBD, 0xF5, 0x80, 0x35, 0x80, 0x55, 0x80, 0x0D, 0x80
    , 0x75, 0x80, 0xD6, 0x64, 0x24, 0x6A, 0x38, 0xA0, 0x10, 0x50, 0x00, 0xC8, 0x93, 0xFC, 0x30
    , 0x1A, 0x00, 0xEB, 0x01, 0xCB, 0x00, 0x1F, 0x00, 0x8E, 0xE8, 0x4E, 0x54, 0x57, 0xC0, 0xB5
    , 0x80, 0xA9, 0x80, 0x41, 0x80, 0xCC, 0x04, 0x98, 0x93, 0xFD, 0x80, 0xB7, 0x00, 0xF3, 0x00
    , 0x07, 0x75, 0x24, 0xEA, 0x06, 0x40, 0x29, 0xE0, 0x26, 0x4D, 0x26, 0x07, 0x24, 0xE9, 0x15
    , 0x22, 0xAC, 0x5E, 0x07, 0xA2, 0x72, 0x00, 0x8F, 0x03, 0x1E, 0xA0, 0x1E, 0xA5, 0x9B, 0xA0
    , 0xED, 0x7A, 0x16, 0xB0, 0x34, 0x91, 0x44, 0xDD, 0x0C, 0x98, 0x4B, 0x06, 0x5B, 0x67, 0x69
    , 0x02, 0xBC, 0x0A, 0x78, 0x9A, 0x5C, 0x0E, 0x65, 0x44, 0x75, 0x06, 0x4C, 0x06, 0x3C, 0x99
    , 0x20, 0x3B, 0xE4, 0x57, 0xD0, 0xD8, 0x3F, 0x06, 0xD8, 0xA1, 0x82, 0xA8, 0x2E, 0xD4, 0x8B
    , 0xC6, 0xB3, 0xE4, 0x94, 0xDD, 0xD4, 0xF6, 0x55, 0x32, 0x89, 0xCA, 0xA2, 0x19, 0xA5, 0x90
    , 0x25, 0xB7, 0x1C, 0x06, 0x4C, 0x00, 0x7C, 0x29, 0x83, 0xA8, 0x6C, 0xC0, 0x42, 0x8D, 0x66
    , 0x35, 0x11, 0x0E, 0xEB, 0x58, 0xC0, 0xE7, 0x22, 0x89, 0xCA, 0x00, 0x7C, 0x08, 0xB8, 0x93
    , 0xA5, 0x96, 0xD4, 0x02, 0x46, 0x01, 0xD6, 0xC6, 0x2B, 0x68, 0x70, 0x28, 0x33, 0xC8, 0x1F
    , 0x49, 0x35, 0x92, 0x50, 0x7A, 0x03, 0xDE, 0x05, 0x5C, 0x2A, 0x82, 0xA8, 0x49, 0x80, 0x87
    , 0x58, 0xEA, 0xCA, 0xF9, 0x80, 0xD7, 0x01, 0xDD, 0x82, 0x10, 0x75, 0x15, 0xE0, 0x29, 0x96
    , 0xFA, 0x82, 0x8B, 0xF6, 0xE7, 0xFC, 0xDA, 0xA8, 0x9E, 0x80, 0x6F, 0x01, 0x97, 0xB1, 0x8E
    , 0x21, 0x48, 0xC4, 0x68, 0xC0, 0xC7, 0x5E, 0x7B, 0x54, 0x59, 0x07, 0x22, 0xE9, 0xDF, 0x4E
    , 0x03, 0x78, 0x06, 0xD0, 0xD7, 0x0B, 0x51, 0xF9, 0x80, 0x07, 0x15, 0x36, 0xB2, 0x92, 0x1A
    , 0x1A, 0x0B, 0x95, 0x0A, 0xDA, 0x81, 0xC1, 0xC4, 0x12, 0x5E, 0xA2, 0x0C, 0xEA, 0x4D, 0x59
    , 0x0A, 0x89, 0xAA, 0x13, 0x54, 0x46, 0x84, 0x3C, 0xEA, 0x36, 0x0B, 0xBA, 0x11, 0x85, 0x71
    , 0xA4, 0xBB, 0x15, 0x77, 0x7B, 0x1E, 0x12, 0xEA, 0x15, 0xB5, 0x05, 0x7D, 0xC6, 0x52, 0x1E
    , 0xA2, 0x26, 0x27, 0xC8, 0x4B, 0xD6, 0xA5, 0x47, 0x31, 0x72, 0x42, 0xF3, 0x63, 0x11, 0x35
    , 0x10, 0x70, 0xAB, 0xC0, 0x0A, 0xCB, 0x68, 0xCC, 0xB7, 0x29, 0x18, 0x7A, 0x6D, 0x54, 0x57
    , 0x99, 0x80, 0x76, 0x63, 0x5C, 0xAD, 0xA8, 0xFD, 0x9C, 0x08, 0xEE, 0x81, 0x0D, 0xB3, 0x22
    , 0xE2, 0x64, 0xAA, 0x4D, 0x6F, 0x21, 0xA0, 0xD1, 0xA5, 0x4C, 0x18, 0x50, 0x05, 0x28, 0x70
    , 0xB4, 0xC3, 0x0D, 0x05, 0x54, 0x36, 0xEC, 0xA2, 0xA7, 0x91, 0xEA, 0xB0, 0xCA, 0x4E, 0x15
    , 0xD0, 0xFE, 0x1D, 0x80, 0x73, 0x2C, 0x9D, 0xF6, 0x86, 0xF4, 0x06, 0x6C, 0x91, 0x40, 0x92
    , 0x85, 0x21, 0x80, 0x5A, 0xFA, 0xBC, 0x06, 0x50, 0x0A, 0xC8, 0xE1, 0x20, 0xC8, 0x89, 0x1C
    , 0xFA, 0x6E, 0x0D, 0xE9, 0xAA, 0x25, 0xDD, 0xCE, 0x72, 0x41, 0xC9, 0x6A, 0x05, 0xDC, 0xE3
    , 0x46, 0xD4, 0x48, 0xFA, 0x50, 0x06, 0x49, 0x16, 0xF2, 0x00, 0x25, 0x80, 0x4C, 0x1F, 0x04
    , 0x39, 0x91, 0x49, 0xBA, 0xF2, 0x62, 0x94, 0x09, 0x4A, 0xD6, 0xFB, 0x80, 0x34, 0x27, 0x51
    , 0x33, 0x24, 0x93, 0x94, 0x28, 0x04, 0x21, 0xEB, 0x0F, 0x40, 0x5F, 0xD4, 0x63, 0xD8, 0x8C
    , 0xD7, 0x35, 0x02, 0x0C, 0x77, 0xB9, 0x86, 0x1E, 0x77, 0x79, 0x00, 0x03, 0xDF, 0x07, 0xD0
    , 0xDF, 0x3E, 0xEB, 0xF5, 0xA1, 0x19, 0x2F, 0x88, 0x34, 0x33, 0x7D, 0x25, 0x48, 0xDB, 0xAE
    , 0xB7, 0x13, 0x75, 0x31, 0x33, 0x13, 0x06, 0x41, 0x64, 0x26, 0xD3, 0x33, 0x44, 0x5C, 0x48
    , 0x6D, 0x13, 0x42, 0xD4, 0x00, 0x01, 0x0D, 0x42, 0x5D, 0x98, 0xCE, 0x1E, 0xA2, 0x11, 0x49
    , 0x43, 0xA8, 0x4D, 0x46, 0x00, 0x1D, 0x18, 0xAF, 0xCA, 0xB4, 0x14, 0xF4, 0x17, 0xD4, 0x30
    , 0x4C, 0x5D, 0x7D, 0xC6, 0xE4, 0xEF, 0x35, 0xE0, 0x91, 0x3C, 0x6A, 0x4B, 0xD0, 0x74, 0x1A
    , 0x06, 0xF4, 0xCE, 0x35, 0x6C, 0xAC, 0x89, 0x92, 0xD3, 0x01, 0x23, 0x3C, 0x7E, 0x27, 0x97
    , 0x99, 0x59, 0x91, 0x0A, 0x40, 0x35, 0x33, 0x37, 0x5D, 0x34, 0xD0, 0xFF, 0x2B, 0xE8, 0xB3
    , 0x5C, 0x8F, 0x3A, 0x47, 0x50, 0x5B, 0x44, 0x3C, 0xFC, 0x5C, 0x9C, 0x3E, 0x3B, 0x01, 0x7E
    , 0x14, 0xE8, 0x91, 0xD7, 0x78, 0xF0, 0x93, 0x30, 0x70, 0x58, 0x04, 0x58, 0xCB, 0xA1, 0x77
    , 0x2D, 0x95, 0x0D, 0x79, 0xF0, 0xB3, 0x6A, 0x04, 0xFC, 0x3D, 0xE8, 0x5B, 0x8E, 0x36, 0x68
    , 0xC6, 0xEB, 0x24, 0xB0, 0x47, 0xCD, 0x66, 0x66, 0x0A, 0x3B, 0x9E, 0xF4, 0x02, 0xBC, 0xC8
    , 0xCC, 0x4D, 0x14, 0x83, 0x39, 0xC3, 0xB5, 0xF3, 0xE8, 0x3B, 0xBD, 0x38, 0xCA, 0x37, 0x51
    , 0x5B, 0x44, 0xD8, 0xDE, 0x2C, 0x83, 0xBA, 0x67, 0x9A, 0x20, 0x92, 0x5A, 0x69, 0xA8, 0xF0
    , 0x44, 0x13, 0x71, 0x6B, 0xD0, 0x14, 0x1F, 0x75, 0x4C, 0xA1, 0xEF, 0x86, 0x38, 0xCA, 0x56
    , 0x30, 0x31, 0x9B, 0xCB, 0xBA, 0x19, 0x34, 0x06, 0x43, 0x82, 0x88, 0x5A, 0x0D, 0x38, 0xC4
    , 0x51, 0x6E, 0xA2, 0x4F, 0x92, 0xEC, 0x64, 0x4D, 0xE4, 0x28, 0x77, 0x88, 0xDA, 0x14, 0x54
    , 0x3A, 0xA7, 0x07, 0x60, 0xBC, 0x92, 0x42, 0x1F, 0xF5, 0xF4, 0x6F, 0x1D, 0x67, 0xA3, 0xD0
    , 0x28, 0x8F, 0x13, 0xD0, 0x78, 0xD4, 0x81, 0x5B, 0x79, 0xF6, 0xC7, 0x29, 0xF7, 0x04, 0x33
    , 0xB7, 0x45, 0x66, 0x13, 0xB2, 0xE8, 0xDF, 0x1B, 0xBD, 0x8C, 0x94, 0x74, 0x9A, 0x5D, 0xFC
    , 0x90, 0x35, 0xCC, 0xE7, 0x1F, 0x78, 0x0B, 0xA7, 0x4D, 0xE2, 0xB1, 0x59, 0xA8, 0x6B, 0x7E
    , 0x9C, 0x72, 0x6B, 0x08, 0x4E, 0xF1, 0xB2, 0xE9, 0xE2, 0x6F, 0x83, 0x7A, 0x42, 0x1B, 0x53
    , 0x27, 0x43, 0x35, 0xD5, 0x15, 0x4B, 0x1A, 0x90, 0x28, 0xCC, 0xBF, 0x37, 0x2A, 0x24, 0x2A
    , 0x5F, 0x53, 0x5D, 0xD1, 0x24, 0x0C, 0x38, 0x66, 0x39, 0x9C, 0x35, 0x0A, 0x89, 0x3A, 0x4F
    , 0x53, 0x5D, 0xB1, 0x16, 0xD4, 0x87, 0x2D, 0xA2, 0x76, 0xB1, 0xFF, 0x25, 0x9A, 0xE0, 0x68
    , 0xDB, 0x63, 0x11, 0xF5, 0x8B, 0xC2, 0x8A, 0x77, 0x6B, 0xAA, 0x2B, 0x9A, 0x1C, 0x43, 0x37
    , 0xC3, 0x22, 0xEA, 0x7B, 0x85, 0x44, 0xFD, 0xA0, 0xA9, 0xAE, 0x68, 0xB2, 0xC5, 0x1E, 0x66
    , 0xF9, 0x9D, 0x98, 0x53, 0x21, 0xAB, 0x35, 0xD5, 0x15, 0xB3, 0x0E, 0xEB, 0xA5, 0x21, 0x74
    , 0x1A, 0x71, 0xF3, 0xE7, 0x28, 0x0F, 0x0A, 0xBE, 0xB1, 0x39, 0x9A, 0xF5, 0x36, 0x87, 0x73
    , 0x4D, 0x9C, 0xEF, 0x2D, 0x67, 0xE6, 0xDE, 0xEF, 0xA0, 0xBE, 0xD4, 0x3A, 0xD2, 0x15, 0x4F
    , 0x0A, 0x6C, 0x0E, 0x67, 0x96, 0xCD, 0xF1, 0xE4, 0x95, 0x2A, 0xD3, 0xEB, 0x3A, 0xB5, 0xDA
    , 0x9E, 0x24, 0x60, 0xA5, 0x5D, 0xC5, 0xB9, 0xB2, 0x2F, 0x12, 0x50, 0x57, 0x11, 0x67, 0x5D
    , 0x55, 0x01, 0xEA, 0xC0, 0xF4, 0x5D, 0x0F, 0x67, 0x16, 0x66, 0x20, 0xE0, 0x68, 0xC0, 0xC6
    , 0x87, 0x39, 0x73, 0x75, 0x18, 0x2A, 0x29, 0x0F, 0x50, 0x4F, 0x39, 0x67, 0xB8, 0x25, 0x27
    , 0x4A, 0xC2, 0x94, 0x57, 0xA6, 0x59, 0xBA, 0xEC, 0x21, 0x52, 0x34, 0xE8, 0x1B, 0x02, 0x0E
    , 0x87, 0x34, 0xCE, 0x75, 0x1C, 0x2E, 0x1F, 0x30, 0x3B, 0x32, 0xDD, 0x47, 0x1D, 0xD3, 0xE9
    , 0xBB, 0x3C, 0x4B, 0x90, 0x71, 0x01, 0x22, 0x23, 0x27, 0x01, 0x2B, 0x4E, 0xB5, 0xB8, 0xFD
    , 0x13, 0x18, 0x23, 0x20, 0x09, 0x9A, 0x2A, 0x81, 0xBB, 0xE5, 0xF6, 0xBF, 0xC3, 0xA9, 0xBC
    , 0x2B, 0x60, 0xBB, 0x00, 0xFB, 0x51, 0xE2, 0x31, 0x49, 0x99, 0x0B, 0x98, 0x00, 0xA8, 0x00
    , 0x54, 0x03, 0x1A, 0x08, 0xD5, 0xF4, 0xBB, 0x09, 0x54, 0xC6, 0x8B, 0xCE, 0x92, 0x00, 0xED
    , 0x6F, 0x03, 0x4C, 0xB4, 0xEB, 0x73, 0xAB, 0xE0, 0x91, 0x80, 0x24, 0xD5, 0xC6, 0x49, 0x73
    , 0xAB, 0x42, 0x9E, 0x6D, 0xAF, 0x83, 0x57, 0xF9, 0x09, 0x90, 0x61, 0xD7, 0xE7, 0x96, 0xC6
    , 0x59, 0xC0, 0xFC, 0xBF, 0xF0, 0x8C, 0xE1, 0xD7, 0x3B, 0x00, 0x3B, 0x35, 0x58, 0x7A, 0xEC
    , 0xA4, 0xB6, 0x34, 0xF9, 0xF8, 0xEE, 0x4B, 0xCC, 0x99, 0x34, 0x8D, 0xF2, 0x34, 0xC6, 0xFB
    , 0x0C, 0xC2, 0x17, 0x6A, 0xB8, 0xF7, 0xA0, 0xD0, 0xA3, 0xDD, 0xFD, 0xCF, 0x25, 0x88, 0x37
    , 0xF4, 0xAC, 0xCC, 0x4C, 0xA5, 0x47, 0xA2, 0x8A, 0x35, 0x24, 0xC9, 0x42, 0xB1, 0x07, 0xDB
    , 0x34, 0xD6, 0x4D, 0x47, 0xB4, 0x0C, 0x6A, 0x0B, 0x60, 0x1A, 0x46, 0xF6, 0x3C, 0x74, 0xD7
    , 0x0C, 0x8D, 0x23, 0x00, 0x19, 0x1E, 0xBC, 0xFD, 0x25, 0xEE, 0x0E, 0x4D, 0xEC, 0x27, 0x31
    , 0xA7, 0x03, 0x6D, 0xFB, 0x09, 0x93, 0x7B, 0xE4, 0xAA, 0x87, 0xC7, 0xB3, 0xDD, 0xDD, 0x41
    , 0x36, 0x92, 0xAD, 0x07, 0x18, 0x7E, 0x89, 0x42, 0xDC, 0x0F, 0x68, 0x49, 0xF1, 0xAD, 0x89
    , 0x68, 0x9B, 0x6E, 0x8B, 0x55, 0x2F, 0x4F, 0xC3, 0xD0, 0x09, 0x5D, 0x92, 0xE2, 0x9B, 0x5D
    , 0x97, 0xD1, 0x04, 0x16, 0x88, 0x28, 0x44, 0x3E, 0x79, 0xCA, 0x5E, 0xA5, 0x94, 0x66, 0x9C
    , 0x78, 0xD3, 0x73, 0x19, 0x47, 0x1B, 0xCA, 0x38, 0xDC, 0x93, 0x62, 0xAA, 0xD3, 0xAB, 0x5B
    , 0x33, 0x3C, 0x5E, 0xFD, 0xBC, 0x87, 0xD8, 0x60, 0x24, 0x71, 0x2E, 0xA5, 0xB2, 0xBD, 0xC8
    , 0x0B, 0x9C, 0xE5, 0xB2, 0x05, 0x94, 0xC1, 0x19, 0xFC, 0x65, 0x9F, 0x89, 0xDC, 0x2A, 0xC6
    , 0xA1, 0x9C, 0x57, 0x66, 0x53, 0x24, 0x54, 0x86, 0x64, 0x09, 0x22, 0xD3, 0xAB, 0x60, 0xE2
    , 0xF7, 0x4D, 0x9E, 0x74, 0x9D, 0x17, 0xA2, 0x30, 0xA5, 0x35, 0x43, 0x12, 0x51, 0xD9, 0x82
    , 0xC8, 0xF4, 0x2A, 0x9B, 0x01, 0x9F, 0xF2, 0x14, 0xF4, 0xBA, 0x65, 0x6F, 0x81, 0xA4, 0x38
    , 0x75, 0x76, 0x82, 0x7A, 0xD4, 0x0C, 0xDE, 0x2C, 0xB9, 0x9F, 0x03, 0x22, 0x70, 0xA1, 0xF9
    , 0x09, 0x53, 0x7F, 0x48, 0x97, 0x68, 0xD9, 0xC4, 0xCC, 0x83, 0x77, 0x4E, 0xC8, 0xE8, 0x51
    , 0x28, 0xB8, 0x2F, 0x72, 0x65, 0x0A, 0x24, 0x36, 0xDF, 0xE1, 0x25, 0xC9, 0x2F, 0x51, 0x28
    , 0xB8, 0xEB, 0x2D, 0x9C, 0xC4, 0x24, 0x61, 0x66, 0xFC, 0x0B, 0x2F, 0x5F, 0xF0, 0x4B, 0x14
    , 0xA6, 0xB6, 0xBE, 0x4A, 0x62, 0xA2, 0x90, 0xA4, 0x7D, 0x2A, 0x88, 0xC2, 0xE8, 0xC2, 0x7B
    , 0xCC, 0xDB, 0x1E, 0x23, 0x5D, 0xE4, 0x2F, 0x1A, 0x76, 0x4C, 0x05, 0x51, 0x56, 0x02, 0xB4
    , 0x3A, 0x09, 0x89, 0xFA, 0x1A, 0xB0, 0x4D, 0x25, 0x51, 0x47, 0x99, 0xB9, 0x4B, 0x37, 0x99
    , 0x04, 0x1D, 0x4B, 0x5F, 0xAF, 0x7B, 0x18, 0x01, 0x2B, 0x7E, 0x83, 0x7A, 0x56, 0xB2, 0xC8
    , 0x6B, 0x2C, 0x7E, 0xCA, 0x5F, 0x98, 0x1F, 0xE5, 0x94, 0xC1, 0xB4, 0x5E, 0xD2, 0xFD, 0x64
    , 0xB2, 0x5F, 0x99, 0xB9, 0xEF, 0x74, 0x6F, 0x22, 0x7A, 0x14, 0xCA, 0x77, 0x51, 0xC3, 0xA7
    , 0x7A, 0xC9, 0x2C, 0xBF, 0x24, 0x89, 0x22, 0x2A, 0x4C, 0x33, 0x60, 0x8B, 0xC6, 0x24, 0x6D
    , 0xA3, 0xC5, 0x2F, 0x4B, 0x24, 0x51, 0x8C, 0x3C, 0xF5, 0x4D, 0x1A, 0x13, 0x35, 0x33, 0xA8
    , 0x83, 0x2C, 0x8A, 0xA8, 0x93, 0x1A, 0x3B, 0xA0, 0x98, 0x08, 0x5D, 0x1A, 0x54, 0x89, 0x21
    , 0xB0, 0x41, 0x8B, 0x98, 0xDA, 0x6D, 0xD8, 0xBC, 0x82, 0x61, 0x94, 0x5A, 0x9D, 0x88, 0xDA
    , 0xCE, 0xE2, 0xBF, 0x6E, 0xA1, 0x5A, 0x30, 0x9D, 0x5E, 0x25, 0x42, 0x91, 0x48, 0xA2, 0x30
    , 0xAE, 0xB3, 0x51, 0x43, 0x97, 0x60, 0xAB, 0x6E, 0x44, 0x31, 0x51, 0x8D, 0x12, 0x1C, 0x25
    , 0x38, 0xA2, 0x23, 0x51, 0x7B, 0x34, 0x23, 0x6A, 0x87, 0x28, 0x45, 0xA2, 0x89, 0xAA, 0xD3
    , 0x8C, 0xA8, 0x03, 0xA2, 0x14, 0x89, 0x0E, 0xE7, 0x36, 0xC7, 0x70, 0x4A, 0x65, 0x1A, 0x7A
    , 0x7C, 0x31, 0xF3, 0x6C, 0xC7, 0x83, 0x8F, 0x90, 0xDB, 0xA2, 0x25, 0x51, 0x6E, 0x6F, 0x92
    , 0x62, 0xFC, 0xE7, 0x61, 0x66, 0x1E, 0x0C, 0x2A, 0x53, 0x8A, 0x01, 0xCF, 0x3B, 0xD6, 0x9C
    , 0x6D, 0xBA, 0x0E, 0x3D, 0xB7, 0xDD, 0x6D, 0x8B, 0x15, 0x90, 0x64, 0xAD, 0xE5, 0x36, 0x3B
    , 0x7A, 0x54, 0x58, 0x57, 0xA2, 0xDC, 0xD6, 0x7B, 0x2A, 0x9D, 0xD0, 0xB0, 0x83, 0xA8, 0x66
    , 0x5D, 0x89, 0x72, 0xEB, 0xEA, 0x78, 0x36, 0xCA, 0x48, 0x66, 0x1E, 0x19, 0x80, 0xF5, 0xA5
    , 0x09, 0x06, 0xA3, 0xE1, 0x86, 0x87, 0x48, 0x5C, 0xE9, 0xA8, 0x5B, 0x58, 0xA8, 0x5A, 0xB4
    , 0x8D, 0x72, 0x6B, 0x18, 0x66, 0x78, 0x3F, 0x62, 0xE6, 0x75, 0x24, 0x61, 0x26, 0xEE, 0x8D
    , 0x78, 0xFB, 0xC3, 0x41, 0xA2, 0x30, 0x2E, 0x96, 0xE1, 0xF8, 0x7D, 0xB3, 0xCE, 0x44, 0x45
    , 0x5C, 0xC8, 0xC0, 0xF3, 0x4D, 0x86, 0x31, 0xF5, 0xA2, 0xAD, 0x31, 0x6F, 0x65, 0x7A, 0xC5
    , 0xA5, 0xB4, 0x25, 0x8A, 0x69, 0x46, 0x52, 0x93, 0xAE, 0x43, 0x2F, 0x9A, 0x9D, 0x6A, 0xA2
    , 0xE5, 0xC4, 0x41, 0x49, 0x0F, 0xE7, 0x4C, 0x66, 0x5E, 0x18, 0x96, 0x9E, 0x2C, 0xC6, 0x3C
    , 0x4C, 0xA4, 0xD8, 0x8D, 0x2A, 0xBE, 0xF4, 0x38, 0x9E, 0xFC, 0x29, 0x99, 0x82, 0x17, 0xF8
    , 0xCC, 0x61, 0xED, 0x2F, 0xF1, 0x89, 0x24, 0xD3, 0xD0, 0x5B, 0xA2, 0x80, 0x24, 0x94, 0xB7
    , 0x1D, 0x0E, 0xA7, 0xD0, 0x59, 0xCF, 0x50, 0x30, 0xF4, 0x54, 0xA6, 0xDD, 0x0D, 0x59, 0xC6
    , 0x5C, 0xF4, 0xD0, 0x6B, 0x75, 0x79, 0x8A, 0xB8, 0x9F, 0x0A, 0x2F, 0xB6, 0x58, 0x46, 0x5E
    , 0xBA, 0x0C, 0x27, 0xB7, 0x3B, 0xE0, 0x5E, 0x26, 0xF1, 0xA0, 0x7A, 0x59, 0x7E, 0x94, 0xD3
    , 0xE1, 0xC4, 0x9D, 0x7A, 0x1B, 0xC8, 0x7E, 0x85, 0x24, 0xD4, 0x89, 0x44, 0x0D, 0x62, 0xED
    , 0x0F, 0x0C, 0x8B, 0xE8, 0xDC, 0xA3, 0xA2, 0x0D, 0x35, 0xF4, 0x9C, 0xAF, 0x53, 0xEC, 0x1E
    , 0x44, 0x44, 0xBA, 0x07, 0x86, 0x82, 0xA1, 0x97, 0x48, 0xD1, 0x76, 0xD6, 0x8B, 0xB0, 0xE4
    , 0xDC, 0x33, 0xA5, 0xCD, 0xD0, 0xC3, 0x48, 0x23, 0xA6, 0xB5, 0x8F, 0x4B, 0x32, 0xE6, 0x68
    , 0x07, 0xF3, 0x1D, 0xFE, 0x5B, 0x84, 0x09, 0xBC, 0x3C, 0x55, 0xC6, 0xAC, 0xE7, 0x7C, 0xC7
    , 0x0F, 0x1D, 0x4E, 0xBC, 0x6F, 0x45, 0x76, 0x26, 0x19, 0xEB, 0x98, 0xCF, 0x4E, 0xDD, 0x18
    , 0xA4, 0x75, 0x3C, 0xCA, 0x6D, 0xA6, 0x59, 0xCC, 0xD4, 0xA4, 0xDB, 0x17, 0x3A, 0x1C, 0x4E
    , 0xA9, 0x0E, 0x9A, 0x8C, 0xA1, 0x97, 0x96, 0xE4, 0xA6, 0x44, 0xD9, 0xD0, 0xC3, 0xAB, 0x53
    , 0x56, 0xD0, 0x52, 0xE6, 0x38, 0x73, 0x8F, 0x57, 0x05, 0x7D, 0x30, 0x67, 0x00, 0xC6, 0x30
    , 0xF3, 0x46, 0x6D, 0xFB, 0xEF, 0x5B, 0x74, 0x25, 0xCA, 0x6D, 0xE8, 0xA1, 0x0F, 0x85, 0xBB
    , 0x70, 0x37, 0x32, 0x39, 0x77, 0xBB, 0x60, 0x9D, 0x78, 0x8D, 0xD2, 0xE5, 0x8E, 0x07, 0x10
    , 0x61, 0xDE, 0xDE, 0x89, 0x56, 0x4E, 0x54, 0xB4, 0x73, 0xD1, 0x07, 0x29, 0x9E, 0xD1, 0x43
    , 0x22, 0x4D, 0x8B, 0x68, 0x1B, 0x85, 0x67, 0x7A, 0x2E, 0x67, 0x7A, 0x08, 0xBA, 0x0A, 0x5A
    , 0xA7, 0xAB, 0xA6, 0x31, 0x33, 0xC7, 0x96, 0x48, 0xB1, 0xDA, 0x71, 0x5C, 0x58, 0xF7, 0x94
    , 0x78, 0xE9, 0x3C, 0xDE, 0xF4, 0x88, 0xD7, 0xB0, 0xF5, 0x54, 0x4C, 0x12, 0xDA, 0x42, 0xBC
    , 0x7B, 0x6A, 0xA5, 0xD0, 0x71, 0x2C, 0x91, 0x28, 0x14, 0xCC, 0xB3, 0xE1, 0x01, 0xC8, 0xB7
    , 0x33, 0xF9, 0xDB, 0xAB, 0x31, 0x3A, 0x51, 0xC1, 0xCC, 0x00, 0x9E, 0xF0, 0x04, 0x87, 0x6C
    , 0xA2, 0x2C, 0xC1, 0x03, 0x8C, 0xEF, 0x03, 0xDC, 0x05, 0x38, 0x4D, 0xA0, 0x5E, 0x8C, 0x6F
    , 0xE1, 0xF5, 0xB9, 0xF8, 0xFE, 0x20, 0xEE, 0x75, 0x97, 0x76, 0xA0, 0xA1, 0x2A, 0xA2, 0xAC
    , 0x59, 0x08, 0x93, 0x00, 0x78, 0x89, 0x18, 0x66, 0x8F, 0x07, 0xD0, 0x1A, 0xAD, 0x3B, 0xE7
    , 0xEC, 0x8B, 0xA4, 0x9C, 0x20, 0xBB, 0x83, 0xA7, 0xA7, 0x2D, 0xA2, 0x61, 0x76, 0x80, 0x29
    , 0x78, 0x25, 0x4E, 0x25, 0x51, 0x6E, 0x13, 0x09, 0x66, 0x4E, 0xF0, 0x12, 0x89, 0x7E, 0x44
    , 0x96, 0x75, 0xB2, 0xA1, 0xB5, 0xD8, 0x3D, 0x44, 0x24, 0xFC, 0xC9, 0xCC, 0x2C, 0x0E, 0xEE
    , 0xF0, 0xDD, 0x97, 0x88, 0xC6, 0xFE, 0x23, 0xC0, 0x00, 0xD5, 0xB8, 0x01, 0xE2, 0xB5, 0x1A
    , 0x05, 0x73, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82

};

const size_t DeviceIconSize = sizeof(DeviceIcon);

OPTIONAL_NAMESPACE_CLOSE

}