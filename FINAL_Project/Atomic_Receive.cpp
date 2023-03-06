#include "Atomic_Receive.h"
#include <SimLogger.h>

Atomic_Receive::Atomic_Receive(int type, int idx, int subidx, int pk) {
	// �� �̸� ����
	SetName("Atomic_Receive");
	// ��,��� ��Ʈ ����
	// Ÿ�� : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
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

	// �ʱ� �� ���� ����
	
	m_modelState = STATE::INIT;
	m_product = nullptr;
	// �� ���� �ʱ�ȭ
	m_type = type;
	m_idx = idx;
	m_pk = pk;
	GLOBAL_VAR->readymap[m_pk].push_back(false);
	if(GLOBAL_VAR->scenario_num!=1) GLOBAL_VAR->readymap[m_pk].push_back(false);
	m_subidx = subidx;
}

// �ܺ� ���� õ�� �Լ�
bool Atomic_Receive::ExtTransFn(const WMessage& msg) {
	
	if (msg.GetPort() == (unsigned int)IN_PORT::PRODUCT) {
		if (m_modelState == STATE::RECEIVE) {
			
		 if (m_type != 0) {
				CProduct* cproduct = (CProduct*)msg.GetValue();
				m_product = new CProduct(*cproduct);
				if (GLOBAL_VAR->scenario_num == 2 && (m_pk == 7 || m_pk == 8)) {
					if (m_product->m_targetPk == m_pk) {
						m_product->m_passTime = WAISER->CurentSimulationTime().GetValue();
						m_product->m_pastPk = m_product->m_curPk;
						m_product->m_pastType = m_product->m_curType;
						m_product->m_curPk = m_pk;
						m_product->m_curType = getModel2Str(m_type);
						CLOG->info("PK: {}, idx : {} {} {} {}�� ��ǰ ���� �Ϸ�, at t = {}", m_pk, m_idx, m_subidx, m_product->m_curType, m_product->m_genID, WAISER->CurentSimulationTime().GetValue());
						CLOG->info("pastPk={} pastType={} curPk={} curtype={}", m_product->m_pastPk, m_product->m_pastType, m_product->m_curPk, m_product->m_curType);
						GLOBAL_VAR->mBufferPush(m_subidx, m_pk, m_product, &GLOBAL_VAR->p_buffer);
					}
				}
				else {
					m_product->m_passTime = WAISER->CurentSimulationTime().GetValue();
					m_product->m_pastPk = m_product->m_curPk;
					m_product->m_pastType = m_product->m_curType;
					m_product->m_curPk = m_pk;
					m_product->m_curType = getModel2Str(m_type);
					CLOG->info("PK: {}, idx : {} {} {} {}�� ��ǰ ���� �Ϸ�, at t = {}", m_pk, m_idx, m_subidx, m_product->m_curType, m_product->m_genID, WAISER->CurentSimulationTime().GetValue());
					CLOG->info("pastPk={} pastType={} curPk={} curtype={}", m_product->m_pastPk, m_product->m_pastType, m_product->m_curPk, m_product->m_curType);
					GLOBAL_VAR->mBufferPush(m_subidx, m_pk, m_product, &GLOBAL_VAR->p_buffer);
				}
			}
			m_modelState = STATE::DECISION;
		}
	}
	else if (msg.GetPort() == (unsigned int)IN_PORT::SEND) {
		if (m_modelState == STATE::FULL) {
			m_modelState = STATE::DECISION;
		}
	}
	else if (m_type != 3 && msg.GetPort() == (unsigned int)IN_PORT::READY) {
		if (GLOBAL_VAR->scenario_num == 1) GLOBAL_VAR->readymap[m_pk].at(0) = true;
		else if (GLOBAL_VAR->scenario_num == 2 && m_pk != 10) GLOBAL_VAR->readymap[m_pk].at(0) = true;
		else if (GLOBAL_VAR->scenario_num == 2 && m_pk == 10) {
			CProduct* cnext = (CProduct*)msg.GetValue();
			if (cnext != nullptr) {
				CProduct* next = new CProduct(*cnext);
				if (next->m_curPk == 7) {
					GLOBAL_VAR->readymap[m_pk].at(0) = true;
				}
				else if (next->m_curPk == 8) {
					GLOBAL_VAR->readymap[m_pk].at(1) = true;
				}
			}
		}
		m_modelState = STATE::READYMAP;
	}
	else if (m_type != 3 && msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
		if (GLOBAL_VAR->scenario_num == 1) GLOBAL_VAR->readymap[m_pk].at(0) = false;
		else if (GLOBAL_VAR->scenario_num == 2 && m_pk != 10) GLOBAL_VAR->readymap[m_pk].at(0) = false;
		else if (GLOBAL_VAR->scenario_num == 2 && m_pk == 10) {
			CProduct* cnext = (CProduct*)msg.GetValue();
			if (cnext != nullptr) {
				CProduct* next = new CProduct(*cnext);
				if (next->m_curPk == 7) {
					GLOBAL_VAR->readymap[m_pk].at(0) = false;
				}
				else if (next->m_curPk == 8) {
					GLOBAL_VAR->readymap[m_pk].at(1) = false;
				}
			}
		}
		m_modelState = STATE::READYMAP;
	}
	else Continue();
	return true;
}

