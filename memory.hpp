#pragma once
#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <windows.h>
#include <vector>
#include <Psapi.h>
template<class ClassName>
class Memory
{
public:
	ClassName instance;
	DWORD getProcessIdByProcessName(const wchar_t * processName)
	{
		return instance.getProcessIdByProcessName(processName);
	}
	BOOL modifyCurrentProcessPath(ULONG ProcessId) 
	{
		return instance.modifyCurrentProcessPath(ProcessId);
	}
	BOOL protectCurrentProcess()
	{
		return instance.protectCurrentProcess();
	}
	BOOL protectCurrentProcessFile()
	{
		return instance.protectCurrentProcessFile();
	}
	BOOL unProtectCurrentProcessFile()
	{
		return instance.unProtectCurrentProcessFile();
	}
	BOOL getModuleInfoByModuleName(LPMODULEINFO ModuleInfo, const wchar_t *moduleName)
	{
		return instance.getModuleInfoByModuleName(ModuleInfo, moduleName);
	}
	HMODULE getModuleHandleByModuleName(const wchar_t * moduleName)
	{
		return instance.getModuleHandleByModuleName(moduleName);
	}
	HANDLE getProcessHandle() {
		return instance.getProcessHandle();
	}
	DWORD getProcessId() {
		return (DWORD)instance.getProcessId();
	}
	BOOL readVirtualMemory(ULONG Address, PVOID Response, SIZE_T Size)
	{
		return instance.readVirtualMemory(Address, Response, Size);
	}
	BOOL writeVirtualMemory(ULONG Address, PVOID Value, SIZE_T Size)
	{
		return instance.writeVirtualMemory(Address, Value, Size);
	}
	void close() {
		instance.closeHandle();
	}
	template<typename T>
	T read(DWORD_PTR address) {
		T value = T();
		readVirtualMemory((ULONG)address, &value, sizeof(T));
		return value;
	}
	template<typename T>
	BOOL write(DWORD_PTR dwBaseAddress, T Value)
	{
		return writeVirtualMemory((ULONG)dwBaseAddress, &Value, sizeof(T));
	}
	template <typename T>
	T readOffset(DWORD_PTR base_address, std::vector<int> offset)
	{
		DWORD ofset_address = read<DWORD>(base_address);
		T value;
		for (size_t i = 0; i < offset.size(); i++)
		{
			if (ofset_address > 0)
			{
				if ((i + 1) < offset.size())
				{
					ofset_address = read<DWORD>(ofset_address + offset[i]);
				}
				else {
					value = read<T>(ofset_address + offset[i]);
				}
			}
		}
		return value;
	}
	template <typename T>
	bool writeOffset(DWORD_PTR base_address, std::vector<int> offset, T value)
	{
		DWORD ofset_address = read<DWORD>(base_address);
		bool result = false;

		for (size_t i = 0; i < offset.size(); i++)
		{
			if (ofset_address)
			{
				if ((i + 1) < offset.size())
				{
					ofset_address = read<DWORD>(ofset_address + offset[i]);
				}
				else {
					result = write<T>(ofset_address + offset[i], value);
				}
			}
		}

		return result;
	}
	std::wstring readWString(DWORD_PTR dwBaseAddress, SIZE_T Size)
	{
		wchar_t *buffer = new wchar_t[Size];
		instance.readVirtualMemory((ULONG)dwBaseAddress, buffer, Size);
		std::wstring wstr(buffer, Size);
		delete[]buffer;
		return wstr;
	}
	std::string readString(DWORD_PTR dwBaseAddress, SIZE_T Size)
	{
		char *buffer = new char[Size];
		instance.readVirtualMemory((ULONG)dwBaseAddress, buffer, Size);
		std::string str(buffer, Size);
		delete[]buffer;
		return str;
	}
	std::vector<byte> readBytes(DWORD_PTR dwBaseAddress, SIZE_T Size)
	{
		std::vector<byte> bytes;
		byte * buffer = new byte[Size];
		instance.readVirtualMemory((ULONG)dwBaseAddress, buffer, Size);
		for (size_t i = 0; i < Size; i++)
		{
			bytes.insert(bytes.end(), buffer[i]);
		}
		delete[]buffer;
		return bytes;
	}
	BOOL writeBytes(DWORD_PTR dwBaseAddress, std::vector<byte> Bytes)
	{
		byte * buffer = new byte[Bytes.size()];
		BOOL result = TRUE;
		for (size_t i = 0; i < Bytes.size(); i++)
		{
			buffer[i] = Bytes[i];
		}
		result = instance.writeVirtualMemory((ULONG)dwBaseAddress, buffer, Bytes.size());
		delete[]buffer;
		return result;
	}
};
#endif // !__MEMORY_H__





