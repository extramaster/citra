// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "core/hle/service/service.h"
#include "common/common_types.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace LDR_RO

namespace LDR_RO {

class Interface : public Service::Interface {
public:
    Interface();

    std::string GetPortName() const override {
        return "ldr:ro";
    }
};

} // namespace