// ���� ���� õ�� �Լ�
bool Atomic_Receive::IntTransFn() {
	if (m_modelState == STATE::READYMAP) {
		if (GLOBAL_VAR->m_maxbuffer_Generator > GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
			m_modelState = STATE::RECEIVE;
		}
		else if (GLOBAL_VAR->m_maxbuffer_Generator <= GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
			m_modelState = STATE::FULL;
		}
	}
	if (m_modelState == STATE::INIT) {
		m_modelState = STATE::RECEIVE;
	}
	
	return true;
}

// ��� �Լ�
bool Atomic_Receive::OutputFn(WMessage& msg) {
	switch (m_type) {
	case 0:
	if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} GEN Buffer size {}", m_pk, m_idx, GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer));
			if (GLOBAL_VAR->m_maxbuffer_Generator <= GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, nullptr);
				m_modelState = STATE::FULL;
			}
			else if (GLOBAL_VAR->m_maxbuffer_Generator > GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
				msg.SetPortValue((unsigned int)OUT_PORT::READY, nullptr);
				m_modelState = STATE::RECEIVE;
			}
		}
		
		break;
	case 1:
		 if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} TRACK Buffer size {}", m_pk, m_idx, GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer));
			if (GLOBAL_VAR->m_maxbuffer_Receive > GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
				CProduct* next = new CProduct(1, 0.0);
				next->m_curPk = m_pk;
				msg.SetPortValue((unsigned int)OUT_PORT::READY, next);
				m_modelState = STATE::RECEIVE;
			} else if(GLOBAL_VAR->m_maxbuffer_Receive <= GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)){
				CProduct* next = new CProduct(1, 0.0);
				next->m_curPk = m_pk;
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, next);
				m_modelState = STATE::FULL;
			}
		}
		
		break;
	case 2:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} PROC Buffer {} size {}", m_pk, m_idx, m_subidx, GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer));
			if (GLOBAL_VAR->m_maxbuffer_Process > GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
				CProduct* next = new CProduct(1, 0.0);
				next->m_curPk = m_pk;
				msg.SetPortValue((unsigned int)OUT_PORT::READY, next);
				m_modelState = STATE::RECEIVE;
			} else if (GLOBAL_VAR->m_maxbuffer_Process <= GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
				CProduct* next = new CProduct(1, 0.0);
				next->m_curPk = m_pk;
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, next);
				m_modelState = STATE::FULL;
			}
		}
		
		break;
	case 3:
		if (m_modelState == STATE::DECISION) {
			CLOG->info("PK: {}, idx : {} STOCK Buffer size {}", m_pk, m_idx, GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer));
			if (GLOBAL_VAR->m_maxbuffer_Stock > GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
				CProduct* next = new CProduct(1, 0.0);
				next->m_curPk = m_pk;
				msg.SetPortValue((unsigned int)OUT_PORT::READY, next);
				m_modelState = STATE::RECEIVE;
			} else if (GLOBAL_VAR->m_maxbuffer_Stock <= GLOBAL_VAR->mBufferSize(m_subidx, m_pk, &GLOBAL_VAR->p_buffer)) {
				CProduct* next = new CProduct(1, 0.0);
				next->m_curPk = m_pk;
				msg.SetPortValue((unsigned int)OUT_PORT::PAUSE, next);
				m_modelState = STATE::FULL;
			}
		}
		
		break;
	}

	return true;
}

// TA�Լ�
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