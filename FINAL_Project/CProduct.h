#pragma once
#include <WObject.h>
// �ִ� ���� ���� ��

class CProduct : public WObject
{
public:
	// ���� ���� PK
	int m_pastPk;
	// ���� ���� Ÿ��
	std::string m_pastType;
	// ���� ���� PK
	int m_curPk;
	// ���� ���� Ÿ��
	std::string m_curType;
	// Ÿ�� ���� PK
	int m_targetPk;
	// ���׷����� PK
	int m_genPk;
	// ���׷����� ���� ��ȣ
	// ���׷����� ���� ��ȣ ��Ģ�� ���׷����� �ε��� * 1000 + ���׷����� ���� ī��Ʈ
	int m_genPk;
	// ���� �ð�
	double m_genTime;
	// ��� �ð�
	double m_passTime;


	CProduct(int genPk, double genTime);
	CProduct(CProduct& copy);
};

