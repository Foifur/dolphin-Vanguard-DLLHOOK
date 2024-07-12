#include "Core/Core.h"
#include "Core/HW/Memmap.h"
#include "Core/PowerPC/Jit64Common/Jit64PowerPCState.h"
#include "Core/State.h"
#include <debugapi.h>
#include <VanguardHelpers.h>
#include <filesystem>
#include <stdlib.h>


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

//converts a BSTR received from the Vanguard client to std::string
std::string BSTRToString(BSTR string)
{
  std::wstring ws(string, SysStringLen(string));
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::string converted_string = converter.to_bytes(ws);
  return converted_string;
}
