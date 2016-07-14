// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <string>

#include "common/assert.h"
#include "common/common_types.h"

#include "core/hle/kernel/kernel.h"
#include "core/hle/kernel/thread.h"
#include "core/hle/result.h"
#include "core/memory.h"

namespace IPC {


enum DescriptorType {
    // Buffer related desciptors types (mask : 0x0F)
    StaticBuffer = 0x02,
    PXIBuffer    = 0x04,
    MappedBuffer = 0x08,
    // Handle related descriptors types (mask : 0x30, but need to check for buffer related descriptors first )
    MoveHandle   = 0x00,
    CopyHandle   = 0x10,
    CallingPid   = 0x20,
};

/**
* @brief Creates a command header to be used for IPC
* @param command_id            ID of the command to create a header for.
* @param normal_params         Size of the normal parameters in words. Up to 63.
* @param translate_params_size Size of the translate parameters in words. Up to 63.
* @return The created IPC header.
*
* Normal parameters are sent directly to the process while the translate parameters might go through modifications and checks by the kernel.
* The translate parameters are described by headers generated with the IPC_Desc_* functions.
*
* @note While #normal_params is equivalent to the number of normal parameters, #translate_params_size includes the size occupied by the translate parameters headers.
*/
constexpr u32 MakeHeader(u16 command_id, unsigned int normal_params, unsigned int translate_params_size) {
    return ((u32)command_id << 16) | (((u32)normal_params & 0x3F) << 6) | (((u32)translate_params_size & 0x3F) << 0);
}

inline void ParseHeader(u32 header,u16& command_id, unsigned int& normal_params, unsigned int& translate_params_size) {
    command_id       = (u16) (header >> 16);
    normal_params = (header >> 6) & 0x3F;
    translate_params_size = header & 0x3F;
}

static inline DescriptorType GetDescriptorType(u32 descriptor) {
    if ((descriptor & 0xF) == 0x0) {
        return (DescriptorType) (descriptor & 0x30);
    // handle the fact that the following descriptors can have rights
    } else if (descriptor & MappedBuffer) {
        return MappedBuffer;
    } else if (descriptor & PXIBuffer) {
        return PXIBuffer;
    } else return StaticBuffer;
}

constexpr u32 MoveHandleDesc(unsigned int num_handles = 1) {
    return MoveHandle | ((num_handles - 1) << 26);
}

constexpr u32 CopyHandleDesc(unsigned int num_handles = 1) {
    return CopyHandle | ((num_handles - 1) << 26);
}

constexpr unsigned int HandleNumberFromDesc(u32 handle_descriptor) {
    return (handle_descriptor >> 26) + 1;
}

constexpr u32 CallingPidDesc() {
    return CallingPid;
}

constexpr u32 TransferHandleDesc() {
    return 0x20;
}

constexpr u32 StaticBufferDesc(u32 size, unsigned int buffer_id) {
    return StaticBuffer | (size << 14) | ((buffer_id & 0xF) << 10);
}

/**
* @brief Creates a header describing a buffer to be sent over PXI.
* @param size         Size of the buffer. Max 0x00FFFFFF.
* @param buffer_id    The Id of the buffer. Max 0xF.
* @param is_read_only true if the buffer is read-only. If false, the buffer is considered to have read-write access.
* @return The created PXI buffer header.
*
* The next value is a phys-address of a table located in the BASE memregion.
*/
static inline u32 IPC_Desc_PXIBuffer(size_t size, unsigned buffer_id, bool is_read_only)
{
    u8 type = PXIBuffer;
    if (is_read_only) type |= 0x2;
    return  type | (size << 8) | ((buffer_id & 0xF) << 4);
}

enum MappedBufferPermissions {
    R = 2,
    W = 4,
    RW = R | W,
};

constexpr u32 MappedBufferDesc(u32 size, MappedBufferPermissions perms) {
    return MappedBuffer | (size << 4) | (u32)perms;
}


enum BufferMappingType {
    InputBuffer     = 1,
    OutputBuffer    = 2,
    ReadWriteBuffer = InputBuffer| OutputBuffer
};

class MessageHelper;

namespace detail {
    /// Pop
    template<class T>
    T Pop_Impl(MessageHelper &);

    /// Push
    template<class T>
    void Push_Impl(MessageHelper & rh,T value);
}


class MessageHelper{

    // make implementation details friends
    template<class T>
    friend T detail::Pop_Impl(MessageHelper &);
    template<class T>
    friend void detail::Push_Impl(MessageHelper & rh,T value);

    u32* cmdbuf;
    ptrdiff_t index = 1;
    u16       command_id;
    unsigned  normal_params;
    unsigned  translate_params_size;

public:
    MessageHelper(u16 command_id, unsigned normal_params, unsigned translate_params_size, bool parsing = true);

    MessageHelper(u32 command_header, bool parsing = true);

    ~MessageHelper();

    /// Returns the total size of the request in words
    size_t totalSize() const { return 1 /* command header */ + normal_params + translate_params_size; };

    void ValidateHeader(){
        ASSERT_MSG( index == totalSize(),"Operations do not match the header (cmd 0x%x)",MakeHeader(command_id,normal_params,translate_params_size));
    }

    void NewMessage(unsigned normal_params, unsigned translate_params_size = 0);

    /// Pop ///

    template<class T>
    T Pop(){ return detail::Pop_Impl<T>(*this); }

    template<class T>
    void Pop(T& value) { value = Pop<T>(); }

