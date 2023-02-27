#include "Atomic_Receive.h"
#include <SimLogger.h>

Atomic_Receive::Atomic_Receive(int type, int idx, int pk) {
	// 모델 이름 설정
	SetName("Atomic_Receive");
	// 입,출력 포트 설정
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	switch (type) {
	case 0:
		AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
		AddInPort((unsigned int)IN_PORT::SEND, "SEND");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
		AddOutPort((unsigned int)OUT_PORT::READY, "READY");
		break;
	case 1:
		AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
		AddInPort((unsigned int)IN_PORT::SEND, "SEND");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
		AddOutPort((unsigned int)OUT_PORT::READY, "READY");
		break;
	case 2:
		AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
		AddInPort((unsigned int)IN_PORT::SEND, "SEND");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
		AddOutPort((unsigned int)OUT_PORT::READY, "READY");
		break;
	case 3:
		AddInPort((unsigned int)IN_PORT::SEND, "SEND");
		AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
		AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
		AddOutPort((unsigned int)OUT_PORT::READY, "READY");
		break;
	}

	// 초기 모델 상태 설정
	
	m_modelState = STATE::INIT;
	product = nullptr;
	// 모델 변수 초기화
	m_type = type;
	m_idx = idx;
	m_pk = pk;
	GLOBAL_VAR->ReadyMapPush(m_pk, false);
}

// 외부 상태 천이 함수
bool Atomic_Receive::ExtTransFn(const WMessage& msg) {
	if (msg.GetPort() == (unsigned int)IN_PORT::PRODUCT) {
		if (m_modelState == STATE::RECEIVE) {
			if (m_type != 0) {
				CProduct* cproduct = (CProduct*)msg.GetValue();
				product = new CProduct(*cproduct);
				product->m_passTime = WAISER->CurentSimulationTime().GetValue();
				product->m_pastPk = product->m_curPk;
				product->m_pastType = product->m_curType;
				product->m_curPk = m_pk;
				product->m_curType = getModel2Str(m_type);
				CLOG->info("PK: {}, idx : {} {} {}번 제품 수신 완료, at t = {}", m_pk, m_idx, product->m_curType, product->m_genID, WAISER->CurentSimulationTime().GetValue());
				CLOG->info("pastPk={} pastType={} curPk={} curtype={}", product->m_pastPk, product->m_pastType, product->m_curPk, product->m_curType);
				GLOBAL_VAR->mBufferPush(0, m_pk, product, &GLOBAL_VAR->p_buffer);
			}
			m_modelState = STATE::DECISION;
		}
	} else if (msg.GetPort() == (unsigned int)IN_PORT::SEND) {
			if (m_modelState == STATE::FULL) {
				m_modelState = STATE::DECISION;
			}
		} 
		else if (m_type!=3 && msg.GetPort() == (unsigned int)IN_PORT::READY) {
		GLOBAL_VAR->ReadyMapPush(m_pk, true);
		m_modelState = STATE::READYMAP;
		}
		else if (m_type != 3 && msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
		GLOBAL_VAR->ReadyMapPush(m_pk, false);
		m_modelState = STATE::READYMAP;
	}
	return true;
}

// 내부 상태 천이 함수
bool Atomic_Receive::IntTransFn() {
	if (m_modelState == STATE::READYMAP) {
		if (GLOBAL_VAR->m_maxbuffer_Generator > GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
			m_modelState = STATE::RECEIVE;
		}
		else if (GLOBAL_VAR->m_maxbuffer_Generator <= GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
			m_modelState = STATE::FULL;
		}
	}
	return true;
}

// 출력 함수
bool Atomic_Receive::OutputFn(WMessage& msg) {
	if (m_modelState == STATE::INIT) {
		msg.SetPortValue((unsigned int)OUT_PORT::READY, product);
		m_modelState = STATE::RECEIVE;
	}
	switch (m_type) {
	case 0:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} GEN Buffer size {}", m_pk, m_idx, GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer));
			if (GLOBAL_VAR->m_maxbuffer_Generator <= GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, nullptr);
				m_modelState = STATE::FULL;
			}
			else if (GLOBAL_VAR->m_maxbuffer_Generator > GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::READY, nullptr);
				m_modelState = STATE::RECEIVE;
			}
		}
		break;
	case 1:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} TRACK Buffer size {}", m_pk, m_idx, GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer));
			if (GLOBAL_VAR->m_maxbuffer_Receive > GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::READY, product);
				m_modelState = STATE::RECEIVE;
			} else if(GLOBAL_VAR->m_maxbuffer_Receive <= GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)){
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, product);
				m_modelState = STATE::FULL;
			}
		}
		break;
	case 2:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} PROC Buffer size {}", m_pk, m_idx, GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer));
			if (GLOBAL_VAR->m_maxbuffer_Process > GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::READY, product);
				
				m_modelState = STATE::RECEIVE;
			} else if (GLOBAL_VAR->m_maxbuffer_Process <= GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, product);
				m_modelState = STATE::FULL;
			}
		}
		break;
	case 3:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} STOCK Buffer size {}", m_pk, m_idx, GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer));
			if (GLOBAL_VAR->m_maxbuffer_Stock > GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::READY, product);
				
				m_modelState = STATE::RECEIVE;
			} else if (GLOBAL_VAR->m_maxbuffer_Stock <= GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, product);
				m_modelState = STATE::FULL;
			}
		}
		break;
	}

	return true;
}

// TA함수
WTime Atomic_Receive::TimeAdvanceFn() {
	return TA_STATE[(int)m_modelState];
}

const char* Atomic_Receive::getModel2Str(int m_type)
{
	switch (m_type) {
	case 0:
		return "GEN";
	case 1:
		return "TRACK";
	case 2:
		return "PROC";
	case 3:
		return "STOCK";
	}
}