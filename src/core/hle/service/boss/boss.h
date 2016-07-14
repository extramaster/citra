// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "core/hle/service/service.h"

namespace Service {
namespace BOSS {

/**
 * BOSS::InitializeSession service function
 *  Inputs:
 *      0 : Header Code[0x00010082]
 *      1 : u32 unknown1 (unused)
 *      2 : u32 unknown2 (unused)
 *      3 : u32 unknown traslation (if not 0x20 , The errorcode 0xD9001830 will get into cmd_buf[1])
 *      4 : u32 unknown4 (pass to a function)
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 */
void InitializeSession(Service::Interface* self);

/**
 * BOSS::GetStorageInfo(No Sure) service function
 *  Inputs:
 *      0 : Header Code[0x00020010]
 *      1 : u32 unknown1
 *      2 : u32 unknown2
 *      3 : u32 unknown3
 *      4 : u8 unknown_flag
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 */
void GetStorageInfo(Service::Interface* self);

/**
 * BOSS::Unk_0x00080002 service function
 *  Inputs:
 *      0 : Header Code[0x00080002]
 *      1 : u32 unknown1
 *      2 : u32 unknown2
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 */
void Unk_0x00080002(Service::Interface* self);

/**
 * BOSS::Unk_0x00090040 service function
 *  Inputs:
 *      0 : Header Code[0x00090040]
 *      1 : u32 unknown1
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 */
void Unk_0x00090040(Service::Interface* self);

/**
 * BOSS::Unk_0x000A0000 service function
 *  Inputs:
 *      0 : Header Code[0x000A0000]
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : u8 unknown value
 */
void Unk_0x000A0000(Service::Interface* self);

/**
 * BOSS::Unk_0x000E0000 service function
 *  Inputs:
 *      0 : Header Code[0x000E0000]
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 */
void Unk_0x000E0000(Service::Interface* self);

/**
 * BOSS::Unk_0x00110102 service function
 *  Inputs:
 *      0 : Header Code[0x00110102]
 *      1 : u32 unknown1
 *      2 : u32 unknown2
 *      3 : u32 unknown3
 *      4 : u32 unknown4
 *      5 : u32 unknown5
 *      6 : u32 unknown6
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : u16 unknown value
 *      3 : u16 unknown value
 */
void Unk_0x00110102(Service::Interface* self);

/**
 * BOSS::Unk_0x00160082 service function
 *  Inputs:
 *      0 : Header Code[0x00160082]
 *      1 : u32 unknown1
 *      2 : u32 unknown_size
 *      3 : u32 (unknown translation & 0xF) == 0xC
 *      4 : u32 unknown addr
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : u16 unknown value
 *      3 : u16 unknown value
 */
void Unk_0x00160082(Service::Interface* self);

/**
 * BOSS::Unk_0x00180082 service function
 *  Inputs:
 *      0 : Header Code[0x00180082]
 *      1 : u32 unknown1
 *      2 : u32 unknown2
 *      3 : u32 unknown3
 *      4 : u32 unknown4
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 */
void Unk_0x00180082(Service::Interface* self);

/**
 * BOSS::Unk_0x00200082 service function
 *  Inputs:
 *      0 : Header Code[0x00200082]
 *      1 : u32 unknown_size
 *      2 : 0
 *      3 : u32 (unknown_translation & 0xF) == 0xA
 *      4 : u32 unknown_addr
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : u8 unknown value
 *      3 : u32 unknown value
 *      4 : u8 unknown value
 */
void Unk_0x00200082(Service::Interface* self);

/// Initialize BOSS service(s)
void Init();

/// Shutdown BOSS service(s)
void Shutdown();

} // namespace BOSS
} // namespace Service
