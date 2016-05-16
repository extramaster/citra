// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "core/hle/kernel/session.h"
#include "core/hle/kernel/thread.h"

namespace IPC {

namespace detail{
    /// Pop ///

    template<> u32 Pop_Impl<u32>(MessageHelper & rh) { return rh.cmdbuf[rh.index++]; }
    template<> ResultCode Pop_Impl<ResultCode>(MessageHelper & rh) { return ResultCode{ rh.cmdbuf[rh.index++] }; }

    template<> u64 Pop_Impl<u64>(MessageHelper & rh) {
        u64 value = ((u64)rh.cmdbuf[rh.index+1] << 32) | rh.cmdbuf[rh.index];
        rh.index +=2;
        return  value;
    }

    /// Push ///

    template<> void Push_Impl<u32>(MessageHelper & rh, u32 value) { rh.cmdbuf[rh.index++] = value; }
    template<> void Push_Impl<ResultCode>(MessageHelper & rh, ResultCode value){ rh.cmdbuf[rh.index++] = value.raw; }

    template<> void Push_Impl<u64>(MessageHelper & rh,u64 value){
        rh.cmdbuf[rh.index++] = (u32) (value & 0xFFFFFFFF);
        rh.cmdbuf[rh.index++] = (u32) ((value >> 32) & 0xFFFFFFFF);
    }

} // namespace detail

MessageHelper::MessageHelper(u16 command_id, unsigned normal_params, unsigned translate_params_size, bool parsing)
    : cmdbuf(Kernel::GetCommandBuffer()), normal_params(normal_params), translate_params_size(translate_params_size) {
    if (parsing) ASSERT_MSG(cmdbuf[0] == MakeHeader(command_id, normal_params, translate_params_size), "Invalid command header in cmdbuf");
    else cmdbuf[0] = MakeHeader(command_id, normal_params, translate_params_size); // Assign the header if we're building an IPC message
}

MessageHelper::MessageHelper(u32 command_header, bool parsing)
    : cmdbuf(Kernel::GetCommandBuffer()) {
    if (parsing) ASSERT_MSG(cmdbuf[0] == command_header, "Invalid command header in cmdbuf");
    else cmdbuf[0] = command_header; // Assign the header if we're building an IPC message
    ParseHeader(command_header, command_id, normal_params, translate_params_size);
}

MessageHelper::~MessageHelper() {
    ValidateHeader(); // Make sure the message was parsed/built correctly
}

void MessageHelper::NewMessage(unsigned normal_params, unsigned translate_params_size) {
    ValidateHeader(); // Make sure the previous message was parsed/built correctly
    this->normal_params = normal_params;
    this->translate_params_size = translate_params_size;
    cmdbuf[0] = MakeHeader(command_id, normal_params, translate_params_size);
    index = 1;
}

// TODO : This is a STUB
//        Once multi-process is implemented, the real operations will have to be implemented
//        Translate() will require the CurrentPID and the destinationPID
bool MessageHelper::Translate() {
    index = 1 + normal_params; // skip normal params
    const size_t total_size = totalSize();
    while (index != total_size)
    {
        const DescriptorType descriptor = GetDescriptorType(cmdbuf[index]);
        switch (descriptor) {
        case MoveHandle: {
            const auto handles_number = HandleNumberFromDesc(descriptor);
            // TODO : Move handles
            index += 1 + handles_number;
            break;
        }
        case CopyHandle: {
            const auto handles_number = HandleNumberFromDesc(descriptor);
            // TODO : Copy handles
            index += 1 + handles_number;
            break;
        }
        case CallingPid:
            // TODO : use the real handle, not a pseudo handle
            cmdbuf[index + 1] = Kernel::CurrentProcess;
            index += 2;
            break;
        case StaticBuffer:
            u8 buffer_id = (descriptor >> 10) & 0xF;
            u32 size = (descriptor >> 14);
            // TODO : copy the buffer to the buffer pointed by the remote TLS
            index += 2;
            break;
        case PXIBuffer:


            index += 2;
            break;
        case MappedBuffer: {
            const u8 perms = descriptor & MappedBufferPermissions::RW;
            if (!perms) LOG_CRITICAL(Kernel, "Panic: Buffer mapping has no permission set.");
            //
            index += 2;
            break;
        }
        default:
            LOG_DEBUG(Kernel, "Unknown translation descriptor type");
            index = 1;
            return false;
        }
    }
    index = 1;
    return true;
}

} // namespace IPC


namespace Kernel {

Session::Session() {}
Session::~Session() {}

}
