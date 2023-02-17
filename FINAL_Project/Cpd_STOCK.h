#pragma once
#include <WCoupModel.h>
#include "CProduct.h"
#include "Atomic_Receive.h"
#include "Atomic_State.h"

class Cpd_STOCK : public WCoupModel	//Ŀ�ø�
{
public:
	Cpd_STOCK(int stock_idx, int pk);
	// �Է���Ʈ ����
	enum class IN_PORT : unsigned int {
		PAUSE = 1300,
		READY,
		PRODUCT
	};

	// �����Ʈ ����
	enum class OUT_PORT : unsigned int {
		PAUSE = 1310,
		READY,
		PRODUCT
	};

	// �ӽ� ����
	std::queue<CProduct*> m_buffer;
};
