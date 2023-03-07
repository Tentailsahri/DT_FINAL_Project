#include "Atomic_Send.h"
#include <SimLogger.h>

Atomic_Send::Atomic_Send(int type, int idx, int pk) {
	// 모델 이름 설정
	SetName("Atomic_Send");
	// 입,출력 포트 설정
	switch (type) {
	case 0:
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::MAKE, "MAKE");
		AddInPort((unsigned int)IN_PORT::ERROR_ON, "ERROR_ON");
		AddInPort((unsigned int)IN_PORT::ERROR_OFF, "ERROR_OFF");
		AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");
		break;
	case 1:
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::RECEIVE, "RECEIVE");
		AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");
		break;
	case 2:
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
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

	m_modelState = STATE::WAIT;
	// 모델 변수 초기화
	m_type = type;
	m_idx = idx;
	m_pk = pk;
	newgencount = 0;
	newgencount1 = 0;
}

// 외부 상태 천이 함수
bool Atomic_Send::ExtTransFn(const WMessage& msg) {
	if (m_type != 3) {
		if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (m_modelState == STATE::PAUSE) {
				if (GLOBAL_VAR->readymap[m_pk].at(0) == true && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
					m_modelState = STATE::SEND;
				}
				else if (GLOBAL_VAR->readymap[m_pk].at(0) == true && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
					m_modelState = STATE::WAIT;
				}
				else Continue();
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if ((m_modelState == STATE::SEND || m_modelState == STATE::WAIT)) {
				if (GLOBAL_VAR->readymap[m_pk].at(0) == false) {
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

// 내부 상태 천이 함수
bool Atomic_Send::IntTransFn() {
	if (m_modelState == STATE::PENDING) {
		m_modelState = STATE::SEND;
	}
	return true;
}

// 출력 함수
bool Atomic_Send::OutputFn(WMessage& msg) {
	switch (GLOBAL_VAR->scenario_num) {
	case 1:
		if (m_modelState == STATE::SEND) {
			if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 1) {
				if (m_type == 3) {
					CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
					GLOBAL_VAR->MapPush(m_pk, product, &GLOBAL_VAR->stock);
					auto a = GLOBAL_VAR->StockBack(m_pk, &GLOBAL_VAR->stock);
					CLOG->info("PK: {}, idx : {} Stock Size : {}", m_pk, m_idx, GLOBAL_VAR->BufferSize(m_pk, &GLOBAL_VAR->stock));
					if (a != nullptr) {
						CLOG->info("PK: {}, idx : {} STOCK {}번 제품 적재 완료, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
						GLOBAL_VAR->CsvProductFlowList(m_pk, a->m_genID, a->m_passTime, WAISER->CurentSimulationTime().GetValue());
						if (GLOBAL_VAR->SQLConnect == true)
							GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(a->m_genID) + ", " + std::to_string(a->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
					}
					msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
					m_modelState = STATE::WAIT;
				}
				else if (GLOBAL_VAR->readymap[m_pk].at(0) == true) {
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
						CLOG->info("PK: {}, idx : {} GEN {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					case 1:
						CLOG->info("PK: {}, idx : {} TRACK {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					case 2:
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
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
						CLOG->info("PK: {}, idx : {} STOCK {}번 제품 적재 완료, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
						GLOBAL_VAR->CsvProductFlowList(m_pk, a->m_genID, a->m_passTime, WAISER->CurentSimulationTime().GetValue());
						if (GLOBAL_VAR->SQLConnect == true)
							GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(a->m_genID) + ", " + std::to_string(a->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
					}
					msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
					m_modelState = STATE::PENDING;
				}
				else if (GLOBAL_VAR->readymap[m_pk].at(0) == true) {
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
						CLOG->info("PK: {}, idx : {} GEN {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					case 1:
						CLOG->info("PK: {}, idx : {} TRACK {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					case 2:
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					}
					m_modelState = STATE::PENDING;
				}

			}
		}
		break;
	case 2:
		if (m_modelState == STATE::SEND) {
			if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 1) {
				if (m_type == 3) {
					CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
					GLOBAL_VAR->MapPush(m_pk, product, &GLOBAL_VAR->stock);
					auto a = GLOBAL_VAR->StockBack(m_pk, &GLOBAL_VAR->stock);
					CLOG->info("PK: {}, idx : {} Stock Size : {}", m_pk, m_idx, GLOBAL_VAR->BufferSize(m_pk, &GLOBAL_VAR->stock));
					if (a != nullptr) {
						CLOG->info("PK: {}, idx : {} STOCK {}번 제품 적재 완료, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
						GLOBAL_VAR->CsvProductFlowList(m_pk, a->m_genID, a->m_passTime, WAISER->CurentSimulationTime().GetValue());
						if (GLOBAL_VAR->SQLConnect == true)
							GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(a->m_genID) + ", " + std::to_string(a->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
					}
					msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
					m_modelState = STATE::WAIT;
				}
				else if (m_type!=2 && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
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
						CLOG->info("PK: {}, idx : {} GEN {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					case 1:
						CLOG->info("PK: {}, idx : {} TRACK {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					}
					m_modelState = STATE::WAIT;
				}
				else if (m_type == 2 && m_idx==0 && GLOBAL_VAR->mBufferSize(1, m_pk, &GLOBAL_VAR->p_buffer) >= 1 && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
					    CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
						product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
						newgencount++;
						int genid= 3000 + newgencount;
						CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
						msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
						GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
						m_modelState = STATE::WAIT;
				}
				else if (m_type == 2 && m_idx == 1 && GLOBAL_VAR->mBufferSize(1, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {

						if (GLOBAL_VAR->readymap[m_pk].at(0) == true && GLOBAL_VAR->readymap[m_pk].at(1) == false) {
							CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
							product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
							newgencount1++;
							int genid = 4000 + newgencount1;
							CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
							newproduct->m_targetPk = 7;
							msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
							GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
							CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
						}
						else if (GLOBAL_VAR->readymap[m_pk].at(1) == true && GLOBAL_VAR->readymap[m_pk].at(0) == false) {
							CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
							product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
							newgencount1++;
							int genid = 4000 + newgencount1;
							CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
							newproduct->m_targetPk = 8;
							msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
							GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
							CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
						}
						else if(GLOBAL_VAR->readymap[m_pk].at(1) == true && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
							CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
							product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
							newgencount1++;
							int genid = 4000 + newgencount1;
							CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
							std::uniform_int_distribution<int> u_dis(7, 8);
							newproduct->m_targetPk = u_dis(WAISER->random_gen_);
							msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
							GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
							CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
							
						}
						m_modelState = STATE::WAIT;
				}
                }
			else if (m_type == 2 && GLOBAL_VAR->mBufferSize(1, m_pk, &GLOBAL_VAR->p_buffer) == 1) {
				if (m_idx == 0 && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1 && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
					CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
					product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
					newgencount++;
					int genid = 3000 + newgencount;
					CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
					msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
					GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
					CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
					m_modelState = STATE::WAIT;
				}
				else if (m_idx == 1 && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {

					if (GLOBAL_VAR->readymap[m_pk].at(0) == true && GLOBAL_VAR->readymap[m_pk].at(1) == false) {
						CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
						product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
						newgencount1++;
						int genid = 4000 + newgencount1;
						CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
						newproduct->m_targetPk = 7;
						msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
						GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
					}
					else if (GLOBAL_VAR->readymap[m_pk].at(1) == true && GLOBAL_VAR->readymap[m_pk].at(0) == false) {
						CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
						product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
						newgencount1++;
						int genid = 4000 + newgencount1;
						CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
						newproduct->m_targetPk = 8;
						msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
						GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
					}
					else if (GLOBAL_VAR->readymap[m_pk].at(1) == true && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
						CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
						product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
						newgencount1++;
						int genid = 4000 + newgencount1;
						CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
						std::uniform_int_distribution<int> u_dis(7, 8);
						newproduct->m_targetPk = u_dis(WAISER->random_gen_);
						msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
						GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());

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
						CLOG->info("PK: {}, idx : {} STOCK {}번 제품 적재 완료, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
						GLOBAL_VAR->CsvProductFlowList(m_pk, a->m_genID, a->m_passTime, WAISER->CurentSimulationTime().GetValue());
						if (GLOBAL_VAR->SQLConnect == true)
							GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(a->m_genID) + ", " + std::to_string(a->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
					}
					msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
					m_modelState = STATE::PENDING;
				}
				else if (m_type != 2 && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
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
						CLOG->info("PK: {}, idx : {} GEN {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					case 1:
						CLOG->info("PK: {}, idx : {} TRACK {}번 제품 송신 완료, at t = {}", m_pk, m_idx, product->m_genID, WAISER->CurentSimulationTime().GetValue());
						break;
					}
					m_modelState = STATE::PENDING;
				}
				else if (m_type == 2 && m_idx == 0 && GLOBAL_VAR->mBufferSize(1, m_pk, &GLOBAL_VAR->p_buffer) > 1 && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
					CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
					product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
					newgencount++;
					int genid = 3000 + newgencount;
					CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
					msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
					GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
					CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
					m_modelState = STATE::PENDING;
				}
				else if (m_type == 2 && m_idx == 1 && GLOBAL_VAR->mBufferSize(1, m_pk, &GLOBAL_VAR->p_buffer) > 1) {

					if (GLOBAL_VAR->readymap[m_pk].at(0) == true && GLOBAL_VAR->readymap[m_pk].at(1) == false) {
						CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
						product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
						newgencount1++;
						int genid = 4000 + newgencount1;
						CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
						newproduct->m_targetPk = 7;
						msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
						GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
					}
					else if (GLOBAL_VAR->readymap[m_pk].at(1) == true && GLOBAL_VAR->readymap[m_pk].at(0) == false) {
						CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
						product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
						newgencount1++;
						int genid = 4000 + newgencount1;
						CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
						newproduct->m_targetPk = 8;
						msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
						GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());
					}
					else if (GLOBAL_VAR->readymap[m_pk].at(1) == true && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
						CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
						product = GLOBAL_VAR->mBufferPop(1, m_pk, &GLOBAL_VAR->p_buffer);
						newgencount1++;
						int genid = 4000 + newgencount1;
						CProduct* newproduct = new CProduct(genid, WAISER->CurentSimulationTime().GetValue());
						std::uniform_int_distribution<int> u_dis(7, 8);
						newproduct->m_targetPk = u_dis(WAISER->random_gen_);
						msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, newproduct);
						GLOBAL_VAR->CsvProductFlowList(m_pk, newproduct->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
						CLOG->info("PK: {}, idx : {} PROC {}번 제품 송신 완료, at t = {}", m_pk, m_idx, newproduct->m_genID, WAISER->CurentSimulationTime().GetValue());

					}
					m_modelState = STATE::PENDING;
				}

			}
		}
		break;
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
