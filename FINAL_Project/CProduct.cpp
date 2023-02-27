#include "CProduct.h"

CProduct::CProduct(int genID, double genTime)
{
	m_pastPk = -1;
	m_pastType = "NONE";
	m_curPk = -2;
	m_curType = "NONE";
	m_genID = genID;
	m_genPk = -1;
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
	m_pastPk = copy.m_pastPk;
	m_pastType = copy.m_pastType;
	m_curPk = copy.m_curPk;
	m_curType = copy.m_curType;
	m_genID = copy.m_genID;
	m_genPk = copy.m_genPk;
	m_genTime = copy.m_genTime;
	m_passTime = copy.m_passTime;
}