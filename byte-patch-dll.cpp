#include "byte-patch-dll.h"
#include "utils.h"
#include <format>
#include <tlhelp32.h>
#include <cstdint>
#include <intrin.h>
#include <iostream>
#include <DbgHelp.h>
#include <codecvt>
#include <format>
#include <locale>
#include <winternl.h>
#include <iostream>
#include <cstdarg>

#pragma comment(lib, "dbghelp.lib")


bytepatchdll_t::bytepatchdll_t(HMODULE mod)
{
    SetConsoleTitleA(("The BinFbs"));
}
void patch_vmp()
{
    unsigned long old_protect = 0;
    const auto ntdll = GetModuleHandleA(("ntdll.dll"));
    if (!ntdll)
        return;

    unsigned char callcode = ((unsigned char*)GetProcAddress(ntdll, ("NtQuerySection")))[4] - 1;
    unsigned char restore[] = { 0x4C, 0x8B, 0xD1, 0xB8, callcode };

    const auto nt_protect_virtual_mem = (unsigned char*)GetProcAddress(ntdll, ("NtProtectVirtualMemory"));
    if (!nt_protect_virtual_mem)
        return;

    VirtualProtect(nt_protect_virtual_mem, sizeof(restore), PAGE_EXECUTE_READWRITE, &old_protect);
    memcpy(nt_protect_virtual_mem, restore, sizeof(restore));
    VirtualProtect(nt_protect_virtual_mem, sizeof(restore), old_protect, &old_protect);

}
void patch(std::uintptr_t addr, const std::vector<uint8_t> bytes)
{
    unsigned long old_protect;
    VirtualProtect((void*)addr, bytes.size(), PAGE_EXECUTE_READWRITE, &old_protect);
    memcpy((void*)addr, bytes.data(), bytes.size());
    VirtualProtect((void*)addr, bytes.size(), old_protect, &old_protect);
}

void bytepatchdll_t::patches()
{
    DWORD ulOldProtect, ulNewProtect;

    uint8_t* Shit = util::pattern_scan((uintptr_t)GetModuleHandleA(0), "75 06");

    VirtualProtect(Shit, 0x21, PAGE_EXECUTE_READWRITE, &ulOldProtect);
    patch((uintptr_t)(Shit + 0x15 ), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
    VirtualProtect(Shit, 0x21, ulOldProtect, &ulNewProtect);

    uint8_t* Shit2 = util::pattern_scan((uintptr_t)GetModuleHandleA(0), "0F 84 ? ? ? ? 48 B8 AD 3A 23 7C CF 96 5F 42 48 89 45 E7 48 BB 96 87 B3 FE FA F7 37 0E 48 89 5D EF 48 B8 3F 68 7F C9 95 BE EA 6D 48 89 45 F7 48 B8 88 68 6A 8A D8 9C D9 2F 48 89 45 FF 48 B8 71 7E 56 83 DB 09 D9 78 48 89 45 07 48 B8 BA 3D 2C C1 06 B1 62 69 48 89 45 0F 49 BD A7 10 09 56 E5 BC 75 68 4C 89 6D B7 49 BC BC AD 99 D4 D0 DD 1D 24 4C 89 65 BF 49 BF 15 42 55 E3 BF 94 C0 47 4C 89 7D C7 48 BE A2 42 40 A0 F2 B6 F3 05 48 89 75 CF 48 BF 5B 7E 56 83 DB 09 D9 78 48 89 7D D7 48 89 45 DF");
    VirtualProtect(Shit - 2, 0x6, PAGE_EXECUTE_READWRITE, &ulOldProtect);
    patch((uintptr_t)(Shit2 - 2 ), std::vector<uint8_t> { 0x3b });
    VirtualProtect(Shit - 2, 0x6, ulOldProtect, &ulNewProtect);

    MessageBoxA(NULL, "Made by The BinFbs", "Info", MB_OK);
}

void core(HMODULE mod)
{
     bytepatchdll_t* cheat = new bytepatchdll_t(mod);
     patch_vmp();
     cheat->patches();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        DisableThreadLibraryCalls(GetModuleHandleA(0));

        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)core, hModule, 0, 0);
    }

    return 1;
}

