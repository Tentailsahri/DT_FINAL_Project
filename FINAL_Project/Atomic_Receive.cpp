#include "Atomic_Receive.h"
#include <SimLogger.h>

Atomic_Receive::Atomic_Receive(int type, int idx, int pk) {
	// �� �̸� ����
	SetName("Atomic_Receive");
	// ��,��� ��Ʈ ����
	// Ÿ�� : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3

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

	// �ʱ� �� ���� ����
	m_modelState = STATE::INIT;
	m_product = nullptr;
	
	// �� ���� �ʱ�ȭ
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
	init_time = GLOBAL_VAR->TA_STATE_INIT[m_type];
}

// �ܺ� ���� õ�� �Լ�
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
					CLOG->info("PK: {}, idx : {} {} {} {}�� ��ǰ ���� �Ϸ�, at t = {}", m_pk, m_idx, m_subIdx, m_product->m_curType, m_product->m_genID, WAISER->CurentSimulationTime().GetValue());
					GLOBAL_VAR->mBufferPush(m_subIdx, m_pk, m_product, &GLOBAL_VAR->p_buffer);
				}
			}
			if (GLOBAL_VAR->TA_STATE_INIT[m_type] <= WAISER->CurentSimulationTime().GetValue() && ReadyMapBuffer>=1) {
				GLOBAL_VAR->readymap[m_pk].at(sendreadymapNum) = true;
				ReadyMapBuffer = 0;
			}
			m_modelState = STATE::READYMAP;
			
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
			
			if (GLOBAL_VAR->TA_STATE_INIT[m_type] <= WAISER->CurentSimulationTime().GetValue()) {
				GLOBAL_VAR->readymap[m_pk].at(sendreadymapNum) = true;
				
			}
			else {
				GLOBAL_VAR->readymap[m_pk].at(sendreadymapNum) = false;
				ReadyMapBuffer++;
			}
			CLOG->info("{} pk readymap {}", m_pk, GLOBAL_VAR->readymap[m_pk].at(sendreadymapNum));
		}
		
		m_modelState = STATE::READYMAP;
	} else if (m_type != 3 && msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
		GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(m_pk));
		for (int i = 0; i < bufferPopNum; i++) {
			bufferPop[i] = std::stoi(PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 0));
		}
		if (bufferPopNum == 1) {
			CProduct* cnext = (CProduct*)msg.GetValue();
			if (cnext != nullptr) {
				CProduct* next = new CProduct(*cnext);
				if(m_pk == next->m_pastPk)
				GLOBAL_VAR->readymap[m_pk].at(0) = false;
			}
			
		}
		else {
			CProduct* cnext = (CProduct*)msg.GetValue();
			if (cnext != nullptr) {
				CProduct* next = new CProduct(*cnext);
				for (int i = 0; i < bufferPopNum; i++) {
					if (next->m_curPk == bufferPop[i] && m_pk==next->m_pastPk) {
						GLOBAL_VAR->readymap[m_pk].at(i) = false;
					}
				}
			}
		}
		
		m_modelState = STATE::READYMAP;
	} else Continue();

	return true;
}

// ���� ���� õ�� �Լ�
bool Atomic_Receive::IntTransFn() {
	if (m_modelState == STATE::READYMAP) {
		if(GLOBAL_VAR->TA_STATE_INIT[m_type] <= WAISER->CurentSimulationTime().GetValue())
			m_modelState = STATE::DECISION;
		else {
			init_time = GLOBAL_VAR->TA_STATE_INIT[m_type] - WAISER->CurentSimulationTime().GetValue();
			m_modelState = STATE::INIT;
		}
	}else if (m_modelState == STATE::INIT) {
		m_modelState = STATE::RECEIVE;
	}
	
	return true;
}

// ��� �Լ�
bool Atomic_Receive::OutputFn(WMessage& msg) {
	if (m_modelState == STATE::DECISION) {
		CProduct* next = new CProduct(2, 0.0);
		next->m_curPk = m_pk;
		int sendPop[100] = { 0 };
		int fullNum1 = 0;
		int sendSize = 0;
		GLOBAL_VAR->pgconn->SendQuery("SELECT send_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE receive_object_id=" + std::to_string(m_pk));
		for (int i = 0; i < PQntuples(GLOBAL_VAR->pgconn->GetSQLResult()); i++) {
			sendPop[i] = std::stoi(PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 0));

		}
		CLOG->info("PK: {}, idx : {} {} Buffer size {}, at t = {}", m_pk, m_idx, getModel2Str(m_type), GLOBAL_VAR->mBufferSize(m_subIdx, m_pk, &GLOBAL_VAR->p_buffer), WAISER->CurentSimulationTime().GetValue());
		next->m_pastPk = sendPop[m_subIdx];
		if (GLOBAL_VAR->m_maxbuffer[m_type] <= GLOBAL_VAR->mBufferSize(m_subIdx, m_pk, &GLOBAL_VAR->p_buffer)) {
			msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, next);
			
		} else {
			msg.SetPortValue((unsigned int)OUT_PORT::READY, next);
			
		}
		GLOBAL_VAR->pgconn->SendQuery("SELECT send_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE receive_object_id=" + std::to_string(m_pk));
		for (int i = 0; i < PQntuples(GLOBAL_VAR->pgconn->GetSQLResult()); i++) {
			if (GLOBAL_VAR->m_maxbuffer[m_type] <= GLOBAL_VAR->mBufferSize(m_subIdx, m_pk, &GLOBAL_VAR->p_buffer)) {
				fullNum1++;
			}
			sendSize++;
		}
		if (fullNum1 == sendSize) {
			m_modelState = STATE::FULL;
		}
		else {
			m_modelState = STATE::RECEIVE;
		}
	}

	return true;
}

// TA�Լ�
WTime Atomic_Receive::TimeAdvanceFn() {
	if (m_modelState != STATE::INIT) {
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
	else return init_time;
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