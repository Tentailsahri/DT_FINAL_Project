#pragma once
#include <WAtomModel.h>
#include "CProduct.h"
#include "GlobalVar.h"

/**
* 랜덤한 주기로 차량을 생성해서 RoadQueue로 차량을 보내는 모델
*/

class Atomic_Receive : public WAtomModel	// DEVS 원자모델
{
public:
	Atomic_Receive(int type, int idx, int pk);
	
	// 입력포트 정의
	enum class IN_PORT : unsigned int {
		PAUSE = 100,
		READY,
		PRODUCT,
		SEND
	};

	// 출력포트 정의
	enum class OUT_PORT : unsigned int {
		PAUSE = 110,
		READY
	};

	// DEVS 함수 재정의
	bool ExtTransFn(const WMessage& msg);
	bool IntTransFn();
	bool OutputFn(WMessage& msg);
	WTime TimeAdvanceFn();

	// 모델 상태 정의
	enum class STATE : unsigned int {
		RECEIVE = 0,
		DECISION,
		FULL,
		INIT,
		READYMAP
	};

	// 모델 TA 정의
	double TA_STATE[5] = { WTime::infinity_, 0, WTime::infinity_, GLOBAL_VAR->TA_STATE_INIT[2], 0};

	// 현재 모델 상태 변수
	STATE m_modelState;

	// 설비 타입
	int m_type;
	// 설비 인덱스
	int m_idx;
	// pk
	int m_pk;
};

