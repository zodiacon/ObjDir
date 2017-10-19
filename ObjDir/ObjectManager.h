#pragma once

#include "NtDll.h"

class ObjectManager {
public:
	ObjectManager();
	~ObjectManager();

	std::vector<std::pair<CString, CString>> GetObjects(const CString& root, NTSTATUS&);
	CString GetSymbolicLinkFromName(const CString& directory, const CString& name) const;

	HANDLE OpenObject(const CString& name, ULONG access, const CString& type, NT::NTSTATUS* pStatus = nullptr);

private:
	unsigned _size;
	NT::POBJECT_DIRECTORY_INFORMATION _buffer;
};

