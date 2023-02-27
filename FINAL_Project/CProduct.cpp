#include "CProduct.h"

CProduct::CProduct(int genPk, double genTime)
{
	m_pastPk = -1;
	m_pastType = "NONE";
	m_curPk = -2;
	m_curType = "NONE";
	m_targetPk = -3;
	m_genPk = -1;
	m_genPk = genPk;
	m_genTime = genTime;
	m_passTime = -1;
}

/**
* OutputFn에서 생성해 msg 객체로 전달 된 WObject는 ExtTranFn 종료 후 엔진이 자체적으로 삭제한다.
* ExtTransFn에서 msg 객체로 전달 받은 객체를 다른 위치에서 사용하려면 Copy 생성자로 객체 복사 후
* 복사한 객체를 사용하도록 해야한다.
*/
CProduct::CProduct(CProduct& copy)
{
	m_pastPk = copy.m_pastPk;
	m_pastType = copy.m_pastType;
	m_curPk = copy.m_curPk;
	m_curType = copy.m_curType;
	m_targetPk = copy.m_targetPk;
	m_genPk = copy.m_genPk;
	m_genPk = copy.m_genPk;
	m_genTime = copy.m_genTime;
	m_passTime = copy.m_passTime;
}