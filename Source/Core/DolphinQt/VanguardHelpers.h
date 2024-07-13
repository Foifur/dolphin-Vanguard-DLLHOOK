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

extern "C" __declspec(dllexport) void Vanguard_loadROM(BSTR filename);

inline HINSTANCE vanguard = LoadLibraryA("../RTCV/VanguardHook.dll");

/* CallImportedFunction -- calls a function that has been exported from the Vanguard */
/*                        client to be accessed by the emulator                      */
/*                                                                                   */
/* arguments:                                                                        */
/* const char* function name -- the name of the `DllExport`ed Vanguard function      */
/* std::string string        -- the function's input string (if it has one)          */
template <typename T>
T CallImportedFunction(char* function_name, std::string string = "")
{
  // AddDllDirectory(L"S:/RTC Test/RTCV");
  // LoadLibraryExA("Newtonsoft.Json.dll", nullptr, LOAD_LIBRARY_SEARCH_USER_DIRS);
  // LoadLibraryExA("CorruptCore.dll ", nullptr, LOAD_LIBRARY_SEARCH_USER_DIRS);

  // since we don't have access to a C++/CLI wrapper, we need to convert the game name string
  // into a BSTR so that the C# function can read it. We also need to make sure we allocate
  // the BSTR into memory before conversion, and then free the memory after we've sent it off
  BSTR converted_string = SysAllocString(L"");

  if (!string.empty())
  {
    // change the string into a char array and convert it to a BSTR
    const char* char_array = string.c_str();
    converted_string = _com_util::ConvertStringToBSTR(char_array);
  }

  // check to see if we converted a string and need to pass an input argument
  // this is kind of hacky...
  if (SysStringLen(converted_string))
  {
    // find the function in the dll and call it
    typedef T (*FUNC)(BSTR);
    FUNC function = (FUNC)GetProcAddress(vanguard, function_name);
    if (!function)
      DWORD error = GetLastError();
    else
    {
      return function(converted_string);
    }
  }
  else
  {
    // find the function in the dll and call it
    typedef T (*FUNC)();
    FUNC function = (FUNC)GetProcAddress(vanguard, function_name);
    if (!function)
      DWORD error = GetLastError();
    else
      return function();
  }
  // make sure to free the BSTR from memory once we're done with it
  SysFreeString(converted_string);
}

std::string BSTRToString(BSTR string);
