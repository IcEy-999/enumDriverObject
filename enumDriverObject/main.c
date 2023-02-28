#include"head.h"


NTSTATUS Exit(DRIVER_OBJECT* DriverObject) {
	DbgPrint("DriverUnload\n");
	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(DRIVER_OBJECT* DriverObject, UNICODE_STRING* STR) {
	DriverObject->DriverUnload = Exit;
	NTSTATUS status = STATUS_SUCCESS;

	EnumDriverObject(DriverObject);
	return status;
}