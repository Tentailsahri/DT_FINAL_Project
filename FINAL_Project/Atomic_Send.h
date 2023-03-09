#pragma once
#include <WAtomModel.h>
#include "CProduct.h"
#include "GlobalVar.h"

/**
* 랜덤한 주기로 차량을 생성해서 RoadQueue로 차량을 보내는 모델
*/

class Atomic_Send : public WAtomModel	// DEVS 원자모델
{
public:
	Atomic_Send(int type, int idx, int pk);
	// 입력포트 정의
	enum class IN_PORT : unsigned int {
		PAUSE = 300,
		READY,
		MAKE,
		ERROR_ON,
		ERROR_OFF,
		RECEIVE
	};

	// 출력포트 정의
	enum class OUT_PORT : unsigned int {
		PRODUCT = 310,
	
	};

	// DEVS 함수 재정의
	bool ExtTransFn(const WMessage& msg);
	bool IntTransFn();
	bool OutputFn(WMessage& msg);
	WTime TimeAdvanceFn();

	// 모델 상태 정의
	enum class STATE : unsigned int {
		WAIT = 0,
		SEND,
		PENDING,
		PAUSE,
		SERROR
	};

	// 모델 TA 정의
	double TA_STATE_GEN[5] = { WTime::infinity_, GLOBAL_VAR->TA_SEND_SEND_TIME[0], 0, WTime::infinity_, WTime::infinity_ };
	double TA_STATE_TRACK[4] = { WTime::infinity_, GLOBAL_VAR->TA_SEND_SEND_TIME[1], 0, WTime::infinity_};
	double TA_STATE_PROC[5] = { WTime::infinity_, GLOBAL_VAR->TA_SEND_SEND_TIME[2], 0, WTime::infinity_ , WTime::infinity_};
	double TA_STATE_STOCK[5] = { WTime::infinity_, GLOBAL_VAR->TA_SEND_SEND_TIME[3], 0, 0 , WTime::infinity_ };

	// 현재 모델 상태 변수
	STATE m_modelState;
	
	// 설비 타입
	int m_type;
	// 설비 인덱스
	int m_idx;
	// pk
	int m_pk;
	int newgencount;
	int newgencount1;

	static const char* getModel2Str(int model_type);
	void m_sendGenQuery(CProduct* product);
	void m_sendPassQuery(CProduct* product);
};

