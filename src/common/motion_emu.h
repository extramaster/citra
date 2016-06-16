// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

class EmuWindow;

namespace MotionEmu {

void Init(EmuWindow& emu_window);
void Shutdown();
void BeginTilt(int x, int y);
void Tilt(int x, int y);
void EndTilt();

}
