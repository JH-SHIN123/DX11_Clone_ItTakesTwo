#pragma once

#define BEGIN(NAMESPACE) namespace NAMESPACE {
#define END }

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#ifndef MSG_BOX
#define	MSG_BOX(_message) MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define NULL_CHECK(_ptr)						if( _ptr == 0 ){ __debugbreak(); return; }
#define NULL_CHECK_RETURN(_ptr, _return)		if( _ptr == 0 ){ __debugbreak(); return _return; }
#define NOT_NULL_CHECK(_ptr)					if( _ptr != 0 ){ __debugbreak(); return; }
#define NOT_NULL_CHECK_RETURN(_ptr, _return)	if( _ptr != 0 ){ __debugbreak(); return _return; }
#define FAILED_CHECK(_hr)						if( ((HRESULT)(_hr)) < 0 ){ __debugbreak(); return; }
#define FAILED_CHECK_RETURN(_hr, _return)		if( ((HRESULT)(_hr)) < 0 ){ __debugbreak(); return _return; }

#define NO_COPY(CLASSNAME)									\
	private:												\
	CLASSNAME(const CLASSNAME&);							\
	CLASSNAME& operator = (const CLASSNAME&);				

#define DECLARE_SINGLETON(CLASSNAME)						\
	NO_COPY(CLASSNAME)										\
	private:												\
	static CLASSNAME*	m_pInstance;						\
	public:													\
	static CLASSNAME*	GetInstance( void );				\
	static unsigned long DestroyInstance( void );

#define IMPLEMENT_SINGLETON(CLASSNAME)						\
	CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
	CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
		if(nullptr == m_pInstance) {						\
			m_pInstance = new CLASSNAME;					\
		}													\
		return m_pInstance;									\
	}														\
	unsigned long CLASSNAME::DestroyInstance( void ) {		\
		unsigned long dwRefCnt = 0;							\
		if(nullptr != m_pInstance)	{						\
			dwRefCnt = Safe_Release(m_pInstance);			\
			if(0 == dwRefCnt)								\
				m_pInstance = nullptr;						\
		}													\
		return dwRefCnt;									\
	}

#pragma region TextureType
enum aiTextureType
{
	aiTextureType_NONE = 0,
	aiTextureType_DIFFUSE = 1,
	aiTextureType_SPECULAR = 2,
	aiTextureType_AMBIENT = 3,
	aiTextureType_EMISSIVE = 4,
	aiTextureType_HEIGHT = 5,
	aiTextureType_NORMALS = 6,
	aiTextureType_SHININESS = 7,
	aiTextureType_OPACITY = 8,
	aiTextureType_DISPLACEMENT = 9,
	aiTextureType_LIGHTMAP = 10,
	aiTextureType_REFLECTION = 11,
	aiTextureType_BASE_COLOR = 12,
	aiTextureType_NORMAL_CAMERA = 13,
	aiTextureType_EMISSION_COLOR = 14,
	aiTextureType_METALNESS = 15,
	aiTextureType_DIFFUSE_ROUGHNESS = 16,
	aiTextureType_AMBIENT_OCCLUSION = 17,
	aiTextureType_UNKNOWN = 18,
};
#define AI_TEXTURE_TYPE_MAX  aiTextureType_UNKNOWN
#pragma endregion

#pragma region Return
#define NO_EVENT		0
#define EVENT_DEAD		1
#define EVENT_ERROR		-1
#pragma endregion

#pragma region PHYSX
#define GRAVITY			9.8f
#pragma endregion

#pragma region PostProcessing
#define MAX_CASCADES		4
#define SHADOWMAP_SIZE		2048 /* Width size equal height Size */
#pragma endregion