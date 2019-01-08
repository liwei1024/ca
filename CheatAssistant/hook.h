#pragma once
class Hook
{
public:
	std::vector<byte> g_oldData;
	int g_targetAddress;
	Hook();
	~Hook();
	void jmpHook(int targetAddress, int jmpAddress, size_t hookLength);
	void unHook();
	void jmpHook2(int targetAddress, int jmpAddress, size_t hookLength, std::vector<byte> hookCode);
};

