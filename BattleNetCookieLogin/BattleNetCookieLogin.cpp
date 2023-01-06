#include <windows.h>
#include <shlwapi.h>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <iomanip>

#pragma comment(lib,"Crypt32.lib")

// got these bytes when I hooked CryptPprotectData from crypt32.dll
BYTE entropyBytes[] = { 0xc8, 0x76, 0xf4, 0xae, 0x4c, 0x95, 0x2e, 0xfe, 0xf2, 0xfa, 0xf, 0x54, 0x19, 0xc0, 0x9c, 0x43 };


// thanks chatgpt
int OverwriteValues(HKEY hKey, const char* subFolderPath, BYTE* bytes, int len)
{
    LONG result = RegOpenKeyEx(
        hKey,  // handle to open key
        subFolderPath,    // subkey to open
        0,                  // reserved
        KEY_ALL_ACCESS,     // desired security access
        &hKey               // handle to open key
    );

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error opening registry key!" << std::endl;
        return 0;
    }

    DWORD index = 0;
    char name[4096];
    DWORD nameSize = 4096;
    DWORD valueType;
    BYTE valueData[4096];
    DWORD valueDataSize = 4096;

    // Enumerate all the values in the key
    while (RegEnumValue(hKey, index, name, &nameSize, NULL, &valueType, valueData, &valueDataSize) == ERROR_SUCCESS) {
        std::cout << name << std::endl;
        // Check if the value is of type REG_BINARY
        if (valueType == REG_BINARY) {
            // Overwrite the value with the new data
            result = RegSetValueEx(
                hKey,               // handle to key
                name,               // value name
                0,                  // reserved
                valueType,          // type of value
                bytes,              // value data
                len                 // size of value data
            );
            if (result != ERROR_SUCCESS) {
                std::cerr << "Error setting registry value!" << std::endl;
            }
        }

        // Reset the name size and value data size for the next iteration
        nameSize = 4096;
        valueDataSize = 4096;
        index++;
    }

    RegCloseKey(hKey);
    return index;
}

int main() {
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
    
    if (OverwriteValues(HKEY_CURRENT_USER, "SOFTWARE\\Blizzard Entertainment\\Battle.net\\UnifiedAuth", out.pbData, out.cbData) > 0) 
    {
        std::cout << "Success!" << std::endl;
    }
    else 
    {
        std::cout << "Couldn't find any reg values, are you sure that you are logged in with \"Keep logged in\" option?" << std::endl;
    }

    
    Sleep(1000);
    return 0;
}
