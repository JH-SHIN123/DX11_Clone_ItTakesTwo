#pragma once

#include"Client_Defines.h"
#include"Base.h"


BEGIN(Client)
class CGrindLine : public CBase
{

public:
	enum	   ScreenType { LScreen, RScreen, ScreenType_End };
	typedef struct tagGrindLineNode
	{
		_double			dTime = 0.f;
		//For.Move
		_float3			vPos = { 0.f,0.f,0.f };
		_float3			vLook = { 0.f,0.f,0.f };
	}GrindLineNode;


public:
	explicit CGrindLine();
	virtual ~CGrindLine() = default;

public:
	HRESULT NativeConstruct(const _tchar* pFilmName);


public:		//For.System
	void Set_Duration(_double dDuration) { m_dDuration = dDuration; }
	_double Get_Duration() { return m_dDuration; }

public:
	HRESULT Add_Node(GrindLineNode* pNode);
	_bool	Get_IsEnd(ScreenType eScreenType) { return m_bIsEnd[eScreenType]; }
	const	_tchar* Get_Name() { return m_szGrindLineName; }

	vector<GrindLineNode*>* Get_CamNodes() { return &m_GrindLineNodes; }
	//얘가 매트릭스 만들고 
	HRESULT Tick_GrindLine(_double dTimeDelta, ScreenType eScreenType);
	//이거받아와서 플레이어 셋팅
	_fmatrix Get_CurNodeMatrix(ScreenType eScreenType);

	_fmatrix MakeCurProgressMatrix(_double dTimeDelta, GrindLineNode* pCurNode, ScreenType eScreenType);
	void ReSetGrindLine(ScreenType eScreenType); // For.NativeConstruct
public:
	static CGrindLine* Create(const _tchar* pFilmName);
	virtual void Free()override;
private:	//For.Camera
	_fmatrix MakeMatrix(_float3 Pos, _float3 Look);

	_float3  VectorLerp(_float3& vDst, _float3& vSour, _float t);
	HRESULT  Check_GrindNodeProgress(ScreenType eScreenType, _uint iCurrentNode, _uint iLastNode);

private:

	TCHAR m_szGrindLineName[MAX_PATH] = L"";
	vector<GrindLineNode*> m_GrindLineNodes;
	_uint m_iCurrentNode[ScreenType_End];
	_bool m_bIsEnd[ScreenType_End];
	_float4x4 m_matCurProgress[ScreenType_End];
	_double m_dProgressTime[ScreenType_End];
	_double m_dDuration;


	//노드 사이의 진행시간
	_double m_dMoveTime[ScreenType_End];
	
	_bool m_bCurNodeEnd[ScreenType_End];


};
END

