#include <Windows.h>
#include <string>
#include "stringproc.h"
#include "lazy.h"
#include <winternl.h>
#include <TlHelp32.h>
#include <tchar.h>
#include "selfcode/selfcode.h"
#include "antiattach.h"
#include "antivm.h"
#define STATUS_SUCCESS   ((NTSTATUS)0x00000000L) 

typedef NTSTATUS(__stdcall* _NtQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);
typedef NTSTATUS(__stdcall* _SetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);
typedef NTSTATUS(__stdcall* _NtSetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);


void debugger_detected(const std::string& msg)
{
    std::string message = "Disable Application: " + msg;
    std::string command = "start cmd /c msg * \"" + message + "\"";
    system(command.c_str());
    exit(0);
}

void messageboxissue(const std::string& msg)
{
    std::string message = "WEEDVM: " + msg;
    std::string command = "cmd /c msg * \"" + message + "\"";
    system(command.c_str());
    exit(0);
}


int debugString()
{
    SetLastError(0);
    OutputDebugStringA(miggerencstr("ReFo Security"));
    const auto lastError = GetLastError();
    return lastError != 0;
}

int checkThreadContext()
{
    int found = false;
    CONTEXT ctx{};
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    void* hThread = GetCurrentThread();
    if (GetThreadContext(hThread, &ctx))
    {
        if (ctx.Dr0 != 0x00 || ctx.Dr1 != 0x00 || ctx.Dr2 != 0x00 || ctx.Dr3 != 0x00 || ctx.Dr6 != 0x00 || ctx.Dr7 != 0x00)
        {
            found = true;
        }
    }
    return found;
}


int antiDump()
{
    const auto peb = reinterpret_cast<PPEB>(__readgsqword(0x60));
    const auto inLoadOrderModuleList = reinterpret_cast<PLIST_ENTRY>(peb->Ldr->Reserved2[1]);
    const auto tableEntry = CONTAINING_RECORD(inLoadOrderModuleList, LDR_DATA_TABLE_ENTRY, Reserved1[0]);
    const auto pSizeOfImage = reinterpret_cast<PULONG>(&tableEntry->Reserved3[1]);
    *pSizeOfImage = static_cast<ULONG>(reinterpret_cast<INT_PTR>(tableEntry->DllBase) + 0x100000);
    return 0;
}


void findExeTitle()
{

    if (FindWindowA(NULL, miggerencstr("FolderChangesView"))) { debugger_detected("FolderChangesView"); }
    if (FindWindowA(NULL, miggerencstr("KsDumper"))) { debugger_detected("KsDumper"); }
    if (FindWindowA(NULL, miggerencstr("The Wireshark Network Analyzer"))) { debugger_detected("The Wireshark Network Analyzer"); }
    if (FindWindowA(NULL, miggerencstr("Process Hacker"))) { debugger_detected("Process Hacker"); }
    if (FindWindowA(NULL, miggerencstr("Progress Telerik Folder Web Debugger"))) { debugger_detected("Progress Telerik Folder Web Debugger"); }
    if (FindWindowA(NULL, miggerencstr("Finddler"))) { debugger_detected("Finddler"); }
    if (FindWindowA(miggerencstr("Qt5QWindowIcon"), NULL)) { debugger_detected("Vmprotect? X64dbg? Themida? "); }
    if (FindWindowA(NULL, miggerencstr("HTTP Debugger"))) { debugger_detected("HTTP Debugger"); }
    if (FindWindowA(NULL, miggerencstr("dnSpy"))) { debugger_detected("dnSpy"); }
    if (FindWindowA(miggerencstr("FolderChangesView"), NULL)) { debugger_detected("FolderChangesView"); }
    if (FindWindowA(NULL, miggerencstr("HxD"))) { debugger_detected("HxD"); }
    if (FindWindowA(NULL, miggerencstr("Cheat Engine 7.2"))) { debugger_detected("Cheat Engine 7.2"); }
    if (FindWindowA(NULL, miggerencstr("Cheat Engine 7.1"))) { debugger_detected("Cheat Engine 7.1"); }
    if (FindWindowA(NULL, miggerencstr("Cheat Engine 7.0"))) { debugger_detected("Cheat Engine 7.0"); }
    if (FindWindowA(NULL, miggerencstr("Cheat Engine 6.9"))) { debugger_detected("Cheat Engine 6.9"); }
    if (FindWindowA(NULL, miggerencstr("Cheat Engine 6.8"))) { debugger_detected("Cheat Engine 6.8"); }
    if (FindWindowA(NULL, miggerencstr("Ida"))) { debugger_detected("Ida"); }
    if (FindWindowA(NULL, miggerencstr("Ida Pro"))) { debugger_detected("Ida Pro"); }
    if (FindWindowA(NULL, miggerencstr("Ida Freeware"))) { debugger_detected("Ida Freeware"); }
    if (FindWindowA(NULL, miggerencstr("HTTP Debugger Pro"))) { debugger_detected("HTTP Debugger Pro"); }
    if (FindWindowA(NULL, miggerencstr("ProcessHacker"))) { debugger_detected("ProcessHacker"); }
    if (FindWindowA(NULL, miggerencstr("Process Hacker 2"))) { debugger_detected("Process Hacker 2"); }
    if (FindWindowA(NULL, miggerencstr("OllyDbg"))) { debugger_detected("OllyDbg"); }
    if (FindWindowA(NULL, miggerencstr("Detect It Easy v3.07 [Windows 10 Version 2009] (x86_64)"))) { debugger_detected("Detect It Easy v3.07 [Windows 10 Version 2009] (x86_64)"); }

}

