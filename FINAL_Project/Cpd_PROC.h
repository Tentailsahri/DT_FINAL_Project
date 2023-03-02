#pragma once
#include <WCoupModel.h>
#include "CProduct.h"
#include "Atomic_Receive.h"
#include "Atomic_State.h"
#include "Atomic_Send.h"

class Cpd_PROC : public WCoupModel	//커플모델
{
public:
	Cpd_PROC(int proc_idx, int proc_subidx, int pk);
	// 입력포트 정의
	enum class IN_PORT : unsigned int {
		PAUSE = 1200,
		READY,
		PRODUCT
	};

	// 출력포트 정의
	enum class OUT_PORT : unsigned int {
		PAUSE = 1210,
		READY,
		PRODUCT
	};

	// 임시 버퍼
	std::queue<CProduct*> m_buffer;
};
