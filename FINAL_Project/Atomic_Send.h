#pragma once
#include <WAtomModel.h>
#include "CProduct.h"
#include "GlobalVar.h"

/**
* ������ �ֱ�� ������ �����ؼ� RoadQueue�� ������ ������ ��
*/

class Atomic_Send : public WAtomModel	// DEVS ���ڸ�
{
public:
	Atomic_Send(int type, int idx, int pk);
	// �Է���Ʈ ����
	enum class IN_PORT : unsigned int {
		PAUSE = 300,
		READY,
		MAKE,
		ERROR_ON,
		ERROR_OFF,
		RECEIVE
	};

	// �����Ʈ ����
	enum class OUT_PORT : unsigned int {
		PRODUCT = 310,
	
	};

	// DEVS �Լ� ������
	bool ExtTransFn(const WMessage& msg);
	bool IntTransFn();
	bool OutputFn(WMessage& msg);
	WTime TimeAdvanceFn();

	// �� ���� ����
	enum class STATE : unsigned int {
		WAIT = 0,
		SEND,
		PENDING,
		PAUSE,
		SERROR
	};

	// �� TA ����
	double TA_STATE_GEN[5] = { WTime::infinity_, GLOBAL_VAR->TA_SEND_SEND_TIME[0], 0, WTime::infinity_, WTime::infinity_ };
	double TA_STATE_TRACK[4] = { WTime::infinity_, GLOBAL_VAR->TA_SEND_SEND_TIME[1], 0, WTime::infinity_};
	double TA_STATE_PROC[5] = { WTime::infinity_, GLOBAL_VAR->TA_SEND_SEND_TIME[2], 0, WTime::infinity_ , WTime::infinity_};
	double TA_STATE_STOCK[5] = { WTime::infinity_, GLOBAL_VAR->TA_SEND_SEND_TIME[3], 0, 0 , WTime::infinity_ };

	// ���� �� ���� ����
	STATE m_modelState;
	
	// ���� Ÿ��
	int m_type;
	// ���� �ε���
	int m_idx;
	// pk
	int m_pk;
	int newgencount;
	int newgencount1;

	static const char* getModel2Str(int model_type);
	void m_sendGenQuery(CProduct* product);
	void m_sendPassQuery(CProduct* product);
};

