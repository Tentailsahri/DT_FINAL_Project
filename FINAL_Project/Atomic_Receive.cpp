#include "Atomic_Receive.h"
#include <SimLogger.h>

Atomic_Receive::Atomic_Receive(int type, int idx, int pk) {
	// 모델 이름 설정
	SetName("Atomic_Receive");
	// 입,출력 포트 설정
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	switch (type) {
	case 1:
		AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
		AddInPort((unsigned int)IN_PORT::SEND, "SEND");
		AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
		AddOutPort((unsigned int)OUT_PORT::READY, "READY");
		break;
	case 2:
		AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
		AddInPort((unsigned int)IN_PORT::SEND, "SEND");
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
	m_modelState = STATE::RECEIVE;
	// 모델 변수 초기화
	m_type = type;
	m_idx = idx;
	m_pk = pk;
	
}

// 외부 상태 천이 함수
bool Atomic_Receive::ExtTransFn(const WMessage& msg) {
	switch (m_type) {
	case 1:
		if (msg.GetPort() == (unsigned int)IN_PORT::PRODUCT) {
			if (m_modelState == STATE::RECEIVE) {
				CProduct* cproduct = (CProduct*)msg.GetValue();
				CProduct* product = new CProduct(*cproduct);
				product->m_passTime = WAISER->CurentSimulationTime().GetValue();
				product->m_pastpk = product->m_curpk;
				product->m_pastType = product->m_curType;
				product->m_curpk = m_pk;
				product->m_curType = "TRACK";
				CLOG->info("PK: {}, idx : {} TRACK {}번 제품 수신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
				CLOG->info("pastpk={} pastType={} curpk={} curtype={}", product->m_pastpk, product->m_pastType, product->m_curpk, product->m_curType);
				GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->buffer);
				
				m_modelState = STATE::DECISION;
			}
		}  else if (msg.GetPort() == (unsigned int)IN_PORT::SEND) {
			if (m_modelState == STATE::FULL) {
				m_modelState = STATE::DECISION;
			}
		} 
		break;
	case 2:
		if (msg.GetPort() == (unsigned int)IN_PORT::PRODUCT) {
			if (m_modelState == STATE::RECEIVE) {
				CProduct* cproduct = (CProduct*)msg.GetValue();
				CProduct* product = new CProduct(*cproduct);
				product->m_pastpk = product->m_curpk;
				product->m_pastType = product->m_curType;
				product->m_curpk = m_pk;
				product->m_curType = "PROC";
				CLOG->info("pastpk={} pastType={} curpk={} curtype={}", product->m_pastpk, product->m_pastType, product->m_curpk, product->m_curType);
				GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->buffer);
				CLOG->info("PK: {}, idx : {} PROC {}번 제품 수신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());

				m_modelState = STATE::DECISION;
			}
		} else if (msg.GetPort() == (unsigned int)IN_PORT::SEND) {
			if (m_modelState == STATE::FULL) {
				m_modelState = STATE::DECISION;
			}
		} 
		else Continue();
		break;
	case 3:
		if (msg.GetPort() == (unsigned int)IN_PORT::PRODUCT) {
			if (m_modelState == STATE::RECEIVE) {
				CProduct* cproduct = (CProduct*)msg.GetValue();
				CProduct* product = new CProduct(*cproduct);
				product->m_pastpk = product->m_curpk;
				product->m_pastType = product->m_curType;
				product->m_curpk = m_pk;
				product->m_curType = "STOCK";
				CLOG->info("pastpk={} pastType={} curpk={} curtype={}", product->m_pastpk, product->m_pastType, product->m_curpk, product->m_curType);
				GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->buffer);
				CLOG->info("PK: {}, idx : {} STOCK {}번 제품 수신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
				m_modelState = STATE::DECISION;
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::SEND) {
			if (m_modelState == STATE::FULL) {
				m_modelState = STATE::DECISION;
			}
		}
		
		break;
	}

	return true;
}

// 내부 상태 천이 함수
bool Atomic_Receive::IntTransFn() {
	return true;
}

// 출력 함수
bool Atomic_Receive::OutputFn(WMessage& msg) {
	switch (m_type) {
	case 1:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} TRACK Buffer size {}", m_pk, m_idx, GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer));
			if (GLOBAL_VAR->m_maxbuffer_Receive > GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::READY, nullptr);
				m_modelState = STATE::RECEIVE;
			} else if((GLOBAL_VAR->m_maxbuffer_Receive <= GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer))){
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, nullptr);
				m_modelState = STATE::FULL;
			}
		}
		break;
	case 2:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} PROC Buffer size {}", m_pk, m_idx, GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer));
			if (GLOBAL_VAR->m_maxbuffer_Process > GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::READY, nullptr);
				
				m_modelState = STATE::RECEIVE;
			} else if ((GLOBAL_VAR->m_maxbuffer_Receive <= GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer))) {
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, nullptr);
				m_modelState = STATE::FULL;
			}
		}
		break;
	case 3:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} STOCK Buffer size {}", m_pk, m_idx, GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer));
			if (GLOBAL_VAR->m_maxbuffer_Stock > GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::READY, nullptr);
				
				m_modelState = STATE::RECEIVE;
			} else if ((GLOBAL_VAR->m_maxbuffer_Receive <= GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer))) {
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, nullptr);
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
