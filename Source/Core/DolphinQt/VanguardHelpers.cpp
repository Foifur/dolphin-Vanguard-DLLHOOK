#include "Core/Core.h"
#include "Core/HW/Memmap.h"
#include "Core/PowerPC/Jit64Common/Jit64PowerPCState.h"
#include "Core/State.h"
#include "Core/ConfigManager.h"
#include "Core/HW/dsp.h"
#include <VanguardHelpers.h>
#include <VanguardClientInitializer.h>



unsigned char Vanguard_peekbyte(long long addr, int selection)
{
  if (selection == 0)
    return Memory::Read_U8(static_cast<u32>(addr));
  else
    return DSP::ReadARAM(static_cast<u32>(addr));
}

void Vanguard_pokebyte(long long addr, unsigned char val, int selection)
{
  if (selection == 0)
  {
    Memory::Write_U8(val, static_cast<u32>(addr));
    PowerPC::ppcState.iCache.Invalidate(addr);
  }
  else
  {
    DSP::WriteARAM(val, static_cast<u32>(addr));
  }
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

bool VanguardClient::loading = false;
void Vanguard_loadROM(BSTR filename)
{
  VanguardClient::loading = true;

  std::string converted_filename = BSTRToString(filename);

  SetState(Core::State::Paused);
  VanguardClientInitializer::win->StartGame(converted_filename);

  MSG msg;
  // We have to do it this way to prevent deadlock due to synced calls. It sucks but it's required
  // at the moment
  while (VanguardClient::loading)
  {
    Sleep(20);
    //these lines of code perform the equivalent of the Application.DoEvents method
    ::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
    ::GetMessage(&msg, NULL, 0, 0);
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  Sleep(100);  // Give the emu thread a chance to recover
  
}

void Vanguard_finishLoading()
{
  VanguardClient::loading = false;
}

void Vanguard_prepShutdown()
{
  VanguardClientInitializer::win->m_exit_requested = true;
}

void Vanguard_forceStop()
{
  VanguardClientInitializer::win->MainWindow::ForceStopVanguard();
}

//converts a BSTR received from the Vanguard client to std::string
std::string BSTRToString(BSTR string)
{
  std::wstring ws(string, SysStringLen(string));
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::string converted_string = converter.to_bytes(ws);
  return converted_string;
}

bool VanguardClient::bWii = false;
bool Vanguard_isWii()
{
  return VanguardClient::bWii;
}

std::string getDirectory()
{
  char buffer[MAX_PATH] = {0};
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
  std::string::size_type pos = std::string(buffer).find_last_of("\\/");
  return std::string(buffer).substr(0, pos);
}
