#pragma once
#include <string>
using FString = std::string;

#ifdef TEXT
#undef TEXT
#endif

#define TEXT(s) FString(s)