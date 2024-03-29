// ObjDir.cpp : Defines the entry point for the console application.
//

#include "pch.h"

#include "ObjectManager.h"

using namespace std;

#define STATUS_OBJECT_TYPE_MISMATCH      ((NTSTATUS)0xC0000024L)
#define STATUS_OBJECT_NAME_NOT_FOUND     ((NTSTATUS)0xC0000034L)
#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L)

int wmain(int argc, const wchar_t* argv[]) {
    CString root(L"\\");
    if (argc > 1) {
        root = argv[1];
    }

    ObjectManager mgr;
    NTSTATUS status;
    auto folders = mgr.GetObjects(root, status);
    if (status < 0) {
        switch(status) {
        case STATUS_OBJECT_TYPE_MISMATCH: 
            cout << "Error: not a directory" << endl;
            break;

        case STATUS_ACCESS_DENIED:
            cout << "Error: access denied (try running elevated)" << endl;
            break;

        case STATUS_OBJECT_NAME_NOT_FOUND:
            cout << "Error: directory name not found" << endl;
            break;

        default:
            cout << "Error: " << hex << status << endl;
            break;
        }
        return 1;
    }

    sort(folders.begin(), folders.end(), [](const auto& pair1, const auto& pair2) {
        return pair1.first < pair2.first;
    });

    wcout << L"Directory: " << (PCWSTR)root << endl;

    for (const auto& [name, type] : folders) {
        wcout << setiosflags(ios_base::left) << setw(50) << setiosflags(ios_base::left) << 
            (PCWSTR)name << L" " << setw(30) << (PCWSTR)type;
        if (type == "SymbolicLink") {
            auto link = mgr.GetSymbolicLinkFromName(root, name);
            wcout << L" " << (PCWSTR)link;
        }
        wcout << endl;
    }

    return 0;
}

