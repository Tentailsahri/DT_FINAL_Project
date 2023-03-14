#pragma once
#include <WAtomModel.h>
#include "CProduct.h"
#include "GlobalVar.h"

/**
* ������ �ֱ�� ������ �����ؼ� RoadQueue�� ������ ������ ��
*/

class Atomic_Receive : public WAtomModel	// DEVS ���ڸ�
{
public:
	Atomic_Receive(int type, int idx, int pk);
	
	// �Է���Ʈ ����
	enum class IN_PORT : unsigned int {
		PAUSE = 100,
		READY,
		PRODUCT,
		SEND
	};

	// �����Ʈ ����
	enum class OUT_PORT : unsigned int {
		PAUSE = 110,
		READY
	};

	// DEVS �Լ� ������
	bool ExtTransFn(const WMessage& msg);
	bool IntTransFn();
	bool OutputFn(WMessage& msg);
	WTime TimeAdvanceFn();

	// �� ���� ����
	enum class STATE : unsigned int {
		RECEIVE = 0,
		DECISION,
		FULL,
		INIT,
		READYMAP
	};

	// �� TA ����
	double TA_STATE_GEN[5] = { WTime::infinity_, 0, WTime::infinity_, GLOBAL_VAR->TA_STATE_INIT[0], 0 };
	double TA_STATE_TRACK[5] = { WTime::infinity_, 0, WTime::infinity_, GLOBAL_VAR->TA_STATE_INIT[1], 0 };
	double TA_STATE_PROC[5] = { WTime::infinity_, 0, WTime::infinity_, GLOBAL_VAR->TA_STATE_INIT[2], 0 };
	double TA_STATE_STOCK[5] = { WTime::infinity_, 0, WTime::infinity_, GLOBAL_VAR->TA_STATE_INIT[3], 0 };

	// ���� �� ���� ����
	STATE m_modelState;
	CProduct* m_product;
	// ���� Ÿ��
	int m_type;
	// ���� �ε���
	int m_idx;
	// ������ pk
	int m_pk;
	//������ subidx
	int m_subIdx;
	int bufferPop[100]={0};
	int bufferPopNum;
	int sendreadymapNum = 0;
	// ���� ���¸� ���ڿ��� ��ȯ
	static const char* getModel2Str(int model_type);
};

