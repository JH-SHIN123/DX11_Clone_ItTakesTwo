#pragma once

#include"Client_Defines.h"
#include"Base.h"


BEGIN(Client)
class CFilm : public CBase
{

public:
	enum	   ScreenType { LScreen, RScreen, ScreenType_End };
	enum class CamMoveOption { Move_Jump, Move_Straight, Move_Bezier_3, Move_Bezier_4, End };
	enum class CamNodeVectorType { Eye, At, End };
	enum class ViewPortOption
	{
		//진행방향 
		LScreenType_Split_Immediate, LScreenType_Split_Lerp, //왼쪽 즉시 화면분할 , 왼쪽 보간 화면분할		-> 		
		LScreenType_Merge_Immediate, LScreenType_Merge_Lerp, //왼쪽 즉시 화면병합 , 왼쪽 보간 화면병합		<-	*왼쪽화면 사라짐
		RScreenType_Split_Immediate, RScreenType_Split_Lerp, //오른쪽 즉시 화면분할, 오른쪽 보간 화면분할	<-	
		RScreenType_Merge_Immediate, RScreenType_Merge_Lerp, //오른쪽 즉시 화면병합, 오른쪽 보간 화면병합	-> *오른쪽 화면사라짐
		End
	};
	enum BezierNode { Bezier_1, Bezier_2, Bezier_3, Bezier_4, Bezier_End };
	typedef struct tagCamNode
	{
		_double			dTime = 0.f;
		//For.Move
		_float3			vEye = { 0.f,0.f,0.f };
		_float3			vAt = { 0.f,0.f,0.f };

		CamMoveOption	eEyeMoveOption = CamMoveOption::End;
		CamMoveOption	eAtMoveOption = CamMoveOption::End;
		//For.ViewPort
		_float			fTargetViewPortCenterX = 0.f;
		_float			fTargetViewPortCenterY = 1.f;
		_float			fViewPortLerpSpeed = 0.f;
		ViewPortOption	eViewPortOption = ViewPortOption::End;
	}CamNode;

public:
	explicit CFilm();
	virtual ~CFilm() = default;

public:
	HRESULT NativeConstruct(const _tchar* pFilmName);


public:		//For.System
	void Set_Duration(_double dDuration) { m_dDuration = dDuration; }
	_double Get_Duration() { return m_dDuration; }
	
public:
	HRESULT Add_Node(CamNode* pNode);
	_bool	Get_IsEnd(ScreenType eScreenType) { return m_bIsEnd[eScreenType]; }
	const	_tchar* Get_Name() { return m_szFilmName; }

	vector<CamNode*>* Get_CamNodes() { return &m_CamNodes; }
	//얘가 매트릭스 만들고 뷰포트 설정하고
	HRESULT Tick_Film(_double dTimeDelta, ScreenType eScreenType);
	//Cam_Helper 가 이거 받아와서 카메라에 셋팅.
	_fmatrix Get_CurNodeMatrix(ScreenType eScreenType);

	_fmatrix MakeCurCamMatrix(_double dTimeDelta, CamNode* pCurNode, ScreenType eScreenType);
	void ReSetFilm(ScreenType eScreenType);
public:
	static CFilm* Create(const _tchar* pFilmName);
	virtual void Free()override;
private:	//For.Camera
	_fmatrix MakeViewMatrix(_float3 Eye, _float3 At);
	_fmatrix MakeMatrixLerp(_fmatrix matDst, _fmatrix matSour, _float fLerpTime);
	_float3  VectorLerp(_float3& vDst, _float3& vSour, _float t);
	HRESULT  Check_CamNodeProgress(ScreenType eScreenType, CamMoveOption eOption, CamNodeVectorType eType, _uint iCurrentNode, _uint iLastNode);
	void	 ReSet_CamNodeTime_Progress_End(ScreenType eScreenType, CamMoveOption eOption, CamNodeVectorType eType);
	_float3	 MakeBezier3(_float3& v1, _float3& v2, _float3& v3, _double dTime);
	_float3	 MakeBezier4(_float3& v1, _float3& v2, _float3& v3, _float3& v4, _double dTime);


private:
	
	TCHAR m_szFilmName[MAX_PATH] = L"";
	vector<CamNode*> m_CamNodes;
	_uint m_iCurrentNode[ScreenType_End];
	_bool m_bIsEnd[ScreenType_End];
	_float4x4 m_matCam[ScreenType_End];
	_double m_dTime[ScreenType_End];
	_double m_dDuration;
	ViewPortOption m_eCurViewPortOption;

	//For.CamEye
	CamMoveOption m_eCurEye_CamMoveOption;

	_double m_dCamMoveTime[ScreenType_End][(_uint)CamMoveOption::End];
	_bool m_bCurEye_StartBezier[ScreenType_End] = { false,false };
	_uint m_iCurEye_BezierNode[ScreenType_End][Bezier_End];
	//For.CamAt
	CamMoveOption m_eCurAt_CamMoveOption;

	_double m_dCamAtMoveTime[ScreenType_End][(_uint)CamMoveOption::End];
	_bool m_bCurAt_StartBezier[ScreenType_End] = { false,false };
	_uint m_iCurAt_BezierNode[ScreenType_End][Bezier_End];

	_bool m_bCurNodeEnd[ScreenType_End][(_uint)CamNodeVectorType::End];

};
END

