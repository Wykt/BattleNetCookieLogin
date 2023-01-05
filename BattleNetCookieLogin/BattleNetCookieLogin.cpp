#include <windows.h>
#include <shlwapi.h>
#include <iostream>
#include <cstdio>

#pragma comment(lib,"Crypt32.lib")

// got these bytes when I hooked CryptPprotectData from crypt32.dll
BYTE entropyBytes[] = { 0xc8, 0x76, 0xf4, 0xae, 0x4c, 0x95, 0x2e, 0xfe, 0xf2, 0xfa, 0xf, 0x54, 0x19, 0xc0, 0x9c, 0x43 };

char* GetExeFolderPath() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    PathRemoveFileSpecA(path);
    return path;
}

int main() {
    // Get Battle.net.config file path and write it to buff
    char buff[100];
    snprintf(buff, sizeof(buff), "c:/users/%s/appdata/roaming/battle.net/Battle.net.config", getenv("username"));

    // Get our exe folder path and append it with /Battle.net.config
    std::string exePath(GetExeFolderPath());
    exePath += "\\Battle.net.config";

    // Overwrite with our own config
    remove(buff);
    CopyFile(exePath.c_str(), buff, FALSE);

    // cookie input
    std::string cookie;

    std::cout << "Cookie: ";
    std::cin >> cookie;

    BYTE* cookieBytes = (BYTE*)cookie.c_str();

    DATA_BLOB in;
    in.pbData = cookieBytes;
    in.cbData = cookie.size();

    // encryption
    DATA_BLOB entropy;
    entropy.pbData = entropyBytes;
    entropy.cbData = 16;

    DATA_BLOB out;

    CryptProtectData(&in, NULL, &entropy, NULL, NULL, 1, &out);

    HKEY hKey;
    LPCTSTR lpSubKey = TEXT("SOFTWARE\\Blizzard Entertainment\\Battle.net\\UnifiedAuth");

    // lpValueName depends on your Battle.net.config (emails, and probably clientId)
    // I'm not 100% sure but I think it also depends on Battle.net installation folder
    // That's why I overwrite Battle.net.config with my own config and hardcode this string (i'm too lazy to reverse)
    LPCTSTR lpValueName = TEXT("9DB491B5");
    LONG lResult;

    lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_WRITE, &hKey);
    if (lResult != ERROR_SUCCESS)
    {
        std::cerr << "Error opening key: " << lResult << std::endl;
        return 1;
    }

    lResult = RegSetValueEx(hKey, lpValueName, 0, REG_BINARY, out.pbData, out.cbData);
    if (lResult != ERROR_SUCCESS)
    {
        std::cerr << "Error setting value: " << lResult << std::endl;
        return 1;
    }

    RegCloseKey(hKey);
    return 0;
}
