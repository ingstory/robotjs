#include "power_management.h"
#include <windows.h>

static int currentPowerState = 0;

void setSystemPowerState(int state)
{
    if (state == POWER_STATE_SUSPEND) {
        /* Suspend the system */
        SetSuspendState(FALSE, FALSE, FALSE);
    } else if (state == POWER_STATE_HIBERNATE) {
        /* Hibernate the system */
        SetSuspendState(TRUE, FALSE, FALSE);
    } else if (state == POWER_STATE_SHUTDOWN) {
        /* Shut down the system */
        ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
    } else if (state == POWER_STATE_FORCE_SHUTDOWN) {
        /* Force shut down the system */
        ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE | EWX_FORCEIFHUNG, 0);
    } else {
        /* Invalid state */
        return;
    }

    currentPowerState = state;
}

int getSystemPowerState(void)
{
    return currentPowerState;
}

설치된 프로그램 리스트를 가져오는 기능을 추가해보겠습니다. 이를 위해 power_management.h에 새로운 함수를 선언하고, power_management.c에서 해당 기능을 구현하겠습니다.

power_management.h:

c


Copy code
#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#define POWER_STATE_SUSPEND 0
#define POWER_STATE_HIBERNATE 1
#define POWER_STATE_SHUTDOWN 2
#define POWER_STATE_FORCE_SHUTDOWN 3

#ifdef __cplusplus
extern "C" {
#endif

void setSystemPowerState(int state);
int getSystemPowerState(void);
int deleteProgram(const char* programPath);
char** getInstalledPrograms(int* count);
void freeInstalledPrograms(char** programs, int count);

#ifdef __cplusplus
}
#endif

#endif /* POWER_MANAGEMENT_H */
power_management.c:

c


Copy code
#include "power_management.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static int currentPowerState = 0;

void setSystemPowerState(int state)
{
    /* ... */
}

int getSystemPowerState(void)
{
    /* ... */
}

int deleteProgram(const char* programPath)
{
    /* ... */
}

char** getInstalledPrograms(int* count)
{
    HKEY hKey;
    LONG lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hKey);
    if (lResult != ERROR_SUCCESS) {
        *count = 0;
        return NULL;
    }

    DWORD subKeyCount;
    DWORD maxSubKeyLength;
    lResult = RegQueryInfoKeyA(hKey, NULL, NULL, NULL, &subKeyCount, &maxSubKeyLength, NULL, NULL, NULL, NULL, NULL, NULL);
    if (lResult != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        *count = 0;
        return NULL;
    }

    char** programs = (char**)malloc(subKeyCount * sizeof(char*));
    *count = 0;

    for (DWORD i = 0; i < subKeyCount; i++) {
        DWORD subKeySize = maxSubKeyLength + 1;
        char* subKey = (char*)malloc(subKeySize * sizeof(char));
        lResult = RegEnumKeyExA(hKey, i, subKey, &subKeySize, NULL, NULL, NULL, NULL);
        if (lResult == ERROR_SUCCESS) {
            HKEY hSubKey;
            lResult = RegOpenKeyExA(hKey, subKey, 0, KEY_READ, &hSubKey);
            if (lResult == ERROR_SUCCESS) {
                DWORD valueSize;
                lResult = RegQueryValueExA(hSubKey, "DisplayName", NULL, NULL, NULL, &valueSize);
                if (lResult == ERROR_SUCCESS) {
                    char* displayName = (char*)malloc(valueSize * sizeof(char));
                    lResult = RegQueryValueExA(hSubKey, "DisplayName", NULL, NULL, (LPBYTE)displayName, &valueSize);
                    if (lResult == ERROR_SUCCESS) {
                        programs[*count] = displayName;
                        (*count)++;
                    } else {
                        free(displayName);
                    }
                }
                RegCloseKey(hSubKey);
            }
        }
        free(subKey);
    }

    RegCloseKey(hKey);
    return programs;
}

int deleteProgram(const char* programPath)
{
    if (DeleteFileA(programPath)) {
        return 0; // Success
    } else {
        return GetLastError(); // Return the error code
    }
}

void freeInstalledPrograms(char** programs, int count)
{
    for (int i = 0; i < count; i++) {
        free(programs[i]);
    }
    free(programs);
}