#pragma once

#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>
#include <Windows.h>
#include <CommCtrl.h>

#include <cassert>
#include <cmath>
#include <string>

template <typename T>
void Clear(T* x)
{
	memset(x, 0, sizeof(T));
}