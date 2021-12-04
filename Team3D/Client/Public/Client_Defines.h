#pragma once

#include <process.h>

static const unsigned int g_iWinCX = 1280;
static const unsigned int g_iWinCY = 720;

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bWndActivate;

#define MATH_PI 3.141592

/* Engine */
#include "GameInstance.h"
/* Client */
#include "Client_Animation_Index_Define.h"
#include "Client_Enum.h"
#include "Client_Struct.h"
#include "Client_MathHelper.h"

#include "GameOption.h"
#include "DataStorage.h"

namespace Client {}
using namespace Client;