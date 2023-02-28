#include "Head.h"


VOID EnumDriverObject(PDRIVER_OBJECT DriverObject) {
	PUCHAR DriverObjectByte = (PUCHAR)DriverObject;
	POBJECT_HEADER_NAME_INFO PObjHeaderNameInfo = DriverObjectByte - _OBJECT_HEADER_Body_Offset - sizeof(OBJECT_HEADER_NAME_INFO);
	POBJECT_DIRECTORY PDirectory = PObjHeaderNameInfo->Directory;//该驱动所在目录
	PDRIVER_OBJECT TargetDrvObj = NULL;
	POBJECT_DIRECTORY_ENTRY PSubDirectoryEntry = NULL;
	POBJECT_DIRECTORY_ENTRY PDirectoryEntry = NULL;
	//DbgBreakPoint();
	for (int i = 0; i < 37; i++) {
		PDirectoryEntry = PDirectory->HashBuckets[i];
		if (PDirectoryEntry == NULL) {
			continue;
		}
		PSubDirectoryEntry = PDirectoryEntry;
		while (PSubDirectoryEntry != NULL) {
			TargetDrvObj = PSubDirectoryEntry->Object;
			DbgPrint("DrvObj : %p  DrvName : %wZ  \n", TargetDrvObj, &TargetDrvObj->DriverName);
			PSubDirectoryEntry = PSubDirectoryEntry->ChainLink;
		}
	}

}