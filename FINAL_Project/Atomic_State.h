#pragma once
#include <WAtomModel.h>
#include "CProduct.h"
#include "GlobalVar.h"
#include "PostgreSQLConnector.h"
/**
* ������ �ֱ�� ������ �����ؼ� RoadQueue�� ������ ������ ��
*/

class Atomic_State : public WAtomModel	// DEVS ���ڸ�
{
public:
	Atomic_State(int type, int idx, int pk);
	~Atomic_State();
	// �Է���Ʈ ����
	enum class IN_PORT : unsigned int {
		PAUSE = 200,
		READY
	};

	// �����Ʈ ����
	enum class OUT_PORT : unsigned int {
		READY = 210,
		PAUSE,
		ERROR_ON,
		ERROR_OFF,
		MAKE,
		SEND
	};

	// DEVS �Լ� ������
	bool ExtTransFn(const WMessage& msg);
	bool IntTransFn();
	bool OutputFn(WMessage& msg);
	WTime TimeAdvanceFn();

	// �� ���� ����
	enum class STATE : unsigned int {
		INIT = 0,
		ACTIVE,
		SERROR,
		PAUSE
	};

	// �� TA ����
	double TA_STATE_GEN[4] = { 5, 5, 5, WTime::infinity_ };
	double TA_STATE_TRACK[4] = { 5, WTime::infinity_, 0, WTime::infinity_ };
	double TA_STATE_PROC[4] = { 5, 5, 5, WTime::infinity_ };
	double TA_STATE_STOCK[4] = { 5, 5, 5, 0 };

	// ���� �� ���� ����
	STATE m_modelState;

	// ���� Ÿ��
	int m_type;
	// ���� �ε���
	int m_idx;
	// �۵� Ƚ�� ī��Ʈ
	int m_count;
	// ��ǰ ���Լ��� ���� ī��Ʈ;
	int m_genCount;
	// pk
	int m_pk;

	// ���� ���¸� ���ڿ��� ��ȯ
	static const char* getState2Str(Atomic_State::STATE type);

	// STATE ���� �ð� �����
	double m_current_time;

	double timeStore[4]={0, 0, 0, 0};
	void m_dataUpdate();
	
};

