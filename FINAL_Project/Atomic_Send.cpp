#include "Atomic_Send.h"
#include <SimLogger.h>

Atomic_Send::Atomic_Send(int type, int idx, int pk) {
	// 모델 이름 설정
	SetName("Atomic_Send");
	// 입,출력 포트 설정
	// 타입: GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
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
	GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(pk));
	getValue = new int[PQntuples(GLOBAL_VAR->pgconn->GetSQLResult())]();
	trueCount = 0;
	trueValueCount = 0;
	trueStockBufferCount = 0;
	tuplesNum = 0;
	bufOneCount = 0;
	readyMapCount = 0;
	bufferPopNum = 0;
	bufferSameCount = 0;
	bufferSERRORCount = 0;
	bufferSERRORNum = 0;
	PauseReadyMapCount = 0;
	bufferSendCount = 0;
	bufferSend = 0;
	min = 10000;
	minnum = 0;
	minPk = -1;
}
Atomic_Send::~Atomic_Send()
{
	delete[] getValue;
}
// 외부 상태 천이 함수
bool Atomic_Send::ExtTransFn(const WMessage& msg) {
	if (m_type != 3) {
		if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			CProduct* product = (CProduct*)msg.GetValue();
			CProduct* cproduct = new CProduct(*product);
			if (m_type != 0) {
				if (m_modelState == STATE::PAUSE) {
					GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(m_pk));
					for (int i = 0; i < PQntuples(GLOBAL_VAR->pgconn->GetSQLResult()); i++) {
						if(GLOBAL_VAR->readymap[m_pk].at(i) == true)
						         PauseReadyMapCount++;
					}
					GLOBAL_VAR->pgconn->SendQuery("SELECT send_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE receive_object_id=" + std::to_string(m_pk));
					bufferSend = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
					for (int i = 0; i < bufferSend; i++) {
						if(GLOBAL_VAR->mBufferSize(i, m_pk, &GLOBAL_VAR->p_buffer) >=1)
						bufferSendCount++;
					}
					if (PauseReadyMapCount>=1 && bufferSendCount==bufferSend) {
						m_modelState = STATE::SEND;
					} else if (PauseReadyMapCount >= 1 && bufferSendCount != bufferSend) {
						m_modelState = STATE::WAIT;
					} else Continue();
				}
				else Continue();
				PauseReadyMapCount = 0;
				bufferSendCount = 0;
			} else if (m_type == 0) {
			
					if (GLOBAL_VAR->readymap[m_pk].at(0) == true && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
						m_modelState = STATE::SEND;
					} else if (GLOBAL_VAR->readymap[m_pk].at(0) == true && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
						m_modelState = STATE::WAIT;
					} else Continue();
				
			}
		} else if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if ((m_modelState == STATE::SEND || m_modelState == STATE::WAIT)) {
				GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(m_pk));
				bufferSend = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
				for (int i = 0; i <bufferSend ; i++) {
					if (GLOBAL_VAR->readymap[m_pk].at(i) == false)
						PauseReadyMapCount++;
				}
				if (bufferSend==PauseReadyMapCount) {
					m_modelState = STATE::PAUSE;
				} else Continue();
			}
			PauseReadyMapCount = 0;
		}
	}
	if (m_type != 1) {
		if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_ON) {
			if (m_modelState == STATE::WAIT || m_modelState == STATE::SEND) {
				m_modelState = STATE::SERROR;
			} else Continue();
		} else if (msg.GetPort() == (unsigned int)IN_PORT::ERROR_OFF) {
			if (m_type != 2) {
				if (m_modelState == STATE::SERROR && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
					m_modelState = STATE::SEND;
				}
				else if (m_modelState == STATE::SERROR && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
					m_modelState = STATE::WAIT;
				}
			}
			else if (m_type == 2) {
			if (GLOBAL_VAR->scenario_num == 1 && m_modelState == STATE::SERROR && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
				m_modelState = STATE::SEND;
			}
			else if (GLOBAL_VAR->scenario_num == 1 && m_modelState == STATE::SERROR && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
				m_modelState = STATE::WAIT;
			}
			else if (GLOBAL_VAR->scenario_num != 1 && m_modelState == STATE::SERROR) {
				GLOBAL_VAR->pgconn->SendQuery("SELECT send_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE receive_object_id=" + std::to_string(m_pk));
				bufferSERRORNum = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
				for (int i = 0; i < bufferSERRORNum; i++) {
					if (GLOBAL_VAR->mBufferSize(i, m_pk, &GLOBAL_VAR->p_buffer) == 0) {
						bufferSERRORCount++;
					}
				}
				if (bufferSERRORCount != 0) {
					m_modelState = STATE::WAIT;
				}
				else m_modelState = STATE::SEND;
				
			}
			bufferSERRORCount = 0;
			}
			else Continue();
		}
	}
	if (m_type != 0) {
		if (msg.GetPort() == (unsigned int)IN_PORT::RECEIVE) {
			if (m_modelState == STATE::WAIT) {
				GLOBAL_VAR->pgconn->SendQuery("SELECT send_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE receive_object_id=" + std::to_string(m_pk));
				bufferSend = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
				for (int i = 0; i < bufferSend; i++) {
					if (GLOBAL_VAR->mBufferSize(i, m_pk, &GLOBAL_VAR->p_buffer) >=1)
						bufferSendCount++;
				}
				if (bufferSend==bufferSendCount) {
					m_modelState = STATE::SEND;
				} else Continue();
				bufferSendCount = 0;
			} else Continue();
		}
	} else if (m_type == 0 && msg.GetPort() == (unsigned int)IN_PORT::MAKE) {
		if (m_modelState == STATE::WAIT && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {
			m_modelState = STATE::SEND;
		} else Continue();
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
		if (m_type != 2 && GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) >= 1) {
			if (m_type == 3) {
				CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
				GLOBAL_VAR->MapPush(m_pk, product, &GLOBAL_VAR->stock);
				auto a = GLOBAL_VAR->StockBack(m_pk, &GLOBAL_VAR->stock);
				CLOG->info("PK: {}, idx : {} Stock Size : {}", m_pk, m_idx, GLOBAL_VAR->BufferSize(m_pk, &GLOBAL_VAR->stock));
				if (a != nullptr) {
					CLOG->info("PK: {}, idx : {} {} {}번 제품 적재 완료, at t = {}", m_pk, m_idx, getModel2Str(m_type), a->m_genID, WAISER->CurentSimulationTime().GetValue());
					GLOBAL_VAR->CsvProductFlowList(m_pk, a->m_genID, a->m_passTime, WAISER->CurentSimulationTime().GetValue());
					m_sendPassQuery(a);

				}
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, nullptr);
				if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 1) {
					m_modelState = STATE::WAIT;
				} else if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) > 1) {
					m_modelState = STATE::PENDING;
				}
			} else if (GLOBAL_VAR->readymap[m_pk].at(0) == true) {
				CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
				GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(m_pk));
				product->m_targetPk = std::stoi(PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), 0, 0));
				msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, product);
				if (m_type == 0) {
					GLOBAL_VAR->CsvProductFlowList(m_pk, product->m_genID, product->m_genTime, WAISER->CurentSimulationTime().GetValue());
					m_sendGenQuery(product);
				} else {
					GLOBAL_VAR->CsvProductFlowList(m_pk, product->m_genID, product->m_passTime, WAISER->CurentSimulationTime().GetValue());
					m_sendPassQuery(product);
				}
				CLOG->info("PK: {}, idx : {} {} {}번 제품 송신 완료, at t = {}", m_pk, m_idx, getModel2Str(m_type), product->m_genID, WAISER->CurentSimulationTime().GetValue());
				if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) == 1) {
					m_modelState = STATE::WAIT;
				} else if (GLOBAL_VAR->mBufferSize(0, m_pk, &GLOBAL_VAR->p_buffer) > 1) {
					m_modelState = STATE::PENDING;
				}
			}
		} else if (m_type == 2) {

			GLOBAL_VAR->pgconn->SendQuery("SELECT send_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE receive_object_id=" + std::to_string(m_pk));
			bufferPopNum = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
			for (int i = 0; i < bufferPopNum; i++) {
				if (GLOBAL_VAR->mBufferSize(i, m_pk, &GLOBAL_VAR->p_buffer) >= 1) bufferSameCount++;
			}
			if (bufferPopNum == bufferSameCount) {
				CProduct* product = GLOBAL_VAR->mBufferPop(0, m_pk, &GLOBAL_VAR->p_buffer);
				for (int i = 1; i < bufferPopNum; i++) {
					CProduct* product1 = GLOBAL_VAR->mBufferPop(i, m_pk, &GLOBAL_VAR->p_buffer);
				}
				GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE send_object_id=" + std::to_string(m_pk));
				for (int i = 0; i < PQntuples(GLOBAL_VAR->pgconn->GetSQLResult()); i++) {
					if (GLOBAL_VAR->readymap[m_pk].at(i) == true) {
						readyMapCount++;
					}
				}
				if (readyMapCount >= 1) {
					if (GLOBAL_VAR->SQLConnect == false) {
						/*if (GLOBAL_VAR->readymap[m_pk].at(1) == true && GLOBAL_VAR->readymap[m_pk].at(0) == true) {
							std::uniform_int_distribution<int> u_dis(7, 8);
							newproduct->m_targetPk = u_dis(WAISER->random_gen_);
						} else if (GLOBAL_VAR->readymap[m_pk].at(1) == true && GLOBAL_VAR->readymap[m_pk].at(0) == false) {
							newproduct->m_targetPk = 8;
						} else if (GLOBAL_VAR->readymap[m_pk].at(0) == true && GLOBAL_VAR->readymap[m_pk].at(1) == false) {
							newproduct->m_targetPk = 7;
						}*/
					} else if (GLOBAL_VAR->SQLConnect == true) {
						product->m_targetPk = m_whereTargetPk(m_pk);
					}

					msg.SetPortValue((unsigned int)OUT_PORT::PRODUCT, product);
					GLOBAL_VAR->CsvProductFlowList(m_pk, product->m_genID, product->m_passTime, WAISER->CurentSimulationTime().GetValue());
					m_sendPassQuery(product);
					CLOG->info("PK: {}, idx : {} {} {}번 제품 송신 완료, at t = {}", m_pk, m_idx, getModel2Str(m_type), product->m_genID, WAISER->CurentSimulationTime().GetValue());
					for (int i = 0; i < bufferPopNum; i++) {
						if (GLOBAL_VAR->mBufferSize(i, m_pk, &GLOBAL_VAR->p_buffer) == 1) {
							bufOneCount++;
						}
					}
					if (bufOneCount == 0) {
						m_modelState = STATE::PENDING;
					} else if (bufOneCount >= 1) {
						m_modelState = STATE::WAIT;
					};
					bufOneCount = 0;
					readyMapCount = 0;
				}

			}
			bufferSameCount = 0;

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
			CProduct* cproduct = GLOBAL_VAR->mBufferFront(0, m_pk, &GLOBAL_VAR->p_buffer);
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

const char* Atomic_Send::getModel2Str(int m_type) {
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


void Atomic_Send::m_sendPassQuery(CProduct* product) {
	if (GLOBAL_VAR->SQLConnect == true) {
		GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(" + std::to_string(GLOBAL_VAR->scenario_num) + ", " + std::to_string(m_pk) + ", " + std::to_string(product->m_genID) + ", " + std::to_string(product->m_passTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
	}
}

int Atomic_Send::m_whereTargetPk(int pk)
{
	trueValueCount = 0;
	trueStockBufferCount = 0;
	trueCount = 0;
	GLOBAL_VAR->pgconn->SendQuery("SELECT receive_object_id FROM \"obj_coup_list"+std::to_string(GLOBAL_VAR->scenario_num)+"\" WHERE send_object_id=" + std::to_string(pk));
	tuplesNum = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
	for (int i = 0; i < tuplesNum; i++) {
	      getValue[i] = std::stoi(PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 0));
		  if (GLOBAL_VAR->readymap[pk].at(i) == true) {
			  trueCount++;
		  }
	}
	if (trueCount == 1) {
		for (int i = 0; i < tuplesNum ; i++) {
			if (GLOBAL_VAR->readymap[pk].at(i) == true) {
				trueValue[0]=getValue[i];
			}
		}
		return trueValue[0];
	}
	else if (trueCount > 1) {
		
		for (int i = 0; i < tuplesNum ; i++) {
			if (GLOBAL_VAR->readymap[pk].at(i) == true) {
				trueValue[trueValueCount] = getValue[i];
				trueValueCount++;
			}
		}
		
		for (int i = 0; i < trueCount; i++) {
			if (min>trueStockBuffer[i]) {
				min=trueStockBuffer[i];
				minPk = trueValue[i];
				minnum = i;
			}
			
		}
		trueStockBuffer[minnum]++;
		min = trueStockBuffer[minnum];
		return minPk;

	}
	else return 0;
}

void Atomic_Send::m_sendGenQuery(CProduct* product) {
	if (GLOBAL_VAR->SQLConnect == true) {
		GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"product_flow_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, product_id, in_time, out_time) VALUES(" + std::to_string(GLOBAL_VAR->scenario_num) + ", " + std::to_string(m_pk) + ", " + std::to_string(product->m_genID) + ", " + std::to_string(product->m_genTime) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
	}
}
