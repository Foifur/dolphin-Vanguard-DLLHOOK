#include "Core/Core.h"
#include "Core/HW/Memmap.h"
#include "Core/PowerPC/Jit64Common/Jit64PowerPCState.h"
#include "Core/State.h"
#include <debugapi.h>
#include <VanguardHelpers.h>


unsigned char Vanguard_peekbyte(long long addr)
{
  return Memory::Read_U8(static_cast<u32>(addr));
}

void Vanguard_pokebyte(long long addr, unsigned char val)
{
  Memory::Write_U8(val, static_cast<u32>(addr));
  PowerPC::ppcState.iCache.Invalidate(addr);
}

void Vanguard_savesavestate(BSTR filename, bool wait)
{
  if (Core::IsRunningAndStarted())
  {
    //Convert the BSTR sent by Vanguard to std::string
    std::string filename_converted = BSTRToString(filename);
    State::SaveAs(filename_converted, wait);
  }
}

void Vanguard_loadsavestate(BSTR filename)
{
  // Convert the BSTR sent by Vanguard to std::string
  std::string filename_converted = BSTRToString(filename);
  State::LoadAs(filename_converted);
}


/* CallImportedFunction -- calls a function that has been exported from the Vanguard */
/*                        client to be accessed by the emulator                      */
/*                                                                                   */
/* arguments:                                                                        */
/* const char* function name -- the name of the `DllExport`ed Vanguard function      */
/* std::string string        -- the function's input string (if it has one)          */
void CallImportedFunction(const char* function_name, std::string string)            
{
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

  //Load the Vanguard hook dll
  HINSTANCE vanguard = LoadLibraryA("DolphinVanguard-Hook.dll");
  if (!vanguard)
    DWORD error = GetLastError();

  //check to see if we converted a string and need to pass an input argument
  //this is kind of hacky...
  if (SysStringLen(converted_string))
  {
    //find the function in the dll and call it
    typedef void (*FUNC)(BSTR);
    FUNC function = (FUNC)GetProcAddress(vanguard, function_name);
    if (!function)
      DWORD error = GetLastError();
    else
    {
      function(converted_string);
      //make sure to free the BSTR from memory once we're done with it
      SysFreeString(converted_string);
    }
  }
  else
  {
    // find the function in the dll and call it
    typedef void (*FUNC)();
    FUNC function = (FUNC)GetProcAddress(vanguard, function_name);
    if (!function)
      DWORD error = GetLastError();
    else
      function();
  }
}

//converts a BSTR received from the Vanguard client to std::string
std::string BSTRToString(BSTR string)
{
  std::wstring ws(string, SysStringLen(string));
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::string converted_string = converter.to_bytes(ws);
  return converted_string;
}
