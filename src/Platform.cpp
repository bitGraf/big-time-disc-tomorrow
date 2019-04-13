#include "platform.h"

void change_working_directory(char* dir) {
    printf("Setting working directory to '%s'.\n", dir);
    if (!SetCurrentDirectory(dir)) {
        printf("SetCurrentDirectory failed (%d)\n", GetLastError());
    }
}

void print_current_working_directory() {
    DWORD dwRet;
    TCHAR Buffer[MAX_PATH];

    dwRet = GetCurrentDirectory(MAX_PATH, Buffer);
    if( dwRet == 0 ) {
        printf("GetCurrentDirectory failed (%d)\n", GetLastError());
        return;
    }
    if(dwRet > MAX_PATH) {
        printf("Buffer too small; need %d characters\n", dwRet);
        return;
    }
    printf("Current working directotry is '%s'\n", Buffer);
}

void get_run_location(char* loc) {
    int length = GetModuleFileName(NULL, loc, MAX_PATH);

    int i = 0;
    for (i = length-1; loc[i] != '\\'; i--) {
    }
    loc[i]   = '\\';
    loc[i+1] = '\0';

    //char* CURRENT_WORKING_DIRECTORY = (char*)malloc((i+1) * sizeof(char));
    //CURRENT_WORKING_DIRECTORY[i+1] = 0;

    printf("Executable location is '%s'\n", loc);
}