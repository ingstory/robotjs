#include "power_management.h"

#if defined(IS_WINDOWS)
#include <windows.h>
#include <Powrprof.h>  // For SetSuspendState()

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

int deleteProgram(const char* programPath)
{
    if (DeleteFileA(programPath)) {
        return 0; // Success
    } else {
        return GetLastError(); // Return the error code
    }
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

void freeInstalledPrograms(char** programs, int count)
{
    for (int i = 0; i < count; i++) {
        free(programs[i]);
    }
    free(programs);
}

#else

void setSystemPowerState(int state) {}
int getSystemPowerState(void) { return 0; }
int deleteProgram(const char* programPath) { return 0; }
char** getInstalledPrograms(int* count) { *count = 0; return 0; }
void freeInstalledPrograms(char** programs, int count) {}

#endif