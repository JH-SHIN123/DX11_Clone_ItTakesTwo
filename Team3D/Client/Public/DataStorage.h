#pragma once

#include "GameObject.h"

BEGIN(Client)

#define DATABASE CDataStorage::GetInstance()

class CDataStorage final : public CBase
{
	DECLARE_SINGLETON(CDataStorage)
private:
	explicit CDataStorage() = default;
	virtual ~CDataStorage() = default;
public:

	void Set_CodyPtr(CGameObject* pCody) { m_pCody = pCody; }
	CGameObject* GetCody() { return m_pCody; }

	void Set_MayPtr(CGameObject* pMay) { m_pMay = pMay; }
	CGameObject* GetMay() { return m_pMay; }

	void Set_MainCamPtr(CGameObject* pMainCam) { m_pMainCam = pMainCam; }
	CGameObject* Get_MainCam() { return m_pMainCam; }

	void Set_SubCamPtr(CGameObject* pSubCam) { m_pSubCam = pSubCam; }
	CGameObject* Get_SubCam() { return m_pSubCam; }

	void Set_UFOPtr(CGameObject* pUFO) { m_pUFO = pUFO; }
	CGameObject* Get_UFO() { return m_pUFO; }

private:
	CGameObject* m_pCody = nullptr;
	CGameObject* m_pMay = nullptr;
	CGameObject* m_pMainCam = nullptr;
	CGameObject* m_pSubCam = nullptr;

	// UFO
	CGameObject* m_pUFO = nullptr;

public:
	virtual void Free() override;
};

END