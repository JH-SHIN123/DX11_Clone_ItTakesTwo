#pragma once

#include <process.h>

static const unsigned int g_iWinCX = 1280;
static const unsigned int g_iWinCY = 720;

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

#define MATH_PI 3.141592

#include "Client_Enum.h"
#include "Client_Animation_Index_Define.h"
#include "DataStorage.h"
#include "GameInstance.h"

namespace Client {}
using namespace Client;