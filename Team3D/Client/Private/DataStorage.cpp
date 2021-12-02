#include "stdafx.h"
#include "..\public\DataStorage.h"
#include "Cody.h"
#include "May.h"
#include "MainCamera.h"
#include "SubCamera.h"

IMPLEMENT_SINGLETON(CDataStorage)

_uint CDataStorage::Get_ValveCount_Cody(_bool IsCody)
{
	if (true == IsCody)
		return m_iCody_Valve_Count;
	else
		return m_iMay_Valve_Count;
}

void CDataStorage::Free()
{
}
