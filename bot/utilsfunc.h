#pragma once

#include <string>
#include <Windows.h>
#include "corefunc.h"
#include "corefunc.h"
using namespace std;

namespace patch
{
	template < typename T > std::string to_string(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}

int GetApp()
{
	return 0xA2Cu;
}

unsigned int HashString(const char *str, int len)
{
	if (!str) return 0;

	unsigned char *n = (unsigned char *)str;
	unsigned int acc = 0x55555555;

	if (len == 0)
	{
		while (*n)
			acc = (acc >> 27) + (acc << 5) + *n++;
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			acc = (acc >> 27) + (acc << 5) + *n++;
		}
	}
	return acc;
}

int GetDeviceHash(string deviceId)
{
	string v8 = deviceId;
	int v3;
	v8 += patch::to_string(GetApp() + 2280);
	v8 += "RT";
	v3 = HashString(v8.c_str(), 0);
	return v3;
}


int GetDeviceSecondaryHash(string macAddr)
{
	int v1;
	string v3 = "";
	for (std::string::size_type i = 0; i < macAddr.length(); ++i)
		v3 += tolower(macAddr[i]);
	v3 += "RT";
	v1 = HashString(v3.c_str(), 0);
	return v1;
}


char* CreateGameUpdatePacketWithExtraDataAtEnd(int a1)
{
	char* result = new char[a1 + 56];

	*(DWORD *)result = 0;
	*(DWORD *)(result + 4) = 0;
	*(DWORD *)(result + 8) = 0;
	*(DWORD *)(result + 12) = 8;
	*(DWORD *)(result + 16) = 0;
	*(DWORD *)(result + 20) = 0;
	*(DWORD *)(result + 24) = 0;
	*(DWORD *)(result + 28) = 0;
	*(DWORD *)(result + 32) = 0;
	*(DWORD *)(result + 36) = 0;
	*(DWORD *)(result + 40) = 0;
	*(DWORD *)(result + 44) = 0;
	*(DWORD *)(result + 48) = 0;
	*(DWORD *)(result + 52) = a1;
	return result;
}



std::string colorstr(string str)
{
	string chrs = "0123456789bwpo^$#@!qertas";
	string s;
	for (int i = 0; i < str.length(); i++)
	{
		s += "`";
		char* x;
		x = (char*)malloc(2);
		x[0] = chrs[rand() % chrs.length()];
		x[1] = 0;
		string y = x;
		s += y;
		free(x);
		s += str[i];
	}
	return s;
}

std::string colorstr2(string str)
{
	string chrs = "0123456789bwpo^$#@!qertas";
	char* x;
	x = (char*)malloc(2);
	x[0] = chrs[rand() % chrs.length()];
	x[1] = 0;
	string y = x;
	free(x);
	return "`" + y + str;
}