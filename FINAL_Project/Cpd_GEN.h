#pragma once
#include <WCoupModel.h>
#include "CProduct.h"
#include "Atomic_State.h"
#include "Atomic_Send.h"

class Cpd_GEN : public WCoupModel	//커플모델
{
public:
	Cpd_GEN(int gen_idx, int pk);
	// 입력포트 정의
	enum class IN_PORT : unsigned int {
		PAUSE = 1000,
		READY
	};

	// 출력포트 정의
	enum class OUT_PORT : unsigned int {
		PRODUCT = 1010
	};

	// 임시 버퍼
	std::queue<CProduct*> m_buffer;

};
