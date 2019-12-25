#include "stdafx.h"
#include "ObjectManager.h"

#pragma comment(lib, "ntdll.lib")

using namespace NT;
using namespace std;

ObjectManager::ObjectManager() : _size(1 << 17) {
	_bytes = std::make_unique<BYTE[]>(_size);
	_buffer = reinterpret_cast<NT::POBJECT_DIRECTORY_INFORMATION>(_bytes.get());
}

vector<pair<CString, CString>> ObjectManager::GetObjects(const CString& root, NTSTATUS& status) {
	HANDLE hDirectory;
	OBJECT_ATTRIBUTES attr;
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, root);
	InitializeObjectAttributes(&attr, &name, 0, nullptr, nullptr);
	vector<pair<CString, CString>> list;
	status = NtOpenDirectoryObject(&hDirectory, DIRECTORY_QUERY | DIRECTORY_TRAVERSE, &attr);
	if(status < 0)
		return list;

	ULONG index = 0, bytes;
	BOOLEAN firstEntry = TRUE;
	int start = 0;
	do {
		status = NtQueryDirectoryObject(hDirectory, _buffer, _size, FALSE, firstEntry, &index, &bytes);
		if(status < 0)
			break;
		for(ULONG i = 0; i < index - start; i++)
			list.emplace_back(make_pair(
				CString(_buffer[i].Name.Buffer, _buffer[i].Name.Length / sizeof(WCHAR)), 
				CString(_buffer[i].TypeName.Buffer, _buffer[i].TypeName.Length / sizeof(WCHAR))));
		if(status == 0)
			break;
		// more entries (STATUS_NEED_MORE_ENTRIES)
		start = index;
		if(firstEntry)
			firstEntry = FALSE;
	} while(true);
	CloseHandle(hDirectory);
	return list;
}

CString ObjectManager::GetSymbolicLinkFromName(const CString& directory, const CString& name) const {
	HANDLE hLink;
	OBJECT_ATTRIBUTES attr, attrDir;
	UNICODE_STRING str, strDir;
	HANDLE hRoot;
	RtlInitUnicodeString(&strDir, directory);
	InitializeObjectAttributes(&attrDir, &strDir, 0, nullptr, nullptr);
	NtOpenDirectoryObject(&hRoot, GENERIC_READ, &attrDir);
	ATLASSERT(hRoot != nullptr);
	RtlInitUnicodeString(&str, name);
	InitializeObjectAttributes(&attr, &str, 0, hRoot, nullptr);
	NtOpenSymbolicLinkObject(&hLink, GENERIC_READ, &attr);
	WCHAR buffer[512] = { 0 };
	UNICODE_STRING target;
	RtlInitUnicodeString(&target, buffer);
	target.MaximumLength = sizeof(buffer);
	ULONG len;
	NtQuerySymbolicLinkObject(hLink, &target, &len);
	CloseHandle(hLink);
	CloseHandle(hRoot);
	return CString(target.Buffer, target.Length / sizeof(WCHAR));
}

HANDLE ObjectManager::OpenObject(const CString& name, ULONG access, const CString& type, NTSTATUS* pStatus) {
	HANDLE hObject = nullptr;
	OBJECT_ATTRIBUTES attr;
	UNICODE_STRING uname;
	RtlInitUnicodeString(&uname, name);
	InitializeObjectAttributes(&attr, &uname, 0, nullptr, nullptr);
	NTSTATUS status = E_FAIL;
	if(type == L"Event")
		status = NtOpenEvent(&hObject, access, &attr);
	else if(type == L"Mutant")
		status = NtOpenMutant(&hObject, access, &attr);
	else if(type == L"Section")
		status = NtOpenSection(&hObject, access, &attr);
	else if(type == L"Semaphore")
		status = NtOpenSemaphore(&hObject, access, &attr);
	else if(type == "EventPair")
		status = NtOpenEventPair(&hObject, access, &attr);
	else if(type == L"IoCompletion")
		status = NtOpenIoCompletion(&hObject, access, &attr);
	else if(type == L"SymbolicLink")
		status = NtOpenSymbolicLinkObject(&hObject, access, &attr);
	else if(type == "Key")
		status = NtOpenKey(&hObject, access, &attr);
	if(pStatus)
		*pStatus = status;
	return hObject;
}

