#pragma once
#include "Base.h"

class CDataStorage final : public CBase
{
	DECLARE_SINGLETON(CDataStorage)
public:
	CDataStorage();
	virtual ~CDataStorage() = default;



public:
	void Set_CodyPtr(class CGameObject* pCody) { m_pCody = pCody; }
	CGameObject* GetCody() { return m_pCody; }

	void Set_MayPtr(class CGameObject* pMay) { m_pMay = pMay; }
	CGameObject* GetMay() { return m_pMay; }

	void Set_MainCamPtr(class CGameObject* pMainCam) { m_pMainCam = pMainCam; }
	CGameObject* Get_MainCam() { return m_pMainCam; }

	void Set_SubCamPtr(class CGameObject* pSubCam) { m_pSubCam = pSubCam; }
	CGameObject* Get_SubCam() { return m_pSubCam; }

	void Set_UFOPtr(class CGameObject* pUFO) { m_pUFO = pUFO; }
	CGameObject* Get_UFO() { return m_pUFO; }

private:
	class CGameObject* m_pCody = nullptr;
	class CGameObject* m_pMay = nullptr;
	class CGameObject* m_pMainCam = nullptr;
	class CGameObject* m_pSubCam = nullptr;

	// UFO
	class CGameObject* m_pUFO = nullptr;

public:
	virtual void Free() override;
};

