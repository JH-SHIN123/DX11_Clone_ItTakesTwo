#pragma once
#include "Effect_Env_Particle_Field.h"

BEGIN(Client)
class CEffect_Env_Particle_Field_Dust final : public CEffect_Env_Particle_Field
{
protected:
	explicit CEffect_Env_Particle_Field_Dust(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Env_Particle_Field_Dust(const CEffect_Env_Particle_Field& rhs);
	virtual ~CEffect_Env_Particle_Field_Dust() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	void Check_State(_double TimeDelta);
	void State_Start(_double TimeDelta);
	void State_Disappear(_double TimeDelta);

public:
	static CEffect_Env_Particle_Field_Dust* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END