
#include <Rcpp.h>

#undef Realloc
#undef Free

#include <windows.h>

// credits:
// http://stackoverflow.com/questions/11588765/using-rcpp-with-windows-specific-includes
// http://stackoverflow.com/questions/1888863/how-to-get-main-window-handle-from-process-id
// http://stackoverflow.com/questions/185254/how-can-a-win32-process-get-the-pid-of-its-parent

using namespace Rcpp;

FLASHWINFO info;

struct handle_data {
    unsigned long process_id;
    HWND best_handle;
};

BOOL is_main_window(HWND handle)
{   
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
    handle_data& data = *(handle_data*)lParam;
    unsigned long process_id = 0;
    
    GetWindowThreadProcessId(handle, &process_id);

    if (data.process_id != process_id || !is_main_window(handle)) {
        return TRUE;
    }

    data.best_handle = handle;

    return FALSE;   
}


HWND find_main_window(unsigned long process_id)
{
    handle_data data;
    data.process_id = process_id;
    data.best_handle = 0;

    EnumWindows(enum_windows_callback, (LPARAM)&data);

    return data.best_handle;
}

ULONG_PTR get_parent_process_id()
{
    ULONG_PTR pbi[6];
    ULONG ulSize = 0;

    LONG (WINAPI *NtQueryInformationProcess)(HANDLE ProcessHandle, ULONG ProcessInformationClass,
        PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength); 

    *(FARPROC *)&NtQueryInformationProcess = GetProcAddress(LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");

    if (NtQueryInformationProcess) {
        if (NtQueryInformationProcess(GetCurrentProcess(), 0, &pbi, sizeof(pbi), &ulSize) >= 0 && ulSize == sizeof(pbi)) {
            return pbi[5];
        }
    }

    return (ULONG_PTR)-1;
}

//' @title Make Rstudio window blink
//' @param count number of times to blink
//' @examples
//' winflash(10)
// [[Rcpp::export]]
void winflash(int count = 1)
{
    HWND w = find_main_window(get_parent_process_id());

    info.cbSize = sizeof(info);
    info.hwnd = w;
    info.dwFlags = FLASHW_ALL;
    info.dwTimeout = 0;
    info.uCount = count;
  
    FlashWindowEx(&info);
}