BOOL ProcessDebugFlags()
{
    typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(HANDLE, UINT, PVOID, ULONG, PULONG);

    const int ProcessDebugFlags = 0x1f;

    pNtQueryInformationProcess NtQueryInfoProcess = NULL;

    NTSTATUS Status;
    DWORD NoDebugInherit = 0;
    HMODULE hNtDll = LoadLibrary(L"ntdll.dll");
    if (hNtDll == NULL)
    {
        // Handle error if ntdll.dll cannot be loaded
        printf("Failed to load ntdll.dll\n");
        return false;
    }

    NtQueryInfoProcess = (pNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
    if (NtQueryInfoProcess == NULL)
    {
        // Handle error if NtQueryInformationProcess cannot be found
        printf("Failed to find NtQueryInformationProcess\n");
        FreeLibrary(hNtDll);
        return false;
    }

    // Call function
    Status = NtQueryInfoProcess(GetCurrentProcess(), ProcessDebugFlags, &NoDebugInherit, sizeof(DWORD), NULL);
    FreeLibrary(hNtDll);

    if (Status != 0x00000000)
        return false;
    if (NoDebugInherit == FALSE) {
        // Handle debugger detected
        debugger_detected("ProcessDebugFlags");
        return true;
    }
    else {
        return false;
    }
}

BOOL SystemKernelDebuggerInformation()
{

    typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION {
        BOOLEAN DebuggerEnabled;
        BOOLEAN DebuggerNotPresent;
    } SYSTEM_KERNEL_DEBUGGER_INFORMATION, * PSYSTEM_KERNEL_DEBUGGER_INFORMATION;
    SYSTEM_KERNEL_DEBUGGER_INFORMATION Info;

    enum SYSTEM_INFORMATION_CLASS { SystemKernelDebuggerInformation = 35 };

    /* Function Pointer Typedef for ZwQuerySystemInformation */
    typedef NTSTATUS(WINAPI* pZwQuerySystemInformation)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, IN OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength);

    /* import the function */
    pZwQuerySystemInformation ZwQuerySystemInformation = NULL;

    HANDLE hProcess = GetCurrentProcess();

    HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));
    if (hNtDll == NULL)
    {

    }

    ZwQuerySystemInformation = (pZwQuerySystemInformation)GetProcAddress(hNtDll, miggerencstr("ZwQuerySystemInformation"));

    if (ZwQuerySystemInformation == NULL)
    {

    }

    // Call function
    if (STATUS_SUCCESS == ZwQuerySystemInformation(SystemKernelDebuggerInformation, &Info, sizeof(Info), NULL)) {
        if (Info.DebuggerEnabled)
        {
            if (Info.DebuggerNotPresent) {
                return FALSE;
            }
            else {
                return TRUE;
            }
        }

    }
}

