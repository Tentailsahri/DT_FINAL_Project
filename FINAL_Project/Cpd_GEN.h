#pragma once
#include <WCoupModel.h>
#include "CProduct.h"
#include "Atomic_State.h"
#include "Atomic_Send.h"

class Cpd_GEN : public WCoupModel	//Ŀ�ø�
{
public:
	Cpd_GEN(int gen_idx, int pk);
	// �Է���Ʈ ����
	enum class IN_PORT : unsigned int {
		PAUSE = 1000,
		READY
	};

	// �����Ʈ ����
	enum class OUT_PORT : unsigned int {
		PRODUCT = 1010
	};

	// �ӽ� ����
	std::queue<CProduct*> m_buffer;

};
