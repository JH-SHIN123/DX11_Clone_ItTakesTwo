#include "stdafx.h"
#include "..\Public\Script.h"

CScript::CScript(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CScript::CScript(const CScript & rhs)
	: CGameObject(rhs)
{
}

HRESULT CScript::Render_Script(_uint iScriptIndex, SCREEN eScreenMode, _float fDeadTime, _bool bBackGround)
{
	m_iIndexFirst = iScriptIndex;
	m_iIndexSecond = iScriptIndex;
	m_eScreenMode = eScreenMode;
	m_fDeadCheck = fDeadTime;

	m_isDouble = false;
	m_bRender = true;
	m_bBackGround = bBackGround;

	if (HALF == m_eScreenMode)
	{
		m_pBackGround_First->Set_Position(XMVectorSet(-320.f, -312.f, 0.f, 1.f));
		m_pBackGround_Sub->Set_Position(XMVectorSet(320.f, -312.f, 0.f, 1.f));
	}
	else if (BOSS == m_eScreenMode)
	{
		m_pBackGround_First->Set_Position(XMVectorSet(-320.f + 64.f, -312.f, 0.f, 1.f));
		m_pBackGround_Sub->Set_Position(XMVectorSet(320.f + 64.f, -312.f, 0.f, 1.f));
	}
	else if (ASYMMETRY == m_eScreenMode)
	{
		m_pBackGround_First->Set_Position(XMVectorSet(-320.f + 96.f, -312.f, 0.f, 1.f));
		m_pBackGround_Sub->Set_Position(XMVectorSet(320.f + 96.f, -312.f, 0.f, 1.f));
	}
	else
	{
		m_pBackGround_First->Set_Position(XMVectorSet(0.f, -312.f, 0.f, 1.f));
		m_pBackGround_Sub->Set_Position(XMVectorSet(320.f, -312.f, 0.f, 1.f));
	}

	return S_OK;
}

HRESULT CScript::Render_Script_DoubleLine(_uint iIndexFirst, _uint iIndexSecond, _float fDeadTime, _bool bBackGround)
{
	m_iIndexFirst = iIndexFirst;
	m_iIndexSecond = iIndexSecond;
	m_fDeadCheck = fDeadTime;

	m_isDouble = true;
	m_bRender = true;
	m_bBackGround = bBackGround;

	m_pBackGround_First->Set_Position(XMVectorSet(0.f, -312.f, 0.f, 1.f));

	return S_OK;
}

#pragma region Voice
void CScript::VoiceFile_No01()
{
	m_pGameInstance->Play_Sound(TEXT("01.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(0, 4.f));
}

void CScript::VoiceFile_No02()
{
	m_pGameInstance->Play_Sound(TEXT("02.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(1, 4.f));
}

void CScript::VoiceFile_No03()
{
	m_pGameInstance->Play_Sound(TEXT("03.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(2, 4.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(3, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(4, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(5, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(6, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(7, 3.f));
}

void CScript::VoiceFile_No04()
{
	m_pGameInstance->Play_Sound(TEXT("04.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(8, 4.f));
}

void CScript::VoiceFile_No05()
{
	m_pGameInstance->Play_Sound(TEXT("05.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(9, 5.f));
}

void CScript::VoiceFile_No06()
{
	m_pGameInstance->Play_Sound(TEXT("06.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(10, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(11, 2.f));
}

void CScript::VoiceFile_No07()
{
	m_pGameInstance->Play_Sound(TEXT("07.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(12, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(13, 2.f));
}

void CScript::VoiceFile_No08()
{
	//m_pGameInstance->Play_Sound(TEXT("08.wav"), CHANNEL_VOICE);
	//m_queueScriptInfo.emplace(SCRIPTINFO(14, 3.f));
	//m_queueScriptInfo.emplace(SCRIPTINFO(15, 2.f));
}

void CScript::VoiceFile_No09()
{
	m_pGameInstance->Play_Sound(TEXT("09.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(16, 4.f));
}

void CScript::VoiceFile_No10()
{
	m_pGameInstance->Play_Sound(TEXT("10.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(17, 4.f));
}

void CScript::VoiceFile_No11()
{
	m_pGameInstance->Play_Sound(TEXT("11.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(18, 2.f, SCREEN::FULL));
}

void CScript::VoiceFile_No12()
{
	m_pGameInstance->Play_Sound(TEXT("12.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(19, 2.f, SCREEN::FULL));
}

void CScript::VoiceFile_No13()
{
	m_pGameInstance->Play_Sound(TEXT("13.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(20, 2.f, SCREEN::FULL));
}

void CScript::VoiceFile_No14()
{
	m_pGameInstance->Play_Sound(TEXT("14.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(21, 2.f, SCREEN::FULL));
}

void CScript::VoiceFile_No15()
{
	m_pGameInstance->Play_Sound(TEXT("15.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(22, 2.f, SCREEN::FULL));
}

void CScript::VoiceFile_No16()
{
	m_pGameInstance->Play_Sound(TEXT("16.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(23, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(24, 2.f));
}

void CScript::VoiceFile_No17()
{
	m_pGameInstance->Play_Sound(TEXT("17.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(25, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(26, 2.f));
}

void CScript::VoiceFile_No18()
{
	m_pGameInstance->Play_Sound(TEXT("18.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(27, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(28, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(29, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(30, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(31, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(32, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(33, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(34, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(35, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(36, 3.f));
}

void CScript::VoiceFile_No19()
{
	m_pGameInstance->Play_Sound(TEXT("19.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(37, 2.f));
}

void CScript::VoiceFile_No20()
{
	m_pGameInstance->Play_Sound(TEXT("20.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(38, 2.f, ASYMMETRY));
}

void CScript::VoiceFile_No21()
{
	m_pGameInstance->Play_Sound(TEXT("21.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(39, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(40, 3.f));
}

void CScript::VoiceFile_No22()
{
	m_pGameInstance->Play_Sound(TEXT("22.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(41, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(42, 3.f));
}

void CScript::VoiceFile_No23()
{
	m_pGameInstance->Play_Sound(TEXT("23.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(43, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(44, 2.f));
}

void CScript::VoiceFile_No24()
{
	m_pGameInstance->Play_Sound(TEXT("24.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(45, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(46, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(47, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(48, 1.5f));
	m_queueScriptInfo.emplace(SCRIPTINFO(49, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(50, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(51, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(52, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(53, 2.f));
}

void CScript::VoiceFile_No25()
{
	m_pGameInstance->Play_Sound(TEXT("25.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(54, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(55, 4.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(56, 2.f));
}

void CScript::VoiceFile_No26()
{
	m_pGameInstance->Play_Sound(TEXT("26.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(57, 4.f));
}

void CScript::VoiceFile_No27()
{
	m_pGameInstance->Play_Sound(TEXT("27.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(58, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(59, 2.f));
}

void CScript::VoiceFile_No28()
{
	m_pGameInstance->Play_Sound(TEXT("28.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(60, 4.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(61, 4.f));
}

void CScript::VoiceFile_No29()
{
	m_pGameInstance->Play_Sound(TEXT("29.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(62, 5.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(63, 4.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(64, 3.f));
}

void CScript::VoiceFile_No30()
{
	m_pGameInstance->Play_Sound(TEXT("30.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(65, 5.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(66, 2.f));
}

void CScript::VoiceFile_No31()
{
	m_pGameInstance->Play_Sound(TEXT("31.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(67, 2.f, FULL));
	m_queueScriptInfo.emplace(SCRIPTINFO(68, 2.f, FULL));
	m_queueScriptInfo.emplace(SCRIPTINFO(69, 2.f, FULL));
}

void CScript::VoiceFile_No32()
{
	m_pGameInstance->Play_Sound(TEXT("32.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(70, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(71, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(72, 3.f));
}

void CScript::VoiceFile_No33()
{
	m_pGameInstance->Play_Sound(TEXT("33.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(73, 6.f));
}

void CScript::VoiceFile_No34()
{
	m_pGameInstance->Play_Sound(TEXT("34.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(74, 3.5f));
	m_queueScriptInfo.emplace(SCRIPTINFO(75, 2.f));
}

void CScript::VoiceFile_No35()
{
	m_pGameInstance->Play_Sound(TEXT("35.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(76, 3.f, FULL));
}

void CScript::VoiceFile_No36()
{
	m_pGameInstance->Play_Sound(TEXT("36.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(77, 4.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(78, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(79, 2.f));
}

void CScript::VoiceFile_No37()
{
	m_pGameInstance->Play_Sound(TEXT("37.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(80, 3.f));
}

void CScript::VoiceFile_No38()
{
	m_pGameInstance->Play_Sound(TEXT("38.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(81, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(82, 2.f));
}

void CScript::VoiceFile_No39()
{
	m_pGameInstance->Play_Sound(TEXT("39.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(83, 2.f, BOSS));
}

void CScript::VoiceFile_No40()
{
	m_pGameInstance->Play_Sound(TEXT("40.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(84, 2.f));
}

void CScript::VoiceFile_No41()
{
	m_pGameInstance->Play_Sound(TEXT("41.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(85, 3.f, BOSS));
}

void CScript::VoiceFile_No42()
{
	m_pGameInstance->Play_Sound(TEXT("42.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(86, 3.f, BOSS));
}

void CScript::VoiceFile_No43()
{
	m_pGameInstance->Play_Sound(TEXT("43.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(87, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(88, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(89, 4.f));
}

void CScript::VoiceFile_No44()
{
	m_pGameInstance->Play_Sound(TEXT("44.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(90, 3.f));
}

void CScript::VoiceFile_No45()
{
	m_pGameInstance->Play_Sound(TEXT("45.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(91, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(92, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(93, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(94, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(95, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(96, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(97, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(98, 2.f));
}

void CScript::VoiceFile_No46()
{
	m_pGameInstance->Play_Sound(TEXT("46.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(99, 3.f, BOSS));
}

void CScript::VoiceFile_No47()
{
	m_pGameInstance->Play_Sound(TEXT("47.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(100, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(101, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(102, 2.f));
}

void CScript::VoiceFile_No48()
{
	m_pGameInstance->Play_Sound(TEXT("48.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(103, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(104, 2.f));
}

void CScript::VoiceFile_No49()
{
	m_pGameInstance->Play_Sound(TEXT("49.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(105, 3.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(106, 2.f));
}

void CScript::VoiceFile_No50()
{
	m_pGameInstance->Play_Sound(TEXT("50.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(107, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(108, 2.f));
}

void CScript::VoiceFile_No51()
{
	m_pGameInstance->Play_Sound(TEXT("51.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(109, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(110, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(111, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(112, 4.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(113, 3.f));
}

void CScript::VoiceFile_No52()
{
	m_pGameInstance->Play_Sound(TEXT("52.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(114, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(115, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(116, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(117, 5.f));
}

void CScript::VoiceFile_No53()
{
	m_pGameInstance->Play_Sound(TEXT("53.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(118, 4.f));
}

void CScript::VoiceFile_No54()
{
	m_pGameInstance->Play_Sound(TEXT("54.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(119, 1.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(120, 2.f));
}

void CScript::VoiceFile_No55()
{
	m_pGameInstance->Play_Sound(TEXT("55.wav"), CHANNEL_VOICE);
	m_queueScriptInfo.emplace(SCRIPTINFO(121, 2.f));
	m_queueScriptInfo.emplace(SCRIPTINFO(122, 3.f));
}
#pragma endregion

HRESULT CScript::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CScript::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pBackGround_First->Set_Position(XMVectorSet(-320.f, -312.f, 0.f, 1.f));
	m_pBackGround_Sub->Set_Position(XMVectorSet(320.f, -312.f, 0.f, 1.f));

	DATABASE->Set_Script(this);

	return S_OK;
}

_int CScript::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_fChangeTimeCheck += (_float)dTimeDelta;
	if (false == m_queueScriptInfo.empty())
	{
		m_bRender = true;
		if (m_dChangeTime <= m_fChangeTimeCheck)
		{
			m_fChangeTimeCheck = 0.f;
			m_dChangeTime = m_queueScriptInfo.front().dChangeTime;
			Render_Script(m_queueScriptInfo.front().iScriptIndex, m_queueScriptInfo.front().eScreen, m_queueScriptInfo.front().dChangeTime);
			m_queueScriptInfo.pop();
		}
	}
	else
	{
		if (true == m_bRender)
		{
			m_dDeadTime += dTimeDelta;
			if (m_fDeadCheck <= m_dDeadTime)
			{
				m_bRender = false;
				m_dDeadTime = 0.f;
			}
		}
	}

	return NO_EVENT;
}

_int CScript::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (false == m_bRender)
		return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CScript::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	/* 한줄 */
	if (false == m_isDouble)
	{
		/* 폰트 길이에 따라 백그라운드 사이즈 조절*/
		_float fLength = m_pFontCom_First->Get_TextSizeX(Find_Script(m_iIndexFirst), 0.23f);
		fLength *= 1.7f;

		/* 5:5 */
		if (HALF == m_eScreenMode)
		{
			if (true == m_bBackGround)
			{
				m_pBackGround_First->Render(eGroup, _float2(fLength, 1.f));
				m_pBackGround_Sub->Render(eGroup, _float2(fLength, 1.f));
			}
			m_pFontCom_First->Render_Font(Find_Script(m_iIndexFirst), _float2(320.f, 670.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
			m_pFontCom_Second->Render_Font(Find_Script(m_iIndexFirst), _float2(960.f, 670.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
		}
		/* 0.6 */
		else if (BOSS == m_eScreenMode)
		{
			if (true == m_bBackGround)
			{
				m_pBackGround_First->Render(eGroup, _float2(fLength, 1.f));
				m_pBackGround_Sub->Render(eGroup, _float2(fLength, 1.f));
			}
			m_pFontCom_First->Render_Font(Find_Script(m_iIndexFirst), _float2(320.f + 64.f, 670.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
			m_pFontCom_Second->Render_Font(Find_Script(m_iIndexFirst), _float2(960.f + 64.f, 670.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
		}
		/* 0.65 */
		else if (ASYMMETRY == m_eScreenMode)
		{
			if (true == m_bBackGround)
			{
				m_pBackGround_First->Render(eGroup, _float2(fLength, 1.f));
				m_pBackGround_Sub->Render(eGroup, _float2(fLength, 1.f));
			}
			m_pFontCom_First->Render_Font(Find_Script(m_iIndexFirst), _float2(320.f + 96.f, 670.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
			m_pFontCom_Second->Render_Font(Find_Script(m_iIndexFirst), _float2(960.f + 96.f, 670.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
		}
		/* 풀스크린 */
		else
		{
			if(true == m_bBackGround)
				m_pBackGround_First->Render(eGroup, _float2(fLength, 1.f));

			m_pFontCom_First->Render_Font(Find_Script(m_iIndexFirst), _float2(640.f, 670.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
		}
	}
	/* 두줄 */
	else
	{
		if (true == m_bBackGround)
		{
			_float fLength = 0.f;
			_float fLength_First = m_pFontCom_First->Get_TextSizeX(Find_Script(m_iIndexFirst), 0.23f);
			_float fLength_Second = m_pFontCom_First->Get_TextSizeX(Find_Script(m_iIndexSecond), 0.23f);

			if (fLength_First > fLength_Second)
				fLength = fLength_First * 1.7f;
			else
				fLength = fLength_Second * 1.7f;

			m_pBackGround_First->Render(eGroup, _float2(fLength, 1.8f));
		}

		m_pFontCom_First->Render_Font(Find_Script(m_iIndexFirst), _float2(640.f, 661.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
		m_pFontCom_Second->Render_Font(Find_Script(m_iIndexSecond), _float2(640.f, 684.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.23f);
	}

	return S_OK;
}

HRESULT CScript::Ready_Component(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_Font_First"), (CComponent**)&m_pFontCom_First), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_FontDraw"), TEXT("Com_Font_Second"), (CComponent**)&m_pFontCom_Second), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Script"), Level::LEVEL_STAGE, TEXT("GameObject_ScriptBackGround"), &XMVectorSet(-320.f, -318.f, 0.f, 1.f), (CGameObject**)&m_pBackGround_First), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Script"), Level::LEVEL_STAGE, TEXT("GameObject_ScriptBackGround"), &XMVectorSet(320.f, -318.f, 0.f, 1.f), (CGameObject**)&m_pBackGround_Sub), E_FAIL);

	return S_OK;
}

_tchar * CScript::Find_Script(_uint iIndex)
{
	switch (iIndex)
	{
	case 0:
		return TEXT("멈출...수가...없어!");
	case 1:
		return TEXT("무중력이야. 대단한걸!");
	case 2:
		return TEXT("감옥에서 탈출했군. 이 배신자들!");
	case 3:
		return TEXT("하지만 기억해라.");
	case 4:
		return TEXT("절대 살아서 나가진 못한다!");
	case 5:
		return TEXT("저 친구는 좀 진정해야겠는데.");
	case 6:
		return TEXT("녀석은 무시해! 포탈이 있어!");
	case 7:
		return TEXT("저걸 이용할 수 있을 거야.");
	case 8:
		return TEXT("이게 정말 좋은 생각이었을까아아아!");
	case 9:
		return TEXT("세상에... 이거 진짜 대단한데!");
	case 10:
		return TEXT("날 저 위로 올려줄 방법이 없을까?");
	case 11:
		return TEXT("하고 있어. 재촉하지 좀 마!");
	case 12:
		return TEXT("멋진데! 점점 요령이 생기는 것 같아!");
	case 13:
		return TEXT("떨어지지 마!");
	case 14:
		return TEXT("좋아, 이제 하나 남았어!");
	case 15:
		return TEXT("훌륭해!");
	case 16:
		return TEXT("아아, 압력이 어마어마한데!");
	case 17:
		return TEXT("휴! 엄청 강렬했어!");
	case 18:
		return TEXT("난 대충 할 생각 없어.");
	case 19:
		return TEXT("저런! 한 방 먹었네?");
	case 20:
		return TEXT("좋아!");
	case 21:
		return TEXT("하! 안 타게 조심해!");
	case 22:
		return TEXT("레이저는 내가 또 전문이지!");
	case 23:
		return TEXT("내 완승이네.");
	case 24:
		return TEXT("그래도 뭐 땀은 제대로 뺐으니까.");
	case 25:
		return TEXT("열심히 하면 이기는 건 쉽지.");
	case 26:
		return TEXT("이기진 못했지만 나도 그냥 지진 않았다고.");
	case 27:
		return TEXT("포탈이 또 있어!");
	case 28:
		return TEXT("좋아, 계속해 볼까.");
	case 29:
		return TEXT("으!");
	case 30:
		return TEXT("적당히라는 걸 모르는구나.");
	case 31:
		return TEXT("이 배신자들!");
	case 32:
		return TEXT("로즈의 수호자로서");
	case 33:
		return TEXT("절대 로즈를 울리게 할 수 없다!");
	case 34:
		return TEXT("각오해라, 사악한 녀석들.");
	case 35:
		return TEXT("우주 개코원숭이야, 나와서 우리 얘기 좀 들어줘!");
	case 36:
		return TEXT("우리가 산 물건들이 왜 우릴 죽이려 할까?");
	case 37:
		return TEXT("잠깐, 이런 얘긴 없었잖아!");
	case 38:
		return TEXT("날아가라!");
	case 39:
		return TEXT("코디! 장애물을 피해야지!");
	case 40:
		return TEXT("알아, 메이, 안다고! 이게 보이는 것처럼 쉽지가 않아!");
	case 41:
		return TEXT("오, 이번엔 누가 망쳤을까, 응?");
	case 42:
		return TEXT("그래, 이번엔 내 잘못이야!");
	case 43:
		return TEXT("조심해!");
	case 44:
		return TEXT("히히히히!");
	case 45:
		return TEXT("안 돼!");
	case 46:
		return TEXT("어떻게 여기까지 온 거지?!");
	case 47:
		return TEXT("제어실을 봉쇄해서");
	case 48:
		return TEXT("악당을 막아야겠어!");
	case 49:
		return TEXT("컴퓨터!");
	case 50:
		return TEXT("방어를 준비해라!");
	case 51:
		return TEXT("방어? 그게 무슨 말이야. '방어'라니?");
	case 52:
		return TEXT("음, 아마 레이저가 더 있을 거야.");
	case 53:
		return TEXT("우리라면 해결할 수 있어!");
	case 54:
		return TEXT("좋아, 내가 해낼 줄 알았다니까.");
	case 55:
		return TEXT("음, 저기요? 팀워크에 '나'라는 건 없어.");
	case 56:
		return TEXT("알았어, 하킴'박사'.");
	case 57:
		return TEXT("그래. 전류가 흐르지 않고 있어.");
	case 58:
		return TEXT("표적은 나무 인형!! 사격 개시!");
	case 59:
		return TEXT("조심해!");
	case 60:
		return TEXT("위협 수준 감마! 방어 시스템을 가동한다!");
	case 61:
		return TEXT("중력자 폭탄을 배치한다!");
	case 62:
		return TEXT("찰흙 인형은 어떻게 해줄까!? 타버려라!");
	case 63:
		return TEXT("정말 너무하네. 우주 원숭이! 내가 너한테 뭘 잘못했는데!?");
	case 64:
		return TEXT("도망쳐 봤자 소용없다!");
	case 65:
		return TEXT("위협 수준 베타! 배신자들을 처단한다!");
	case 66:
		return TEXT("울어도 소용없다. 패배자들!");
	case 67:
		return TEXT("이런 망할!");
	case 68:
		return TEXT("전원만 다시 연결하면 돼...");
	case 69:
		return TEXT("저 레이저를 없애야 해!");
	case 70:
		return TEXT("딱 기다리고 있어라 배신자들아!");
	case 71:
		return TEXT("전원만 연결하면,");
	case 72:
		return TEXT("너희를 없애고 로즈를 구하겠다!");
	case 73:
		return TEXT("워어, 무, 무슨 짓이야?! 내려놔... 살살!");
	case 74:
		return TEXT("당신이 도와줘야 해! 레이저를 잡아, 메이.");
	case 75:
		return TEXT("그래!");
	case 76:
		return TEXT("으윽! 그 밑에 있는 거 건들지 마!");
	case 77:
		return TEXT("반드시 갚아 주마!");
	case 78:
		return TEXT("로켓이라고?! 메이!");
	case 79:
		return TEXT("달려, 코디!");
	case 80:
		return TEXT("우주 개코원숭이, 이제 더는 못 참아!");
	case 81:
		return TEXT("내 로켓한테 인사해...");
	case 82:
		return TEXT("나한테 그거 겨누지 마!");
	case 83:
		return TEXT("도망칠 수 없다!");
	case 84:
		return TEXT("터져라!");
	case 85:
		return TEXT("난 로즈를 보호할 거다!");
	case 86:
		return TEXT("아예 가루로 만들어 주지!");
	case 87:
		return TEXT("이제 자신감이 좀 떨어졌나 봐?");
	case 88:
		return TEXT("코디, 이제 끝을 내자!");
	case 89:
		return TEXT("이런! 완벽한 내 기체가 손상됐어.");
	case 90:
		return TEXT("이제 장난은 끝이다!");
	case 91:
		return TEXT("컴퓨터!");
	case 92:
		return TEXT("컴퓨터, 모든 제한을 해제하라!");
	case 93:
		return TEXT("위협 수준 알파!");
	case 94:
		return TEXT("우주 개코원숭이, 잠깐!");
	case 95:
		return TEXT("나 기억 안 나?");
	case 96:
		return TEXT("메이야!");
	case 97:
		return TEXT("로즈는 내 가장 친한 친구다.");
	case 98:
		return TEXT("너희는 로즈를 울릴 작정이고!");
	case 99:
		return TEXT("찰흙 인형이 널 버렸다!");
	case 100:
		return TEXT("이거 열기가 점점 뜨거워지는데, 코디!");
	case 101:
		return TEXT("그래, 이 안에도 엄청 더워!");
	case 102:
		return TEXT("박살을 내주마!");
	case 103:
		return TEXT("조금만 기다려, 거의 다 됐어!");
	case 104:
		return TEXT("탈출 버튼이 있어.");
	case 105:
		return TEXT("사악한 자는 내 우주선을 몰 수 없다!");
	case 106:
		return TEXT("네 말은 듣지 않을 거다!");
	case 107:
		return TEXT("레이더에 녀석이 안 보여, 메이!");
	case 108:
		return TEXT("내가 찾아볼게!");
	case 109:
		return TEXT("저기");
	case 110:
		return TEXT("좀 더 살살 몰아줄 수는");
	case 111:
		return TEXT("없는 거야?!");
	case 112:
		return TEXT("왜, UFO 모는 실력이 마음에 안 들어?");
	case 113:
		return TEXT("아, 녀석을 놓쳤어. 더 가까이 가!");
	case 114:
		return TEXT("정통으로 맞았어!");
	case 115:
		return TEXT("근데 무슨 방어막이 있어.");
	case 116:
		return TEXT("몇 번 더 쏘면 될 거 같아!");
	case 117:
		return TEXT("멈춰! 난 로즈의 유일한 친구야. 로즈는 내가 필요해.");
	case 118:
		return TEXT("어떻게... 어떻게 내가 악에게 질 수가... 있지?");
	case 119:
		return TEXT("이번에는?");
	case 120:
		return TEXT("잘 좀 조준해 봐!");
	case 121:
		return TEXT("하하, 레이저 맞았네!");
	case 122:
		return TEXT("빗나갔어, 코디!");
	case 123:
		return TEXT("- 여긴 또 어디지");
	case 124:
		return TEXT("- 휴");
	case 125:
		return TEXT("- 대체");
	case 126:
		return TEXT("- 우주 개코원숭이");
	case 127:
		return TEXT("- 내 이름을 어떻게 알지, 배신자?");
	case 128:
		return TEXT("- 내가 우주 박물관에서 널 샀으니까.");
	case 129:
		return TEXT("- 거짓말!");
	case 130:
		return TEXT("- 우주 박물관에서 로즈가 직접 날 고르고");
	case 131:
		return TEXT("나한테 특별 임무를 내렸어.");
	case 132:
		return TEXT("우주의 어두운 이면으로부터");
	case 133:
		return TEXT("로즈를 보호할 수 있는 건 나뿐이야.");
	case 134:
		return TEXT("- 그게,우리도 로즈를 보호하는 입장이거든.");
	case 135:
		return TEXT("로즈의 부모로서 우린...");
	case 136:
		return TEXT("- 거짓말하지 마!");
	case 137:
		return TEXT("너희 임무는 로즈를 울리는 거잖아!");
	case 138:
		return TEXT("- 우린 눈물이 꼭 필요해.");
	case 139:
		return TEXT("눈물방울이 클수록 좋고.");
	case 140:
		return TEXT("- 대체 어떤 사악한 행성에서 온 거지?");
	case 141:
		return TEXT("아니,그런 게 아니야.");
	case 142:
		return TEXT("어, 뭔가 오해가 있는 것 같은데, 하하.");
	case 143:
		return TEXT("그게,우린 그냥 로즈의");
	case 144:
		return TEXT("눈물이 몇 방울 필요한...");
	case 145:
		return TEXT("- 너희가 로즈를 울리는 일은 없을 거야!");
	case 146:
		return TEXT("너희는 평생 여기 같일 테니까!");
	case 147:
		return TEXT("- 평생?!방금 저 녀석...");
	case 148:
		return TEXT("평생...이라고 했어?!");
	case 149:
		return TEXT("맙소사,정신 나갈 것 같아.");
	case 150:
		return TEXT("이제 어떻게 하지?");
	case 151:
		return TEXT("우주에서는 소리쳐봤자 아무도 없다고.");
	case 152:
		return TEXT("- 정말,그만 좀 해,코디!");
	case 153:
		return TEXT("멍청한 소리 하지마.여긴 우주가 아니야.");
	case 154:
		return TEXT("- 뭐?그럼 어딘데?");
	case 155:
		return TEXT("- 잘 들어.");
	case 156:
		return TEXT("로즈랑 우주 장난감을");
	case 157:
		return TEXT("가지고 엄청 많이 놀았어!");
	case 158:
		return TEXT("어디 있는지 찾기만 하면 돼.");
	case 159:
		return TEXT("오!저기 있다!");
	case 160:
		return TEXT("- 아!");
	case 161:
		return TEXT("- 코디.");
	case 162:
		return TEXT("이거 차 봐.");
	case 163:
		return TEXT("- 오우!");
	case 164:
		return TEXT("딱 맞네,헤헤.");
	case 165:
		return TEXT("- 우와,작동하네?");
	case 166:
		return TEXT("- 뭐 하는 거야?!오우!");
	case 167:
		return TEXT("우주 팽창.");
	case 168:
		return TEXT("빅뱅 이론 뒤의 팽창 이론이지!");
	case 169:
		return TEXT("- 그만해! 그거 얼른 꺼!");
	case 170:
		return TEXT("워어어어!");
	case 171:
		return TEXT("- 대단해!");
	case 172:
		return TEXT("- 메이,날 원래대로 돌려놔!");
	case 173:
		return TEXT("- 당신 여기서 나가고 싶지 않아?");
	case 174:
		return TEXT("그러려면 이 우주 팽창이랑...");
	case 175:
		return TEXT("우주 부츠가 필요해.");
	case 176:
		return TEXT("중력의 법칙을 거스르지.");
	case 177:
		return TEXT("후!하하!");
	case 178:
		return TEXT("- 왜 당신만 멋진 도구를 쓰는 거야?");
	case 179:
		return TEXT("- 내가 당신보다 멋지니까,코디.");
	case 180:
		return TEXT("- 당신보다 멋지니까,에베베.");
	case 181:
		return TEXT("- 워우.");
	case 182:
		return TEXT("- 우와.");
	case 183:
		return TEXT("- 그래,이건 좀 소름 끼치네.");
	case 184:
		return TEXT("- 아니야,봐.");
	case 185:
		return TEXT("로즈의 행복도를 측정하고 있어.");
	case 186:
		return TEXT("- 그래.그래도 엄청 소름 끼쳐.");
	case 187:
		return TEXT("- 잠깐,이건 뭐하는 거지?");
	case 188:
		return TEXT("- 뭐?아니야...");
	case 189:
		return TEXT("저건 말도 안 돼.");
	case 190:
		return TEXT("로즈는 하루에 적어도");
	case 191:
		return TEXT("몇 번은 웃는다고.");
	case 192:
		return TEXT("- 우리가 요즘 많이 다퉜잖아.");
	case 193:
		return TEXT("- 그래...그랬지,그래도...");
	case 194:
		return TEXT("그럼 로즈의 걱정은 어떨까?");
	case 195:
		return TEXT("- 뭐?아니야,저건 말도 안 돼.");
	case 196:
		return TEXT("이건 아니야, 미안한데");
	case 197:
		return TEXT("그 우주 원숭이 녀석이");
	case 198:
		return TEXT("뭔가 잘못 알고 있는 거라고,");
	case 199:
		return TEXT("알았지?");
	case 200:
		return TEXT("그냥 빨리 나가자.이 소름 끼치는 곳에서.");
	case 201:
		return TEXT("잠깐!메이,어쩌면 우리가");
	case 202:
		return TEXT("여기서 빠져나가서...");
	case 203:
		return TEXT("로즈한테 돌아갈 수 있을지도 몰라!");
	case 204:
		return TEXT("- 안 돼!위험해!");
	case 205:
		return TEXT("- 우후후!");
	case 206:
		return TEXT("예!");
	case 207:
		return TEXT("됐어!");
	case 208:
		return TEXT("- 느낌이 안 좋은데!");
	case 209:
		return TEXT("- 사악한 배신자들!");
	case 210:
		return TEXT("각오해라!");
	case 211:
		return TEXT("- 오,이런.");
	case 212:
		return TEXT("- 메이!");
	case 213:
		return TEXT("뭐가 어떻게 된 거야?");
	case 214:
		return TEXT("- 준비해! 난 녀석을 쫓을게!");
	case 215:
		return TEXT("- 메이!여기 뭔가 있어!");
	case 216:
		return TEXT("쓸 수 있을 것 같아.");
	case 217:
		return TEXT("- 아무거나 막 만지지 마!");
	case 218:
		return TEXT("- 날 믿어 봐.");
	case 219:
		return TEXT("녀석을 계속 쫓아!");
	case 220:
		return TEXT("- 우주 개코원숭이.");
	case 221:
		return TEXT("로즈한테 어떻게 돌아가지?");
	case 222:
		return TEXT("- 로즈를 해치게 놔둘 순 없어.");
	case 223:
		return TEXT("난 로즈의 가장 친한 친구야!");
	case 224:
		return TEXT("- 저기,로즈는 몇 년째 너랑 놀지도 않았어!");
	case 225:
		return TEXT("알았어?");
	case 226:
		return TEXT("넌 여태 로즈를 지켜봤잖아!");
	case 227:
		return TEXT("똑바로 보라고!");
	case 228:
		return TEXT("- 네가 로즈랑 다시 친해지게");
	case 229:
		return TEXT("도와줄 수 있을지도 몰라.");
	case 230:
		return TEXT("- 응?");
	case 231:
		return TEXT("정말이야?");
	case 232:
		return TEXT("- 그럼!");
	case 233:
		return TEXT("너랑 같이 놀라고 로즈한테 부탁해볼게.");
	case 234:
		return TEXT("하지만 우선 로즈한테 갈 수 있도록 도와줘.");
	case 235:
		return TEXT("- 흠,흠.");
	case 236:
		return TEXT("- 그래.");
	case 237:
		return TEXT("하지만 약속해야 해.");
	case 238:
		return TEXT("로즈를 울리지 않겠다고.");
	case 239:
		return TEXT("- 그래,그래,약속할게.");
	case 240:
		return TEXT("- 그래.");
	case 241:
		return TEXT("로즈한테 전해줘.");
	case 242:
		return TEXT("무슨 일이 있어도");
	case 243:
		return TEXT("난 로즈의 가장 친한 친구라고.");
	case 244:
		return TEXT("- 워어!");
	}
	return nullptr;
}

CScript * CScript::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScript* pInstance = new CScript(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CScript");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CScript::Clone_GameObject(void * pArg)
{
	CScript* pInstance = new CScript(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CScript");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CScript::Free()
{
	Safe_Release(m_pBackGround_First);
	Safe_Release(m_pBackGround_Sub);
	Safe_Release(m_pFontCom_Second);
	Safe_Release(m_pFontCom_First);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
