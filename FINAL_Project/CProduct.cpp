#include "CProduct.h"

CProduct::CProduct(int genID, double genTime)
{
	m_pastpk = -1;
	m_pastType = "NONE";
	m_curpk = -2;;
	m_curType = "NONE";
	m_genID = genID;
	m_genIdx = -1;
	m_genTime = genTime;
	m_passTime = -1;
}

/**
* OutputFn���� ������ msg ��ü�� ���� �� WObject�� ExtTranFn ���� �� ������ ��ü������ �����Ѵ�.
* ExtTransFn���� msg ��ü�� ���� ���� ��ü�� �ٸ� ��ġ���� ����Ϸ��� Copy �����ڷ� ��ü ���� ��
* ������ ��ü�� ����ϵ��� �ؾ��Ѵ�.
*/
CProduct::CProduct(CProduct& copy)
{
	m_pastpk = copy.m_pastpk;
	m_pastType = copy.m_pastType;
	m_curpk = copy.m_curpk;
	m_curType = copy.m_curType;
	m_genID = copy.m_genID;
	m_genIdx = copy.m_genIdx;
	m_genTime = copy.m_genTime;
	m_passTime = copy.m_passTime;
}