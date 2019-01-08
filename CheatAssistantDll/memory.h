#pragma once
namespace memory {
	template<typename T>T read(DWORD_PTR address)
	{
		T value = T();
		__try
		{
			if (IsBadReadPtr((LPVOID)address, sizeof(value)) == 0) {
				value = *(T *)address;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			// 此处应该打印错误日志
		}
		return value;
	}
	template<typename T>bool write(DWORD_PTR address, T value)
	{
		__try
		{
			if (IsBadWritePtr((LPVOID)address, sizeof(value)) == 0) {
				*(T *)address = value;
				return true;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			// 此处应该打印错误日志
		}
		return false;
	}
	template <typename T>T readOffset(DWORD_PTR address, std::vector<int> offset)
	{
		DWORD offsetAddress = read<DWORD>(address);
		T value;
		for (size_t i = 0; i < offset.size(); i++)
		{
			if (offsetAddress > 0)
			{
				if ((i + 1) < offset.size())
				{
					offsetAddress = read<DWORD>(offsetAddress + offset[i]);
				}
				else {
					value = read<T>(offsetAddress + offset[i]);
				}
			}
		}
		return value;
	}
	template <typename T>bool write_offset(DWORD_PTR address, std::vector<int> offset, T value)
	{
		DWORD offsetAddress = read<DWORD>(address);
		bool result = false;

		for (size_t i = 0; i < offset.size(); i++)
		{
			if (offsetAddress)
			{
				if ((i + 1) < offset.size())
				{
					offsetAddress = read<DWORD>(offsetAddress + offset[i]);
				}
				else {
					result = write<T>(offsetAddress + offset[i], value);
				}
			}
		}
		return result;
	}
	
	std::vector<BYTE> readBytes(DWORD_PTR address, size_t length);

	void writeBytes(DWORD_PTR address, std::vector<BYTE> bytes);

	std::wstring readWString(DWORD_PTR address, size_t length);

	std::string readString(DWORD_PTR address, size_t length);

}
