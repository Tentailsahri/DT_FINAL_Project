#include "Atomic_Send.h"
#include <SimLogger.h>

Atomic_Send::Atomic_Send(int type, int idx, int pk) {
	// �� �̸� ����
	SetName("Atomic_Send");
	// ��,��� ��Ʈ ����
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
	// �ʱ� �� ���� ����

	m_modelState = STATE::WAIT;
	// �� ���� �ʱ�ȭ
	m_type = type;
	m_idx = idx;
	m_pk = pk;
}

// �ܺ� ���� õ�� �Լ�
bool Atomic_Send::ExtTransFn(const WMessage& msg) {
	if (m_type != 3) {
		if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (m_modelState == STATE::PAUSE) {
				if (GLOBAL_VAR->readymap[m_pk] == true && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
					m_modelState = STATE::SEND;
				}
				else if (GLOBAL_VAR->readymap[m_pk] == true && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
					m_modelState = STATE::WAIT;
				}
				else Continue();
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if ((m_modelState == STATE::SEND || m_modelState == STATE::WAIT)) {
				if (GLOBAL_VAR->readymap[m_pk] == false) {
					m_modelState = STATE::PAUSE;
				}
				else Continue();
			}
		}
	}
	if (m_type != 1) {
		if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_ON) {
			if (m_modelState == STATE::WAIT || m_modelState == STATE::SEND) {
				m_modelState = STATE::SERROR;
			}
			else Continue();
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_OFF) {
			if (m_modelState == STATE::SERROR && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
				m_modelState = STATE::SEND;
			}
			else if (m_modelState == STATE::SERROR && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
				m_modelState = STATE::WAIT;
			}
			else Continue();
		}
	}
	if (m_type != 0) {
		if (msg.GetPort() == (unsigned int)IN_PORT::RECEIVE) {
			if (m_modelState == STATE::WAIT) {
				if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {
					m_modelState = STATE::SEND;
				}
				else Continue();
			}
			else Continue();
		}
	}
	switch (m_type) {
	case 0:
		if (msg.GetPort() == (unsigned int)IN_PORT::MAKE) {
			if (m_modelState == STATE::WAIT && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {
				m_modelState = STATE::SEND;
			}
			else Continue();
		}
		break;
	}
	return true;

}

// ���� ���� õ�� �Լ�
bool Atomic_Send::IntTransFn() {
	if (m_modelState == STATE::PENDING) {
		m_modelState = STATE::SEND;
	}
	return true;
}

// ��� �Լ�
bool Atomic_Send::OutputFn(WMessage& msg) {
	if (m_modelState == STATE::SEND) {
		if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 1) {
			if (m_type == 3) {
				CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
				GLOBAL_VAR->MapPush(m_pk, product, &GLOBAL_VAR->stock);
				auto a = GLOBAL_VAR->StockBack(m_pk, &GLOBAL_VAR->stock);
				CLOG->info("PK: {}, idx : {} Stock Size : {}", m_pk, m_idx, GLOBAL_VAR->BufferSize(m_pk, &GLOBAL_VAR->stock));
				if (a != nullptr) {
					CLOG->info("PK: {}, idx : {} STOCK {}�� ��ǰ ���� �Ϸ�, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
					GLOBAL_VAR->CsvProductFlowList(m_pk, a->m_genID, a->m_passTime, WAISER->CurentSimulationTime().GetValue());
					if (GLOBAL_VAR->SQLConnect == true)
						GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(a->m_genID) + ", " + std::to_string(a->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
				}
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
				m_modelState = STATE::WAIT;
			}
			else if (GLOBAL_VAR->readymap[m_pk] == true) {
				CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
				if (m_type == 0) {
					GLOBAL_VAR->CsvProductFlowList(m_pk, product->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
					if (GLOBAL_VAR->SQLConnect == true)
						GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(product->m_genID) + ", " + std::to_string(product->m_genTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
				}
				else
				{
					GLOBAL_VAR->CsvProductFlowList(m_pk, product->m_genID, product->m_passTime, WAISER->CurentSimulationTime().GetValue());
					if (GLOBAL_VAR->SQLConnect == true)
						GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(product->m_genID) + ", " + std::to_string(product->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
				}
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, product);
				switch (m_type) {
				case 0:
					CLOG->info("PK: {}, idx : {} GEN {}�� ��ǰ �۽� �Ϸ�, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				case 1:
					CLOG->info("PK: {}, idx : {} TRACK {}�� ��ǰ �۽� �Ϸ�, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				case 2:
					CLOG->info("PK: {}, idx : {} PROC {}�� ��ǰ �۽� �Ϸ�, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				}
				m_modelState = STATE::WAIT;
			}

		}
		else if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) > 1) {
			if (m_type == 3) {
				CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
				GLOBAL_VAR->MapPush(m_pk, product, &GLOBAL_VAR->stock);
				auto a = GLOBAL_VAR->StockBack(m_pk, &GLOBAL_VAR->stock);
				CLOG->info("PK: {}, idx : {} Stock Size : {}", m_pk, m_idx, GLOBAL_VAR->BufferSize(m_pk, &GLOBAL_VAR->stock));
				if (a != nullptr) {
					CLOG->info("PK: {}, idx : {} STOCK {}�� ��ǰ ���� �Ϸ�, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
					GLOBAL_VAR->CsvProductFlowList(m_pk, a->m_genID, a->m_passTime, WAISER->CurentSimulationTime().GetValue());
					if (GLOBAL_VAR->SQLConnect == true)
						GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(a->m_genID) + ", " + std::to_string(a->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
				}
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
				m_modelState = STATE::PENDING;
			}
			else if (GLOBAL_VAR->readymap[m_pk] == true) {
				CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
				if (m_type == 0) {
					GLOBAL_VAR->CsvProductFlowList(m_pk, product->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
					if (GLOBAL_VAR->SQLConnect == true)
						GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(product->m_genID) + ", " + std::to_string(product->m_genTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
				}
				else
				{
					GLOBAL_VAR->CsvProductFlowList(m_pk, product->m_genID, product->m_passTime, WAISER->CurentSimulationTime().GetValue());
					if (GLOBAL_VAR->SQLConnect == true)
						GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(product->m_genID) + ", " + std::to_string(product->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
				}
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, product);
				switch (m_type) {
				case 0:
					CLOG->info("PK: {}, idx : {} GEN {}�� ��ǰ �۽� �Ϸ�, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				case 1:
					CLOG->info("PK: {}, idx : {} TRACK {}�� ��ǰ �۽� �Ϸ�, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				case 2:
					CLOG->info("PK: {}, idx : {} PROC {}�� ��ǰ �۽� �Ϸ�, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
					break;
				}
				m_modelState = STATE::PENDING;
			}

		}
	}
	return true;
}

// TA�Լ�
WTime Atomic_Send::TimeAdvanceFn() {
	// Ÿ�� : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3-+
	switch (m_type) {
	case 0:
		return TA_STATE_GEN[(int)m_modelState];
		break;
	case 1:
		if (m_modelState == STATE::SEND) {
			CProduct* cproduct = GLOBAL_VAR->mBufferFront(0, m_pk, &GLOBAL_VAR->p_buffer);
			if (cproduct != nullptr) {
				double PasstoSend = (WAISER->CurentSimulationTime().GetValue()) - (cproduct->m_passTime);
				if ((2 - PasstoSend) <= 0) {
					double sendTime = 2.0;
					TA_STATE_TRACK[1] = sendTime;
				}
				else {
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
