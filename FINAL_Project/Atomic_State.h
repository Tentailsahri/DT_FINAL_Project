#pragma once
#include <WAtomModel.h>
#include "CProduct.h"
#include "GlobalVar.h"
#include "PostgreSQLConnector.h"
/**
* 랜덤한 주기로 차량을 생성해서 RoadQueue로 차량을 보내는 모델
*/

class Atomic_State : public WAtomModel	// DEVS 원자모델
{
public:
	Atomic_State(int type, int idx, int pk);
	~Atomic_State();
	// 입력포트 정의
	enum class IN_PORT : unsigned int {
		PAUSE = 200,
		READY
	};

	// 출력포트 정의
	enum class OUT_PORT : unsigned int {
		READY = 210,
		PAUSE,
		ERROR_ON,
		ERROR_OFF,
		MAKE,
		SEND
	};

	// DEVS 함수 재정의
	bool ExtTransFn(const WMessage& msg);
	bool IntTransFn();
	bool OutputFn(WMessage& msg);
	WTime TimeAdvanceFn();

	// 모델 상태 정의
	enum class STATE : unsigned int {
		INIT = 0,
		ACTIVE,
		SERROR,
		PAUSE
	};

	// 모델 TA 정의
	double TA_STATE_GEN[4] = { 5, 5, 5, WTime::infinity_ };
	double TA_STATE_TRACK[4] = { 5, WTime::infinity_, 0, WTime::infinity_ };
	double TA_STATE_PROC[4] = { 5, 5, 5, WTime::infinity_ };
	double TA_STATE_STOCK[4] = { 5, 5, 5, 0 };

	// 현재 모델 상태 변수
	STATE m_modelState;

	// 설비 타입
	int m_type;
	// 설비 인덱스
	int m_idx;
	// 작동 횟수 카운트
	int m_count;
	// 제품 투입설비 생산 카운트;
	int m_genCount;
	// pk
	int m_pk;

	// 현재 상태를 문자열로 변환
	static const char* getState2Str(Atomic_State::STATE type);

	// STATE 시작 시간 저장용
	double m_current_time;

	double timeStore[4]={0, 0, 0, 0};
	void m_dataUpdate();
	
};

