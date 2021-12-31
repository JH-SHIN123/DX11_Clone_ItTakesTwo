#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEffect_StarBuddy_Move final : public CGameObject
{
private:
	explicit CEffect_StarBuddy_Move(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_StarBuddy_Move(const CEffect_StarBuddy_Move& rhs);
	virtual ~CEffect_StarBuddy_Move() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_WorldMatrix(_fmatrix WorldMatrix);
	void Set_Launch(_bool IsLaunch = true);
	void Set_ColorIndex(_int iColorIndex);

private:
	void Set_Particle_Matrix();
	_int Dead_Func();

private:
	_bool m_IsLaunch = false;

private:
	class CEffect_StarBuddy_Move_Particle* m_pParticle_1 = nullptr; // 머리
	class CEffect_StarBuddy_Move_Particle* m_pParticle_2 = nullptr; // 오른손
	class CEffect_StarBuddy_Move_Particle* m_pParticle_3 = nullptr; // 왼손
	class CEffect_StarBuddy_Move_Particle* m_pParticle_4 = nullptr; // 오른발
	class CEffect_StarBuddy_Move_Particle* m_pParticle_5 = nullptr; // 왼발

	CTransform* m_pTransformCom = nullptr;

public:
	static CEffect_StarBuddy_Move* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override; private:
};

END