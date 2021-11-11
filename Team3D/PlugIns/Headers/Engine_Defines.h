#pragma once

#pragma warning (disable : 4251)

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
/* Assimp */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
/* PhysX */
#include "PxPhysicsAPI.h"
/* Others */
#include <random>
#include <ctime>
#include <unordered_map>
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

/* Convenience */
#include "MathHelper.h"

using namespace Engine;