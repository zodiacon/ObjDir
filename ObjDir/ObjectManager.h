#pragma once

#include "NtDll.h"

class ObjectManager {
public:
	ObjectManager();

	std::vector<std::pair<CString, CString>> GetObjects(const CString& root, NTSTATUS&);
	CString GetSymbolicLinkFromName(const CString& directory, const CString& name) const;

	HANDLE OpenObject(const CString& name, ULONG access, const CString& type, NT::NTSTATUS* pStatus = nullptr);

private:
	unsigned m_Size;
	std::unique_ptr<BYTE[]> m_Bytes;
	NT::POBJECT_DIRECTORY_INFORMATION m_Buffer;
};

