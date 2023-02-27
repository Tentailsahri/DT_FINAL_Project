#include "Atomic_Send.h"
#include <SimLogger.h>

Atomic_Send::Atomic_Send(int type, int idx, int pk) {
	// 모델 이름 설정
	SetName("Atomic_Send");
	// 입,출력 포트 설정
	switch (type) {
	case 0:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::MAKE, "MAKE");
		AddInPort((unsigned int)IN_PORT::ERROR_ON, "ERROR_ON");
		AddInPort((unsigned int)IN_PORT::ERROR_OFF, "ERROR_OFF");
		AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");
		break;
	case 1:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::RECEIVE, "RECEIVE");
		AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");
		break;
	case 2:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::RECEIVE, "RECEIVE");
		AddInPort((unsigned int)IN_PORT::ERROR_ON, "ERROR_ON");
		AddInPort((unsigned int)IN_PORT::ERROR_OFF, "ERROR_OFF");
		AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");
		break;
	case 3:
		AddInPort((unsigned int)IN_PORT::RECEIVE, "RECEIVE");
		AddInPort((unsigned int)IN_PORT::ERROR_ON, "ERROR_ON");
		AddInPort((unsigned int)IN_PORT::ERROR_OFF, "ERROR_OFF");
		AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");
	}
	// 초기 모델 상태 설정
	next = nullptr;
	m_modelState = STATE::WAIT;
	// 모델 변수 초기화
	m_type = type;
	m_idx = idx;
	m_pk = pk;
}

