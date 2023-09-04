#include "pch.h"
#include "ObjectManager.h"

#pragma comment(lib, "ntdll.lib")

using namespace NT;
using namespace std;

ObjectManager::ObjectManager() : m_Size(1 << 17) {
	m_Bytes = std::make_unique<BYTE[]>(m_Size);
	m_Buffer = reinterpret_cast<NT::POBJECT_DIRECTORY_INFORMATION>(m_Bytes.get());
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
		status = NtQueryDirectoryObject(hDirectory, m_Buffer, m_Size, FALSE, firstEntry, &index, &bytes);
		if(status < 0)
			break;
		for(ULONG i = 0; i < index - start; i++)
			list.emplace_back(make_pair(
				CString(m_Buffer[i].Name.Buffer, m_Buffer[i].Name.Length / sizeof(WCHAR)), 
				CString(m_Buffer[i].TypeName.Buffer, m_Buffer[i].TypeName.Length / sizeof(WCHAR))));
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

