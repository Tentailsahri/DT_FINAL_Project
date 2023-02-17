#include "CProduct.h"

CProduct::CProduct(int genID, double genTime)
{
	m_pastIdx = -1;
	m_pastType = "NONE";
	m_curIdx = -2;;
	m_curType = "NONE";
	m_genID = genID;
	m_genIdx = -1;
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
	m_pastIdx = copy.m_pastIdx;
	m_pastType = copy.m_pastType;
	m_curIdx = copy.m_curIdx;
	m_curType = copy.m_curType;
	m_genID = copy.m_genID;
	m_genIdx = copy.m_genIdx;
	m_genTime = copy.m_genTime;
	m_passTime = copy.m_passTime;
}