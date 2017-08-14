#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <wchar.h>
#include <stdio.h>

const wchar_t* REG_SYS = L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"; // System Variables (HKEY_LOCAL_MACHINE)
const wchar_t* PATH = L"Path";

int main(int argc, char** argv)
{
    int returnCode = -1;

    if (argc < 2)
    {
        printf("Usage: AddToSystemPath <Path-to-your-directory>\n");
        return returnCode;
    }
    char const* dir = argv[1];
    printf("Directory appended to PATH: %s\n", dir);

    HKEY hkeyResult;
    LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, REG_SYS, 0, KEY_ALL_ACCESS, &hkeyResult);
    if (result == ERROR_SUCCESS)
    {
        DWORD pathSize = 0;
        DWORD regType = REG_EXPAND_SZ;
        result = RegQueryValueExW(hkeyResult, PATH, 0, &regType, NULL, &pathSize);
        if (result == ERROR_SUCCESS)
        {
            int dirLen = strlen(dir);
            int wdirSize = sizeof(wchar_t)*(dirLen + 1);
            wchar_t* wdir = (wchar_t*)malloc(wdirSize);
            memset(wdir, 0, wdirSize);
            mbstowcs(wdir, dir, dirLen);

            int wdirLen = wcslen(wdir);
            int semicolonSize = sizeof(wchar_t);
            wchar_t* pathStr = (wchar_t*)malloc(pathSize + semicolonSize + wdirSize);
            if (pathStr)
            {
                result = RegQueryValueExW(hkeyResult, PATH, 0, &regType, (BYTE*)pathStr, &pathSize);
                if (result == ERROR_SUCCESS)
                {
                    int pathLen = wcslen(pathStr);
                    int r = swprintf(pathStr + pathLen, wdirLen + 2, L";%ls", wdir);
                    wprintf(L"%ls\n", pathStr);
                    if (r > 0)
                    {
                        result = RegSetValueEx(hkeyResult, PATH, 0, REG_EXPAND_SZ, (BYTE const*)pathStr, pathSize + wdirSize);
                        if (result == ERROR_SUCCESS)
                        {
                            printf("ok\n");
                            returnCode = 0;
                        }
                        else
                        {
                            printf("Error: Could not write PATH registry key");
                        }
                    }
                    else
                    {
                        printf("Error: Could not get value of PATH registry key (#2)");
                    }
                }
                else
                {
                    printf("Could not retrieve path\n");
                }
            }
            else
            {
                printf("Error: Could not allocate mem for PATH string");
            }
        }
        else
        {
            printf("Error: Could not get value of PATH registry key");
        }
    }
    else
    {
        printf("Error: Could not open registry key");
    }

    return returnCode;
}