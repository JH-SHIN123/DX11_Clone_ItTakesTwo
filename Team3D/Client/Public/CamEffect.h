#pragma once
#include"Client_Defines.h"
#include"Film.h"

BEGIN(Client)
class CCamEffect :	public CBase
{
public:
	enum class CamShakeOption
	{
		CamShake_Loc_Right, CamShake_Loc_Up, CamShake_Loc_Look, //카메라 좌우,상하,전후 이동 진동
		CamShake_Rot_Right, CamShake_Rot_Up, CamShake_Rot_Look, //카메라  좌우, 상하, 전후 회전 진동
		CamShake_End
	};
	typedef struct tagCamShakeDesc
	{
		_double	dMaxForce = 0.0;	// 90도일때의  힘
		_double dMinForce = 0.0;	// 270도일때의 힘

	}CamShakeDesc;

	typedef struct tagCamShakeCycleDesc //사인 한 사이클에 대한 정보
	{
		tagCamShakeCycleDesc()
		{
			ZeroMemory(bOnCamShakeOption, sizeof(_bool) * (_uint)CamShakeOption::CamShake_End);
			ZeroMemory(tCamShakeDesc, sizeof(CamShakeDesc) * (_uint)CamShakeOption::CamShake_End);
		}
		_double dStartTime = 0.0; //시작시간
		_double dMiddleTime = 0.0; // 중앙. 0~ 180 까지의 시간
		_double dFinishTime = 0.0; //끝낼시간

		_bool	bOnCamShakeOption[(_uint)CamShakeOption::CamShake_End];
		CamShakeDesc	tCamShakeDesc[(_uint)CamShakeOption::CamShake_End];

	}CamShakeCycleDesc;

	enum BlendState{Blend_None,Blend_In,Blend_Out,Blend_End};
private:
	CCamEffect();
	virtual ~CCamEffect() = default;
public:
	HRESULT NativeConstruct(const _tchar* pEffectName);
	_bool	IsFinish(CFilm::ScreenType eScreen) { return m_bFinish[eScreen]; }
public:
	HRESULT Tick_CamEffect(CFilm::ScreenType eScreen, _double dTimeDelta,_fmatrix matIn);
public:
	_fmatrix Get_ApplyEffectMatrix(CFilm::ScreenType eScreen);
	const _tchar* Get_Name() { return m_szName; }


public:
	HRESULT Add_CamShakeCycleDesc(CamShakeCycleDesc* pDesc);
	void	ReSet_Effect(CFilm::ScreenType eScreen);
	void	Set_Duration(_double dDuration) { m_dDuration = dDuration; }
private:
	_fmatrix MakeShakeMatrix_Location(_fmatrix matIn, _double dCurrentTime, CamShakeCycleDesc& tDesc);
	_fmatrix MakeShakeMatrix_QuarternionRot(_fmatrix matIn,_double dCurrentTime,CamShakeCycleDesc& tDesc);
public:
	static CCamEffect* Create(const _tchar* pName);
	virtual void Free() override;

	vector<CamShakeCycleDesc*>* GetShakeDescs() { return &m_ShakeDescs; }
private:
	TCHAR m_szName[MAX_PATH];
	vector<CamShakeCycleDesc*> m_ShakeDescs;
	_double m_dDuration = 0.0;

	
	_double m_dTime[CFilm::ScreenType::Screen_End];
	_uint m_iCurrentIdx[CFilm::ScreenType::Screen_End];
	_float4x4 m_matApplyEffectMatrix[CFilm::ScreenType::Screen_End];
	_bool	m_bFinish[CFilm::ScreenType::Screen_End];


	//For.BlendIn,BlendOut

public:
	void Set_IsBlendIn(_bool bSet) { m_bIsBlendIn = bSet; }
	void Set_IsBlendOut(_bool bSet) { m_bIsBlendOut = bSet; }

	void Set_BlendInTime(_float fBlendInTime) { m_fBlendInTime = fBlendInTime; }
	void Set_BlendOutTime(_float fBlendOutTime) { m_fBlendOutTime = fBlendOutTime; }

private:
	_bool m_bIsBlendIn = false,m_bIsBlendOut = false;
	_float m_fBlendInTime = 0.f, m_fBlendOutTime = 0.f;
	_double m_dCurBlendInTime[CFilm::ScreenType::Screen_End];
	_double m_dCurBlendOutTime[CFilm::ScreenType::Screen_End];
};

END