#pragma once
#include "Base.h"
class CDataBase :public CBase
{

	DECLARE_SINGLETON(CDataBase)
public:
	CDataBase();
	virtual ~CDataBase() = default;



public:
	void Set_CodyPtr(class CGameObject* pCody) { m_pCody = pCody; }
	CGameObject* GetCody() { return m_pCody; }

	void Set_MayPtr(class CGameObject* pMay) { m_pMay = pMay; }
	CGameObject* GetMay() { return m_pMay; }

	void Set_MainCamPtr(class CGameObject* pMainCam) { m_pMainCam = pMainCam; }
	CGameObject* Get_MainCam() { return m_pMainCam; }

	void Set_SubCamPtr(class CGameObject* pSubCam) { m_pSubCam = pSubCam; }
	CGameObject* Get_SubCam() { return m_pSubCam; }

private:
	class CGameObject* m_pCody = nullptr;
	class CGameObject* m_pMay = nullptr;
	class CGameObject* m_pMainCam = nullptr;
	class CGameObject* m_pSubCam = nullptr;

public:
	virtual void Free();
};

