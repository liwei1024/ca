#include "stdafx.h"
#include "memory.h"

std::vector<BYTE> memory::readBytes(DWORD_PTR address, size_t length)
{
	std::vector<BYTE> bytes;
	DWORD lpflOldProtect;
	VirtualProtect(LPVOID(address), length, PAGE_EXECUTE_READWRITE, &lpflOldProtect);
	for (size_t i = 0; i < bytes.size(); i++)
	{
		bytes[i] = read<BYTE>(address + i);
	}
	VirtualProtect(LPVOID(address), length, lpflOldProtect, &lpflOldProtect);
	return bytes;
}
void memory::writeBytes(DWORD_PTR address, std::vector<BYTE> bytes)
{
	DWORD lpflOldProtect;
	VirtualProtect((LPVOID)address, bytes.size(), PAGE_EXECUTE_READWRITE, &lpflOldProtect);
	for (size_t i = 0; i < bytes.size(); i++)
	{
		write<BYTE>(address + i, bytes[i]);
	}
	VirtualProtect((LPVOID)address, bytes.size(), lpflOldProtect, &lpflOldProtect);
}
std::wstring  memory::readWString(DWORD_PTR address, size_t length)
{
	wchar_t *buffer = new wchar_t[length];
	if (IsBadReadPtr((LPVOID)address, length) == 0) {
		memcpy(buffer, (LPVOID)address, length);
	}
	std::wstring str(buffer, length);
	delete[] buffer;
	return str;
}
std::string memory::readString(DWORD_PTR address, size_t length)
{
	char *buffer = new char[length];
	if (IsBadReadPtr((LPVOID)address, length) == 0) {
		memcpy(buffer, (LPVOID)address, length);
	}
	std::string str(buffer, length);
	delete[] buffer;
	return str;
}