// 외부 상태 천이 함수
bool Atomic_Send::ExtTransFn(const WMessage& msg) {
	switch (m_type) {
	case 0:
		if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			CProduct* cnext = (CProduct*)msg.GetValue();
			next = new CProduct(*cnext);
			if (GLOBAL_VAR->readymap.at(next->m_curPk) == true && m_modelState == STATE::PAUSE && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) !=0) {
				m_modelState = STATE::SEND;
			}
			else if (GLOBAL_VAR->readymap.at(next->m_curPk)==true && m_modelState == STATE::PAUSE && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
				m_modelState = STATE::WAIT;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			CProduct* cnext = (CProduct*)msg.GetValue();
			next = new CProduct(*cnext);
			if (GLOBAL_VAR->readymap.at(next->m_curPk) == false && (m_modelState == STATE::SEND || m_modelState == STATE::WAIT)) {
				m_modelState = STATE::PAUSE;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::MAKE) {
			if (m_modelState == STATE::WAIT && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {
				m_modelState = STATE::SEND;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_ON) {	
			if (m_modelState == STATE::WAIT || m_modelState == STATE::SEND) {
				m_modelState = STATE::SERROR;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_OFF) {
			if (m_modelState == STATE::SERROR && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
				m_modelState = STATE::SEND;
			}
			else if (m_modelState == STATE::SERROR && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
				m_modelState = STATE::WAIT;
			}
			else Continue();
		}
		else Continue();
		break;
	case 1:
		if (msg.GetPort() == (unsigned int)IN_PORT::RECEIVE) {
			if (m_modelState == STATE::WAIT) {
				if (GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {
					m_modelState = STATE::SEND;
				}
			}
		    else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			CProduct* cnext = (CProduct*)msg.GetValue();
			next = new CProduct(*cnext);
			if (GLOBAL_VAR->readymap.at(next->m_curPk) == true && m_modelState == STATE::PAUSE && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
				m_modelState = STATE::SEND;
			}
			else if (GLOBAL_VAR->readymap.at(next->m_curPk) == true && m_modelState == STATE::PAUSE && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
				m_modelState = STATE::WAIT;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if (GLOBAL_VAR->readymap.at(next->m_curPk) == false && (m_modelState == STATE::WAIT || m_modelState == STATE::SEND)) {
				m_modelState = STATE::PAUSE;
			}
		}
		break;
	case 2:
		if (msg.GetPort() == (unsigned int)IN_PORT::RECEIVE) {
			if (m_modelState == STATE::WAIT) {

				if (GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {
					m_modelState = STATE::SEND;
				}
				else Continue();
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (GLOBAL_VAR->readymap.at(next->m_curPk) == true && m_modelState == STATE::PAUSE && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
				m_modelState = STATE::SEND;
			}
			else if (GLOBAL_VAR->readymap.at(next->m_curPk) == true && m_modelState == STATE::PAUSE && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
				m_modelState = STATE::WAIT;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if (GLOBAL_VAR->readymap.at(next->m_curPk) == false && (m_modelState == STATE::WAIT || m_modelState == STATE::SEND)) {
				m_modelState = STATE::PAUSE;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_ON) {
			if (m_modelState == STATE::WAIT || m_modelState == STATE::SEND) {
				m_modelState = STATE::SERROR;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_OFF) {
			if (m_modelState == STATE::SERROR && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
				m_modelState = STATE::SEND;
			}
			else if (m_modelState == STATE::SERROR && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
				m_modelState = STATE::WAIT;
			}
			else Continue();
			
		}
		break;
	case 3:
		if (msg.GetPort() == (unsigned int)IN_PORT::RECEIVE) {
			if (m_modelState == STATE::WAIT) {
				if (GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {
					m_modelState = STATE::SEND;
				}
				else Continue();
			}
			else if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_ON) {
				if (m_modelState == STATE::WAIT || m_modelState == STATE::SEND) {
					m_modelState = STATE::SERROR;
				}
				else Continue();
			}
			else if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_OFF) {
				if (m_modelState == STATE::SERROR && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
					m_modelState = STATE::SEND;
				}
				else if (m_modelState == STATE::SERROR && GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
					m_modelState = STATE::WAIT;
				}
				else Continue();

			}
			else Continue();
		}
		break;

	}
	return true;
	
}

// 내부 상태 천이 함수
bool Atomic_Send::IntTransFn() {
	if (m_modelState == STATE::PENDING) {
		m_modelState = STATE::SEND;
	}
	return true;
}

// 출력 함수
bool Atomic_Send::OutputFn(WMessage& msg) {
	if (m_modelState == STATE::SEND) {
		if (GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) == 1) {
			if (m_type == 3) {
					CProduct* product = GLOBAL_VAR->popmbuffer(0, m_pk, &GLOBAL_VAR->p_buffer);
					
					GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->stock);
					auto a = GLOBAL_VAR->stockback(m_pk, &GLOBAL_VAR->stock);
					CLOG->info("PK: {}, idx : {} Stock Size : {}", m_pk, m_idx, GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock));
					if (a != nullptr) {
						CLOG->info("PK: {}, idx : {} STOCK {}번 제품 적재 완료, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
					}
					msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
			}
			else {
				CProduct* product = GLOBAL_VAR->popmbuffer(0, m_pk, &GLOBAL_VAR->p_buffer);
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, product);
				switch (m_type) {
				case 0:
					CLOG->info("PK: {}, idx : {} GEN {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				case 1:
					CLOG->info("PK: {}, idx : {} TRACK {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				case 2:
					CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				}
			}
			m_modelState = STATE::WAIT;
		}
		else if (GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) > 1) {
			if (m_type == 3) {

				CProduct* product = GLOBAL_VAR->popmbuffer(0, m_pk, &GLOBAL_VAR->p_buffer);
				
				GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->stock);
				auto a = GLOBAL_VAR->stockback(m_pk, &GLOBAL_VAR->stock);
				CLOG->info("PK: {}, idx : {} Stock Size : {}", m_pk, m_idx, GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock));
				if (a != nullptr) {
					CLOG->info("PK: {}, idx : {} STOCK {}번 제품 적재 완료, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
				}
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
			}
			else {
				CProduct* product = GLOBAL_VAR->popmbuffer(0, m_pk, &GLOBAL_VAR->p_buffer);
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, product);
				
				switch (m_type) {
				case 0:
					CLOG->info("PK: {}, idx : {} GEN {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				case 1:
					CLOG->info("PK: {}, idx : {} TRACK {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				case 2:
					CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				}
			}
			m_modelState = STATE::PENDING;
		}
	}
	return true;
}

// TA함수
WTime Atomic_Send::TimeAdvanceFn() {
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3-+
	switch (m_type) {
	case 0:
		return TA_STATE_GEN[(int)m_modelState];
		break;
	case 1:
		if (m_modelState == STATE::SEND) {
			CProduct* cproduct = GLOBAL_VAR->frontmbuffer(0, m_pk, &GLOBAL_VAR->p_buffer);
			if (cproduct != nullptr) {
				double PasstoSend = (WAISER->CurentSimulationTime().GetValue()) - (cproduct->m_passTime);
				if ((2 - PasstoSend) <= 0) {
					double sendTime = 2.0;
					TA_STATE_TRACK[1] = sendTime;
				} else {
					double sendTime = 2 - PasstoSend;
					TA_STATE_TRACK[1] = sendTime;
				}
			}
		}
		return TA_STATE_TRACK[(int)m_modelState];
		break;
	case 2:
		return TA_STATE_PROC[(int)m_modelState];
		break;
	case 3:
		return TA_STATE_STOCK[(int)m_modelState];
		break;
	default:
		return 0;
	}
	
}
