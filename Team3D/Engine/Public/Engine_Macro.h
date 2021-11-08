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
#define FAILED_CHECK_RETURN(_hr, _return)		if( ((HRESULT)(_hr)) < 0 ){ __debugbreak(); return _return; }\
//pGameInstance->Add_GameObject_Prototype(UIDesc.iLevelIndex, PSData.szUITag,
//Å¬·¡½º::Create)
#define STRCMP_ADD_PROTOTYPE(pGameInstance, iLevelIndex, szUITag,_className) ( pGameInstance->Add_GameObject_Prototype(iLevelIndex, \
PSData.szUITag, _className::Create) 


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

#pragma region Return
#define NO_EVENT		0
#define EVENT_DEAD		1
#define EVENT_ERROR		-1
#pragma endregion