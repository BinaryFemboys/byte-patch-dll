#pragma once
#include "includes.h"

class bytepatchdll_t
{
	std::uintptr_t base;
	std::uintptr_t size;
	std::uintptr_t resource;
public:
	bytepatchdll_t(HMODULE mod);

	void patches();
};