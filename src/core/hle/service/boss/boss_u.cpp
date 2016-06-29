// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "core\hle\service\boss\boss.h"
#include "core/hle/service/boss/boss_u.h"

namespace Service {
namespace BOSS {

const Interface::FunctionInfo FunctionTable[] = {
    {0x00010082, InitializeSession,     "InitializeSession"},
    {0x00020100, GetStorageInfo,        "GetStorageInfo"},
    {0x00080002, Unk_0x00080002,        "Unk_0x00080002"},
    {0x00090040, Unk_0x00090040,        "Unk_0x00090040"},
    {0x000A0000, Unk_0x000A0000,        "Unk_0x000A0000"},
    {0x000C0082, nullptr,               "UnregisterTask"},
    {0x000E0000, Unk_0x000E0000,        "Unk_0x000E0000"},
    {0x00110102, Unk_0x00110102,        "Unk_0x00110102"},
    {0x00160082, Unk_0x00160082,        "Unk_0x00160082"},
    {0x00180082, Unk_0x00180082,        "Unk_0x00180082"},
    {0x00200082, Unk_0x00200082,        "Unk_0x00200082"},
    {0x001C0042, nullptr,               "StartTask"},
    {0x001E0042, nullptr,               "CancelTask"},
    {0x00210042, nullptr,               "GetTaskResult"},
    {0x002300C2, nullptr,               "GetTaskStatus"},
    {0x002E0040, nullptr,               "GetErrorCode"},
    {0x00330042, nullptr,               "StartBgImmediate"},
};

BOSS_U_Interface::BOSS_U_Interface() {
    Register(FunctionTable);
}

} // namespace BOSS
} // namespace Service
