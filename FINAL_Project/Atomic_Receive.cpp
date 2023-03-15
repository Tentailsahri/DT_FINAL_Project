#include "Atomic_Receive.h"
#include <SimLogger.h>

Atomic_Receive::Atomic_Receive(int type, int idx, int pk) {
	// 모델 이름 설정
	SetName("Atomic_Receive");
	// 입,출력 포트 설정
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3

	AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
	AddInPort((unsigned int)IN_PORT::SEND, "SEND");
	AddOutPort((unsigned int)OUT_PORT::READY, "READY");
	AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");

	switch (type) {
	case 0:
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		break;
	case 1:
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		break;
	case 2:
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		break;
	case 3:
		break;
	}

	// 초기 모델 상태 설정
	m_modelState = STATE::INIT;
	m_product = nullptr;
	
	// 모델 변수 초기화
	m_type = type;
	m_idx = idx;
	m_pk = pk;
	GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(m_pk));
	bufferPopNum = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
	for(int i = 0; i < bufferPopNum; i++) {
		GLOBAL_VAR->readymap[m_pk].push_back(false);
	}
	GLOBAL_VAR->m_subIdxMapUpdate(m_pk);
	m_subIdx = GLOBAL_VAR->m_subIdxMap.at(m_pk);
}

// 외부 상태 천이 함수
bool Atomic_Receive::ExtTransFn(const WMessage& msg) {

	if (msg.GetPort() == (unsigned int)IN_PORT::PRODUCT) {
		if (m_modelState == STATE::RECEIVE) {
			if (m_type != 0) {
				CProduct* cproduct = (CProduct*)msg.GetValue();
				m_product = new CProduct(*cproduct);
				if (m_product->m_targetPk == m_pk) {
					m_product->m_passTime = WAISER->CurentSimulationTime().GetValue();
					m_product->m_pastPk = m_product->m_curPk;
					m_product->m_pastType = m_product->m_curType;
					m_product->m_curPk = m_pk;
					m_product->m_curType = getModel2Str(m_type);
					CLOG->info("PK: {}, idx : {} {} {} {}번 제품 수신 완료, at t = {}", m_pk, m_idx, m_subIdx, m_product->m_curType, m_product->m_genID, WAISER->CurentSimulationTime().GetValue());
					GLOBAL_VAR->mBufferPush(m_subIdx, m_pk, m_product, &GLOBAL_VAR->p_buffer);
				}
			}
				m_modelState = STATE::DECISION;
			
		}
	}
	else if (msg.GetPort() == (unsigned int)IN_PORT::SEND) {
		if (m_modelState == STATE::FULL) {
			m_modelState = STATE::DECISION;
		}
	} else if (m_type != 3 && msg.GetPort() == (unsigned int)IN_PORT::READY) {
		CProduct* cnext = (CProduct*)msg.GetValue();
		if (cnext != nullptr) {
			CProduct* next = new CProduct(*cnext);

			GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(m_pk));
			for (int i = 0; i < bufferPopNum; i++) {
				bufferPop[i] = std::stoi(PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 0));

			}
			for (int i = 0; i < bufferPopNum; i++) {
				if (bufferPop[i] == next->m_curPk) {
					sendreadymapNum = i;
				}
			}
			GLOBAL_VAR->readymap[m_pk].at(sendreadymapNum) = true;
		}
		m_modelState = STATE::READYMAP;
	} else if (m_type != 3 && msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
		GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(m_pk));
		for (int i = 0; i < bufferPopNum; i++) {
			bufferPop[i] = std::stoi(PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 0));
		}
		if (bufferPopNum == 1) {
			GLOBAL_VAR->readymap[m_pk].at(0) = false;
		}
		else {
			CProduct* cnext = (CProduct*)msg.GetValue();
			if (cnext != nullptr) {
				CProduct* next = new CProduct(*cnext);
				for (int i = 0; i < bufferPopNum; i++) {
					if (next->m_curPk == bufferPop[i]) {
						GLOBAL_VAR->readymap[m_pk].at(i) = false;
					}
				}
			}
		}
		
		m_modelState = STATE::READYMAP;
	} else Continue();

	return true;
}

// 내부 상태 천이 함수
bool Atomic_Receive::IntTransFn() {
	if (m_modelState == STATE::READYMAP) {
		m_modelState = STATE::DECISION;
	}else if (m_modelState == STATE::INIT) {
		m_modelState = STATE::RECEIVE;
	}
	
	return true;
}

// 출력 함수
bool Atomic_Receive::OutputFn(WMessage& msg) {
	if (m_modelState == STATE::DECISION) {
		CProduct* next = new CProduct(2, 0.0);
		next->m_curPk = m_pk;
		CLOG->info("PK: {}, idx : {} {} Buffer size {}, at t = {}", m_pk, m_idx, getModel2Str(m_type), GLOBAL_VAR->mBufferSize(m_subIdx, m_pk, &GLOBAL_VAR->p_buffer), WAISER->CurentSimulationTime().GetValue());
		if (GLOBAL_VAR->m_maxbuffer[m_type] <= GLOBAL_VAR->mBufferSize(m_subIdx, m_pk, &GLOBAL_VAR->p_buffer)) {
			msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, next);
			m_modelState = STATE::FULL;
		} else {
			msg.SetPortValue((unsigned int)OUT_PORT::READY, next);
			m_modelState = STATE::RECEIVE;
		}
	}

	return true;
}

// TA함수
WTime Atomic_Receive::TimeAdvanceFn() {
	switch (m_type) {
	case 0:
		return TA_STATE_GEN[(int)m_modelState];
		break;
	case 1:
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

const char* Atomic_Receive::getModel2Str(int m_type) {
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