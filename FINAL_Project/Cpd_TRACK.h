#pragma once
#include <WCoupModel.h>
#include "CProduct.h"
#include "Atomic_Receive.h"
#include "Atomic_State.h"
#include "Atomic_Send.h"

class Cpd_TRACK : public WCoupModel	//Ŀ�ø�
{
public:
	Cpd_TRACK(int track_idx, int pk);
	// �Է���Ʈ ����
	enum class IN_PORT : unsigned int {
		PAUSE = 1100,
		READY,
		PRODUCT
	};

	// �����Ʈ ����
	enum class OUT_PORT : unsigned int {
		PAUSE = 1110,
		READY,
		PRODUCT
	};

	// �ӽ� ����
	std::queue<CProduct*> m_buffer;
};
