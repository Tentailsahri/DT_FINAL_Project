#pragma once
#include <WCoupModel.h>
#include "CProduct.h"
#include "Atomic_Receive.h"
#include "Atomic_State.h"
#include "Atomic_Send.h"

class Cpd_PROC : public WCoupModel	//Ŀ�ø�
{
public:
	Cpd_PROC(int proc_idx, int proc_subidx, int pk);
	// �Է���Ʈ ����
	enum class IN_PORT : unsigned int {
		PAUSE = 1200,
		READY,
		PRODUCT
	};

	// �����Ʈ ����
	enum class OUT_PORT : unsigned int {
		PAUSE = 1210,
		READY,
		PRODUCT
	};

	// �ӽ� ����
	std::queue<CProduct*> m_buffer;
};