    template<class First, class... Other>
    void Pop(First& first_value, Other&... other_values)
    {
        first_value = Pop<First>();
        Pop(other_values...);
    }

    template<class... H>
    void PopHandles(H&... handles)
    {
        const u32 handle_descriptor = Pop<u32>();
        const int handles_number = sizeof...(H);
        ASSERT_MSG(handles_number == HandleNumberFromDesc(handle_descriptor), "Number of handles doesn't match the descriptor");
        Pop(handles ...);
    }

    template<class T>
    void PopRaw(T&& value)
    {
        static_assert(std::is_trivially_copyable<T>::value);
        std::memcpy(&value, cmdbuf + index, sizeof(T));
        index += (sizeof(T) - 1)/4 + 1; // round up to word length
    }

    /// Push ///

    template<class T>
    void Push(T value){ return detail::Push_Impl(*this,value); }

    template<class First, class... Other>
    void Push(First first_value, Other&&... other_values)
    {
        Push(first_value);
        Push(std::forward<Other>(other_values)...);
    }

    template<class T>
    void PushRaw(T&& value)
    {
        static_assert(std::is_trivially_copyable<T>::value);
        std::memcpy(cmdbuf + index, &value, sizeof(T));
        index += (sizeof(T) - 1)/4 + 1; // round up to word length
    }

    template<class... H>
    void PushHandles(bool copy_handles,H&&... handles)
    {
        if (copy_handles) Push<u32>(CopyHandleDesc(sizeof...(H)));
        else              Push<u32>(MoveHandleDesc(sizeof...(H)));
        Push(std::forward<H>(handles)...);
    }

    /**
     * Translate the parameters
     * @return true on success, false otherwise
     * @note The proper message will be created for the error on failure
     * @note Will reset the index of the message to 1, as if the helper was just created
     */
    bool Translate();
};

/** CheckBufferMappingTranslation function
 *    If the buffer mapping translation was valid, this function will return a true
 *      InputBuffer     : read-only
 *      OutputBuffer    : write-only
 *      ReadWriteBuffer : read-write
 *    Note:
 *        Structure of Buffer Mapping Translation:
 *          bit 1-2 : Access permission flags for the receiving process: 1=read-only, 2=write-only, 3=read-write.
 *                    Specifying 0 will cause a kernel panic.
 *          bit 3   : This kind of translation is enabled by setting bit3 in the translation descriptor.
 *          bit 4-? : Size in bytes of the shared memory block.
 */
bool CheckBufferMappingTranslation(BufferMappingType mapping_type, u32 size, u32 translation);

} // namespace IPC

namespace Kernel {

static const int kCommandHeaderOffset = 0x80; ///< Offset into command buffer of header

/**
 * Returns a pointer to the command buffer in the current thread's TLS
 * TODO(Subv): This is not entirely correct, the command buffer should be copied from
 * the thread's TLS to an intermediate buffer in kernel memory, and then copied again to
 * the service handler process' memory.
 * @param offset Optional offset into command buffer
 * @return Pointer to command buffer
 */
inline static u32* GetCommandBuffer(const int offset = 0) {
    return (u32*)Memory::GetPointer(GetCurrentThread()->GetTLSAddress() + kCommandHeaderOffset + offset);
}

/**
* Returns a pointer to the static buffers area in the current thread's TLS
* TODO(Subv): cf. GetCommandBuffer
* @param offset Optional offset into static buffers area
* @return Pointer to static buffers area
*/
inline static u32* GetStaticBuffers(const int offset = 0) {
    return GetCommandBuffer(0x100 + offset);
}

/**
 * Kernel object representing the client endpoint of an IPC session. Sessions are the basic CTR-OS
 * primitive for communication between different processes, and are used to implement service calls
 * to the various system services.
 *
 * To make a service call, the client must write the command header and parameters to the buffer
 * located at offset 0x80 of the TLS (Thread-Local Storage) area, then execute a SendSyncRequest
 * SVC call with its Session handle. The kernel will read the command header, using it to marshall
 * the parameters to the process at the server endpoint of the session. After the server replies to
 * the request, the response is marshalled back to the caller's TLS buffer and control is
 * transferred back to it.
 *
 * In Citra, only the client endpoint is currently implemented and only HLE calls, where the IPC
 * request is answered by C++ code in the emulator, are supported. When SendSyncRequest is called
 * with the session handle, this class's SyncRequest method is called, which should read the TLS
 * buffer and emulate the call accordingly. Since the code can directly read the emulated memory,
 * no parameter marshalling is done.
 *
 * In the long term, this should be turned into the full-fledged IPC mechanism implemented by
 * CTR-OS so that IPC calls can be optionally handled by the real implementations of processes, as
 * opposed to HLE simulations.
 */
class Session : public WaitObject {
public:
    Session();
    ~Session() override;

    std::string GetTypeName() const override { return "Session"; }

    static const HandleType HANDLE_TYPE = HandleType::Session;
    HandleType GetHandleType() const override { return HANDLE_TYPE; }

    /**
     * Handles a synchronous call to this session using HLE emulation. Emulated <-> emulated calls
     * aren't supported yet.
     */
    virtual ResultVal<bool> SyncRequest() = 0;

    // TODO(bunnei): These functions exist to satisfy a hardware test with a Session object
    // passed into WaitSynchronization. Figure out the meaning of them.

    bool ShouldWait() override {
        return true;
    }

    void Acquire() override {
        ASSERT_MSG(!ShouldWait(), "object unavailable!");
    }
};

}