BOOL ThreadHideFromDebugger()
{

    /* Function Pointer Typedef for NtQueryInformationProcess */
    typedef NTSTATUS(WINAPI* pNtSetInformationThread)(IN HANDLE, IN UINT, IN PVOID, IN ULONG);

    const int ThreadHideFromDebugger = 0x11;

    /* import the function */
    pNtSetInformationThread NtSetInformationThread = NULL;

    NTSTATUS Status;
    BOOL IsBeingDebug = FALSE;

    HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));
    if (hNtDll == NULL)
    {

    }

    NtSetInformationThread = (pNtSetInformationThread)GetProcAddress(hNtDll, "NtSetInformationThread");

    if (NtSetInformationThread == NULL)
    {

    }

    // Call Function
    Status = NtSetInformationThread(GetCurrentThread(), ThreadHideFromDebugger, NULL, 0);

    if (Status) {
        IsBeingDebug = TRUE;
    }

    return IsBeingDebug;
}


bool isBreakpointSet()
{
    CONTEXT context{};
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    // Get the current register values
    GetThreadContext(GetCurrentThread(), &context);

    // Check if any of the debug registers are set
    if (context.Dr0 || context.Dr1 || context.Dr2 || context.Dr3 || IsDebuggerPresent())
        return true;

    return false;
}


void anti_dbg_2() {

    __try {
        DebugBreak();
    }
    __except (GetExceptionCode() == EXCEPTION_BREAKPOINT ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {

    }

}


void adbg_CheckRemoteDebuggerPresent(void)
{
    HANDLE hProcess = INVALID_HANDLE_VALUE;
    BOOL found = FALSE;

    hProcess = GetCurrentProcess();
    CheckRemoteDebuggerPresent(hProcess, &found);

    if (found)
    {
        messageboxissue("Debugging Detected");

    }
}


void adbg_IsDebuggerPresent(void)
{
    BOOL found = FALSE;
    found = IsDebuggerPresent();

    if (found)
    {
        messageboxissue("Debugging Detected");

    }
}

void adbg_NtQueryInformationProcess(void)
{
    HANDLE hProcess = INVALID_HANDLE_VALUE;
    PROCESS_BASIC_INFORMATION pProcBasicInfo = { 0 };
    ULONG returnLength = 0;

    // Get a handle to ntdll.dll so we can import NtQueryInformationProcess
    HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
    if (hNtdll == INVALID_HANDLE_VALUE || hNtdll == NULL)
    {
        return;
    }

    // Dynamically acquire the addres of NtQueryInformationProcess
    _NtQueryInformationProcess  NtQueryInformationProcess = NULL;
    NtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(hNtdll, "NtQueryInformationProcess");

    if (NtQueryInformationProcess == NULL)
    {
        messageboxissue("Check failed. Reopen without a debugger.");

        return;
    }

    hProcess = GetCurrentProcess();
    NTSTATUS status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pProcBasicInfo, sizeof(pProcBasicInfo), &returnLength);
    if (NT_SUCCESS(status)) {
        PPEB pPeb = pProcBasicInfo.PebBaseAddress;
        if (pPeb)
        {
            if (pPeb->BeingDebugged)
            {
                messageboxissue("Virtual Machines are not allowed!");

            }
        }
    }
}

void adbg_NtSetInformationThread(void)
{
    THREAD_INFORMATION_CLASS ThreadHideFromDebugger = (THREAD_INFORMATION_CLASS)0x11;

    // Get a handle to ntdll.dll so we can import NtSetInformationThread
    HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
    if (hNtdll == INVALID_HANDLE_VALUE || hNtdll == NULL)
    {
        return;
    }

    // Dynamically acquire the addres of NtSetInformationThread and NtQueryInformationThread
    _NtSetInformationThread NtSetInformationThread = NULL;
    NtSetInformationThread = (_NtSetInformationThread)GetProcAddress(hNtdll, "NtSetInformationThread");

    if (NtSetInformationThread == NULL)
    {
        return;
    }

    // There is nothing to check here after this call.
    NtSetInformationThread(GetCurrentThread(), ThreadHideFromDebugger, 0, 0);
}



