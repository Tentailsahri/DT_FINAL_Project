#pragma once
#include <WCoupModel.h>
#include "CProduct.h"
#include "Atomic_Receive.h"
#include "Atomic_State.h"
#include "Atomic_Send.h"

class Cpd_TRACK : public WCoupModel	//커플모델
{
public:
	Cpd_TRACK(int track_idx, int pk);
	// 입력포트 정의
	enum class IN_PORT : unsigned int {
		PAUSE = 1100,
		READY,
		PRODUCT
	};

	// 출력포트 정의
	enum class OUT_PORT : unsigned int {
		PAUSE = 1110,
		READY,
		PRODUCT
	};

	// 임시 버퍼
	std::queue<CProduct*> m_buffer;
};
