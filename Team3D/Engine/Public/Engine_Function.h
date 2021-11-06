#pragma once

namespace Engine
{
	template<typename T>
	void Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		if (nullptr == pInstance)
			return 0;

		return pInstance->AddRef();
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long dwRefCnt = 0;
		
		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	class CTagFinder
	{
	public:
		explicit CTagFinder(const wchar_t* pTag) : m_pTargetTag(pTag){}
		~CTagFinder() = default;

	public:
		template<typename T>
		bool operator()(const T& Pair)
		{
			if (0 == lstrcmp(m_pTargetTag, Pair.first))
				return true;
			return false;
		}

	private:
		const wchar_t*	m_pTargetTag;
	};

	class CTagFinderS
	{
	public:
		explicit CTagFinderS(const char* pTag) : m_pTargetTag(pTag) {}
		~CTagFinderS() = default;

	public:
		template<typename T>
		bool operator()(const T& Pair)
		{
			if (0 == lstrcmp(m_pTargetTag, Pair.first))
				return true;
			return false;
		}

	private:
		const char*	m_pTargetTag;
	};
}

/*
	CTagFinder는 멀티바이트 문자열 비교용.
	CTagFinderS는 단일바이트 문자열 비교용.
*/