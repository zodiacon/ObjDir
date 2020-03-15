#pragma once

extern "C" {
	namespace NT {
		typedef struct _UNICODE_STRING {
			USHORT Length;
			USHORT MaximumLength;
#ifdef MIDL_PASS
			[size_is(MaximumLength / 2), length_is((Length) / 2)] USHORT * Buffer;
#else // MIDL_PASS
			_Field_size_bytes_part_(MaximumLength, Length) PWCH   Buffer;
#endif // MIDL_PASS
		} UNICODE_STRING;
		typedef UNICODE_STRING *PUNICODE_STRING;
		typedef const UNICODE_STRING *PCUNICODE_STRING;

		typedef long NTSTATUS;

		typedef struct QueryDirectoryObjectBuffer_t {
			UNICODE_STRING DirectoryEntry;
			UNICODE_STRING DirectoryEntryType;
			char Buffer[1000];
		} QUERYDIRECTORYOBJECTBUFFER, *PQUERYDIRECTORYOBJECTBUFFER;

		typedef struct _OBJECT_ATTRIBUTES {
			ULONG Length;
			HANDLE RootDirectory;
			PUNICODE_STRING ObjectName;
			ULONG Attributes;
			PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
			PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
		} OBJECT_ATTRIBUTES;
		typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;
		typedef CONST OBJECT_ATTRIBUTES *PCOBJECT_ATTRIBUTES;

		typedef ULONG ACCESS_MASK;
		typedef ACCESS_MASK *PACCESS_MASK;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
	(p)->RootDirectory = r;                             \
	(p)->Attributes = a;                                \
	(p)->ObjectName = n;                                \
	(p)->SecurityDescriptor = s;                        \
	(p)->SecurityQualityOfService = NULL;               \
				}

#define DIRECTORY_QUERY                 (0x0001)
#define DIRECTORY_TRAVERSE              (0x0002)
#define DIRECTORY_CREATE_OBJECT         (0x0004)
#define DIRECTORY_CREATE_SUBDIRECTORY   (0x0008)

#define DIRECTORY_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xF)

#define EVENT_QUERY_STATE       0x0001
#define EVENT_MODIFY_STATE      0x0002  
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) 

		typedef struct _OBJECT_DIRECTORY_INFORMATION {
			UNICODE_STRING Name;
			UNICODE_STRING TypeName;
		} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

		typedef enum _OBJECT_INFORMATION_CLASS {
			ObjectBasicInformation,
			ObjectNameInformation,
			ObjectTypeInformation,
			ObjectAllInformation,
			ObjectDataInformation
		} OBJECT_INFORMATION_CLASS;

		NTSTATUS
			NTAPI
			NtOpenDirectoryObject(
			OUT PHANDLE hDirectory,
			IN ACCESS_MASK AccessMask,
			IN POBJECT_ATTRIBUTES ObjectAttributes
			);

		NTSTATUS
			NTAPI
			NtQueryDirectoryObject(
			IN HANDLE hDirectory,
			OUT POBJECT_DIRECTORY_INFORMATION DirectoryEntryBuffer,
			IN ULONG DirectoryEntryBufferSize,
			IN BOOLEAN  bOnlyFirstEntry,
			IN BOOLEAN bFirstEntry,
			IN PULONG  EntryIndex,
			OUT PULONG  BytesReturned
			);

		VOID
			NTAPI
			RtlInitUnicodeString(
			_Out_ PUNICODE_STRING DestinationString,
			_In_opt_z_ __drv_aliasesMem PCWSTR SourceString
			);

		NTSTATUS NTAPI NtOpenSymbolicLinkObject(
			_Out_  PHANDLE LinkHandle,
			_In_   ACCESS_MASK DesiredAccess,
			_In_   POBJECT_ATTRIBUTES ObjectAttributes
			);


		NTSTATUS NTAPI NtQuerySymbolicLinkObject(
			_In_       HANDLE LinkHandle,
			_Inout_    PUNICODE_STRING LinkTarget,
			_Out_opt_  PULONG ReturnedLength
			);

	}
}
