#pragma once
#include <WObject.h>
// �ִ� ���� ���� ��

class CProduct : public WObject
{
public:
	// ���� ���� �ε���
	int m_pastPk;
	// ���� ���� Ÿ��
	std::string m_pastType;
	// ���� ���� �ε���
	int m_curPk;
	// ���� ���� Ÿ��
	std::string m_curType;
	// ���׷����� ���� ��ȣ
	// ���׷����� ���� ��ȣ ��Ģ�� ���׷����� �ε��� * 1000 + ���׷����� ���� ī��Ʈ
	int m_genID;
	// ���׷����� �ε���
	int m_genIdx;
	// ���� �ð�
	double m_genTime;
	// ��� �ð�
	double m_passTime;


	CProduct(int genID, double genTime);
	CProduct(CProduct& copy);
};

