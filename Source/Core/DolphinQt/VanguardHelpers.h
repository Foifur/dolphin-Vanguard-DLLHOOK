#pragma once

#include "Core/HW/Memmap.h"
#include "Core/PowerPC/Jit64Common/Jit64PowerPCState.h"
#include "Core/State.h"
#include <comutil.h>
#include <locale>
#include <codecvt>

extern "C" __declspec(dllexport) unsigned char Vanguard_peekbyte(long long addr);

extern "C" __declspec(dllexport) void Vanguard_pokebyte(long long addr, unsigned char val);

extern "C" __declspec(dllexport) void Vanguard_savesavestate(BSTR filename, bool wait);

extern "C" __declspec(dllexport) void Vanguard_loadsavestate(BSTR filename);

void CallImportedFunction(const char* function_name, std::string string = nullptr);

std::string BSTRToString(BSTR string);