void adbg_MovSS(void)
{
    BOOL found = FALSE;

#ifdef _WIN64
    // This method does not work on x64
#else
    _asm
    {
        push ss;
        pop ss;
        pushfd;
        test byte ptr[esp + 1], 1;
        jne fnd;
        jmp end;
    fnd:
        mov found, 1;
    end:
        nop;
    }
#endif

    if (found)
    {
        messageboxissue("HAHA YOU GOT CAUGHT DEBUGGING BY THE SIMPLEST METHOD EVER :SKULL:");
        exit(0);
    }
}


void adbg_CloseHandleException(void)
{
    HANDLE hInvalid = (HANDLE)0xBEEF; // an invalid handle
    DWORD found = FALSE;

    __try
    {
        CloseHandle(hInvalid);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        found = TRUE;
    }

    if (found)
    {
        //  messageboxissue("Internal Proram Check Failed!");
        exit(0);

    }
}



void adbg_PrefixHop(void)
{
    BOOL found = TRUE;

    __try
    {
#ifdef _WIN64
        // TODO: Not yet implemented in x64
        found = FALSE;
#else
        _asm
        {
            __emit 0xF3;	// 0xF3 0x64 is the prefix 'REP'
            __emit 0x64;
            __emit 0xCC;	// this gets skipped over if being debugged
        }
#endif
    }

    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        found = FALSE;
    }

    if (found)
    {
        // messageboxissue("Program Did Not Except This!");
        exit(0);

    }
}

void adbg_CrashOllyDbg(void)
{
    // crash OllyDbg v1.x by exploit
    __try {
        OutputDebugString(TEXT("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s"));
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { ; }
}

void debugLoop()
{
    VMProtectBeginUltra("niggerfnjkjkhawaitjkagainimeantitlol");

    while (true)
    {
        if (vmDetection::isVM())
        {
            messageboxissue("Virtual Machines are not allowed!");
        }

        adbg_CrashOllyDbg();
        adbg_PrefixHop();
        adbg_CloseHandleException();
        adbg_MovSS();
        adbg_NtQueryInformationProcess();
        adbg_NtSetInformationThread();
        adbg_CheckRemoteDebuggerPresent();
        adbg_IsDebuggerPresent();
        selfcode();
        AntiAttach();
        debugString();
        findExeTitle();
        anti_dbg_2();
        ProcessDebugFlags();


        if (IsDebuggerPresent())
        {
            messageboxissue("Debugger Detected");
        }



        if (isBreakpointSet())
        {
            messageboxissue("Check failed. Reopen");
        }



        BOOL lmaothreaddetected = ThreadHideFromDebugger();

        if (lmaothreaddetected)
        {
            messageboxissue("Debugging Detected");
        }

        int hasDebugRegisters = checkThreadContext();
        if (hasDebugRegisters)
        {
            messageboxissue("Debugging Detected");
        }

        if (SystemKernelDebuggerInformation())
        {
            messageboxissue("Debugger Detected. Restart computer");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    VMProtectEnd();
}
















void debugcheckwithoutloop()
{
    VMProtectBeginUltra("GEORGEFLOYD");


    if (vmDetection::isVM())
    {
        messageboxissue("Virtual Machines are not allowed!");
    }

    adbg_CrashOllyDbg();
    adbg_PrefixHop();
    adbg_CloseHandleException();
    adbg_MovSS();
    adbg_NtQueryInformationProcess();
    adbg_NtSetInformationThread();
    adbg_CheckRemoteDebuggerPresent();
    adbg_IsDebuggerPresent();
    selfcode();
    AntiAttach();
    debugString();
    findExeTitle();
    anti_dbg_2();
    ProcessDebugFlags();


    if (IsDebuggerPresent())
    {
        messageboxissue("Debugger Detected");
    }



    if (isBreakpointSet())
    {
        messageboxissue("Check failed. Reopen");
    }



    BOOL lmaothreaddetected = ThreadHideFromDebugger();

    if (lmaothreaddetected)
    {
        messageboxissue("Debugging Detected");
    }

    int hasDebugRegisters = checkThreadContext();
    if (hasDebugRegisters)
    {
        messageboxissue("Debugging Detected");
    }

    if (SystemKernelDebuggerInformation())
    {
        messageboxissue("Debugger Detected. Restart computer");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));


    VMProtectEnd();
}
