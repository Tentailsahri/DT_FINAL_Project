#pragma once
#include <WCoupModel.h>
#include "CProduct.h"
#include "Atomic_Receive.h"
#include "Atomic_State.h"

class Cpd_STOCK : public WCoupModel	//커플모델
{
public:
	Cpd_STOCK(int stock_idx, int pk);
	// 입력포트 정의
	enum class IN_PORT : unsigned int {
		PAUSE = 1300,
		READY,
		PRODUCT
	};

	// 출력포트 정의
	enum class OUT_PORT : unsigned int {
		PAUSE = 1310,
		READY,
		PRODUCT
	};

	// 임시 버퍼
	std::queue<CProduct*> m_buffer;
};
