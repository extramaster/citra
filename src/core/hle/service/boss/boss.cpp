// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "core/hle/service/boss/boss.h"
#include "core/hle/service/boss/boss_p.h"
#include "core/hle/service/boss/boss_u.h"

namespace Service {
namespace BOSS {

void InitializeSession(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    u32 unk_param1      = cmd_buff[1];
    u32 unk_param2      = cmd_buff[2];
    u32 unk_translation = cmd_buff[3];
    u32 unk_param4      = cmd_buff[4];

    if (unk_translation != 0x20) {
        cmd_buff[0] = IPC::MakeHeader(0, 0x1, 0); // 0x40
        cmd_buff[1] = ResultCode(ErrorDescription::OS_InvalidBufferDescriptor,
                                 ErrorModule::OS, ErrorSummary::WrongArgument, ErrorLevel::Permanent).raw; // 0xD9001830
        LOG_ERROR(Service_BOSS, "The translation was invalid, translation=0x%08X",unk_translation);
        return;
    }

    cmd_buff[0] = IPC::MakeHeader(0x1, 0x1, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;

    LOG_WARNING(Service_BOSS, "(STUBBED) unk_param1=0x%08X, unk_param2=0x%08X, unk_translation=0x%08X, unk_param4=0x%08X",
                unk_param1, unk_param2, unk_translation, unk_param4);
}

void GetStorageInfo(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    u32 unk_param1 = cmd_buff[1];
    u32 unk_param2 = cmd_buff[2];
    u32 unk_param3 = cmd_buff[3];
    u32 unk_flag = cmd_buff[4] & 0xF;

    cmd_buff[0] = IPC::MakeHeader(0x2, 0x1, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;

    LOG_WARNING(Service_BOSS, "(STUBBED) unk_param1=0x%08X, unk_param2=0x%08X, unk_param3=0x%08X, unk_flag=0x%08X",
                unk_param1, unk_param2, unk_param3, unk_flag);
}

void Unk_0x00080002(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    u32 unk_param1 = cmd_buff[1];
    u32 unk_param2 = cmd_buff[2];

    cmd_buff[0] = IPC::MakeHeader(0x8, 0x1, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;

    LOG_WARNING(Service_BOSS, "(STUBBED) unk_param1=0x%08X, unk_param2=0x%08X",unk_param1, unk_param2);
}

void Unk_0x00090040(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    u32 unk_param1 = cmd_buff[1];

    cmd_buff[0] = IPC::MakeHeader(0x8, 0x1, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;

    LOG_WARNING(Service_BOSS, "(STUBBED) unk_param1=0x%08X", unk_param1);
}

void Unk_0x000A0000(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();

    cmd_buff[0] = IPC::MakeHeader(0xA, 0x2, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;
    cmd_buff[2] = 0; // stub 0

    LOG_WARNING(Service_BOSS, "(STUBBED) called");
}

void Unk_0x000E0000(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();

    cmd_buff[0] = IPC::MakeHeader(0xE, 0x1, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;

    LOG_WARNING(Service_BOSS, "(STUBBED) called");
}

void Unk_0x00110102(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    u32 unk_param1 = cmd_buff[1];
    u32 unk_param2 = cmd_buff[2];
    u32 unk_param3 = cmd_buff[3];
    u32 unk_param4 = cmd_buff[4];
    u32 unk_param5 = cmd_buff[5];
    u32 unk_param6 = cmd_buff[6];

    cmd_buff[0] = IPC::MakeHeader(0x11, 0x3, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;
    cmd_buff[2] = 0; // 16 bit value
    cmd_buff[3] = 0; // 16 bit value

    LOG_WARNING(Service_BOSS, "(STUBBED) unk_param1=0x%08X, unk_param2=0x%08X, unk_param3=0x%08X, unk_param4=0x%08X, unk_param5=0x%08X, unk_param6=0x%08X",
                unk_param1, unk_param2, unk_param3, unk_param4, unk_param5, unk_param6);
}

void Unk_0x00160082(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    u32 unk_param1 = cmd_buff[1];
    u32 unk_size = cmd_buff[2];
    u32 unk_translation = cmd_buff[3];
    u32 unk_addr = cmd_buff[4];

    cmd_buff[0] = IPC::MakeHeader(0x16, 0x3, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;
    cmd_buff[2] = 0; // 16 bit value
    cmd_buff[3] = 0; // 16 bit value

    LOG_WARNING(Service_BOSS, "(STUBBED) unk_param1=0x%08X, unk_size=0x%08X, unk_translation=0x%08X, unk_addr=0x%08X",
                unk_param1, unk_size, unk_translation, unk_addr);
}

void Unk_0x00180082(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    u32 unk_param1 = cmd_buff[1];
    u32 unk_param2 = cmd_buff[2];
    u32 unk_param3 = cmd_buff[3];
    u32 unk_param4 = cmd_buff[4];

    cmd_buff[0] = IPC::MakeHeader(0x18, 0x1, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;

    LOG_WARNING(Service_BOSS, "(STUBBED) unk_param1=0x%08X, unk_param2=0x%08X, unk_param3=0x%08X, unk_param4=0x%08X",
        unk_param1, unk_param2, unk_param3, unk_param4);
}

void Unk_0x00200082(Service::Interface* self) {
    u32* cmd_buff = Kernel::GetCommandBuffer();
    u32 unk_size = cmd_buff[1];
    u32 unk_param2 = cmd_buff[2];
    u32 unk_translation = cmd_buff[3];
    u32 unk_addr = cmd_buff[4];

    cmd_buff[0] = IPC::MakeHeader(0x20, 0x4, 0);
    cmd_buff[1] = RESULT_SUCCESS.raw;
    cmd_buff[2] = 0; // 8 bit value
    cmd_buff[3] = 0; // 32 bit value
    cmd_buff[4] = 0; // 8 bit value

    LOG_WARNING(Service_BOSS, "(STUBBED) unk_size=0x%08X, unk_param2=0x%08X, unk_translation=0x%08X, unk_addr=0x%08X",
        unk_size, unk_param2, unk_translation, unk_addr);
}

void Init() {
    using namespace Kernel;

    AddService(new BOSS_P_Interface);
    AddService(new BOSS_U_Interface);
}

void Shutdown() {
}

} // namespace BOSS

} // namespace Service
