#pragma once

#pragma warning (disable : 4251)

/* Microsoft */
#define _CRT_SECURE_NO_WARNINGS 
#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#include <wrl.h>
/* DirectX */
#include "D3D11.h"
#include "d3dx11effect.h"
#include "d3dxGlobal.h"
#include "D3DCompiler.h"
#include "DirectXTex.h"
#include "DirectXCollision.h"
#include <DirectXTK/Effects.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
/* DirectInput */
#define DIRECTINPUT_VERSION 0x0800
#include "dInput.h"
/* PhysX */
#include "PxPhysicsAPI.h"
/* Others */
#include <random>
#include <ctime>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <functional>

using namespace std;
using namespace DirectX;
using namespace physx;

#ifndef _AFX
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifndef DBG_NEW
#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif // _DBG_NEW
#endif // _DEBUG
#endif // _MFC

/* Engine */
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_ID.h"
#include "Engine_MathHelper.h"

using namespace Engine;