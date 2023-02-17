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
* OutputFn���� ������ msg ��ü�� ���� �� WObject�� ExtTranFn ���� �� ������ ��ü������ �����Ѵ�.
* ExtTransFn���� msg ��ü�� ���� ���� ��ü�� �ٸ� ��ġ���� ����Ϸ��� Copy �����ڷ� ��ü ���� ��
* ������ ��ü�� ����ϵ��� �ؾ��Ѵ�.
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