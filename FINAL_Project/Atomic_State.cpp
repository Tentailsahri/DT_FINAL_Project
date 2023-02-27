#include "Atomic_State.h"
#include <SimLogger.h>
#include "WAISERSim.h"

Atomic_State::Atomic_State(int type, int idx, int pk) {
	// 모델 이름 설정
	SetName("Atomic_State");
	// 입,출력 포트 설정
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	switch (type) {
	case 0:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::POP, "POP");
		AddOutPort((unsigned int)OUT_PORT::ERROR_ON, "ERROR_ON");
		AddOutPort((unsigned int)OUT_PORT::ERROR_OFF, "ERROR_OFF");
		AddOutPort((unsigned int)OUT_PORT::MAKE, "MAKE");
		break;
	case 1:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::POP, "POP");
		break;
	case 2:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddInPort((unsigned int)IN_PORT::POP, "POP");
		AddOutPort((unsigned int)OUT_PORT::ERROR_ON, "ERROR_ON");
		AddOutPort((unsigned int)OUT_PORT::ERROR_OFF, "ERROR_OFF");
		break;
	case 3:
		AddOutPort((unsigned int)OUT_PORT::ERROR_ON, "ERROR_ON");
		AddOutPort((unsigned int)OUT_PORT::ERROR_OFF, "ERROR_OFF");
		break;
	}
	// 초기 모델 상태 설정
	m_modelState = STATE::INIT;
	// 모델 변수 초기화
	m_type = type;
	m_idx = idx;
	m_count = 0;
	m_genCount = 0;
	m_pk = pk;
	m_current_time = 0;
	m_endtime = 0;
}
Atomic_State::~Atomic_State()
{
   m_dataUpdate();
	if (m_type == 3) {
		CLOG->info("총 적재개수 : {} 평균 적재 시간 : {}", GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock), (double)GLOBAL_VAR->time / GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock));
   }
}
// 외부 상태 천이 함수
bool Atomic_State::ExtTransFn(const WMessage& msg) {
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	m_dataUpdate();
	switch (m_type) {
	case 0:
		if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if (m_modelState == STATE::ACTIVE) {
				m_modelState = STATE::WAIT;
				CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (m_modelState == STATE::WAIT) {
				m_modelState = STATE::ACTIVE;
				CLOG->info("PK: {}, idx : {} GEN ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else Continue();
		
		break;
	case 1:
		if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if (m_modelState == STATE::ACTIVE) {
				m_modelState = STATE::WAIT;
				CLOG->info("PK: {}, idx : {} TRACK PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (m_modelState == STATE::WAIT) {
				m_modelState = STATE::ACTIVE;
				CLOG->info("PK: {}, idx : {} TRACK ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else Continue();
		break;
	case 2:
		if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if (m_modelState == STATE::ACTIVE) {
				m_modelState = STATE::WAIT;
				CLOG->info("PK: {}, idx : {} PROC PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (m_modelState == STATE::WAIT) {
				m_modelState = STATE::ACTIVE;
				CLOG->info("PK: {}, idx : {} PROC ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else {
			Continue();
		}
		break;
	}

	return true;
}

// 내부 상태 천이 함수
bool Atomic_State::IntTransFn() {
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	m_dataUpdate();
	switch (m_type) {
	case 1:
		if (m_modelState == STATE::INIT) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} TRACK ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		}
		break;
	case 3:
		if (m_modelState == STATE::INIT) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} STOCK ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		}
		else if (m_modelState == STATE::ACTIVE) {
			m_count++;
			if (m_count >= GLOBAL_VAR->error_stock) {
				m_modelState = STATE::SERROR;
				CLOG->info("PK: {}, idx : {} STOCK ERROR, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
		}
		else if (m_modelState == STATE::SERROR) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} STOCK ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			m_count = 0;
		}
		
		break;
	}

	return true;
}

// 출력 함수
bool Atomic_State::OutputFn(WMessage& msg) {
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	m_dataUpdate();
	switch (m_type) {
	case 0:
		if (m_modelState == STATE::INIT) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} GEN ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		}
		else if (m_modelState == STATE::ACTIVE) {
			m_count++;
			m_genCount++;
			auto genID = m_idx * 1000 + m_genCount;
			CProduct* cproduct = new CProduct(genID, WAISER->CurentSimulationTime().GetValue());
			CProduct* product = new CProduct(*cproduct);
			product->m_curPk = m_pk;
			product->m_curType = "GEN";
			CLOG->info("PK: {}, idx : {} GEN {}번 제품 생산, at t = {}", m_pk, m_idx, genID, WAISER->CurentSimulationTime().GetValue());
			CLOG->info("curPk={} curtype={}", product->m_curPk, product->m_curType);
			GLOBAL_VAR->pushmbuffer(0, m_pk, product, &GLOBAL_VAR->p_buffer);
			CLOG->info("PK: {}, idx : {} GEN MAKE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			CLOG->info("GEN BUFFER SIZE : {} at {}", GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer), WAISER->CurentSimulationTime().GetValue());
			msg.SetPortValue((unsigned int)(unsigned int)OUT_PORT::MAKE, nullptr);
			if (GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) >= GLOBAL_VAR->m_maxbuffer_Generator) {
				m_modelState = STATE::WAIT;
				CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else if (m_count >= GLOBAL_VAR->error_gen) {
				m_modelState = STATE::SERROR;
				CLOG->info("PK: {}, idx : {} GEN ERROR, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
				msg.SetPortValue((unsigned int)(unsigned int)OUT_PORT::ERROR_ON, nullptr);
			}
		}
		else if (m_modelState == STATE::SERROR) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} GEN ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			msg.SetPortValue((unsigned int)(unsigned int)OUT_PORT::ERROR_OFF, nullptr);
			m_count = 0;
		}
		break;
	case 2:
		if (m_modelState == STATE::INIT) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} PROC ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		}
		else if (m_modelState == STATE::ACTIVE) {
			if (GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer) != 0) {
				m_count++;
				CLOG->info("PK: {}, idx : {} PROC BUFFER SIZE = {}", m_pk, m_idx, GLOBAL_VAR->mbuffer_size(0, m_pk, &GLOBAL_VAR->p_buffer));
				if (m_count >= GLOBAL_VAR->error_proc) {
					m_modelState = STATE::SERROR;
					CLOG->info("PK: {}, idx : {} PROC ERROR, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
					msg.SetPortValue((unsigned int)OUT_PORT::ERROR_ON, nullptr);
				}
			}
		}
		else if (m_modelState == STATE::SERROR) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} PROC ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			msg.SetPortValue((unsigned int)OUT_PORT::ERROR_OFF, nullptr);
			m_count = 0;
		}
		break;
	case 3:
		if (m_modelState == STATE::INIT) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} STOCK ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		}
		else if (m_modelState == STATE::ACTIVE) {
			m_count++;
			if (m_count >= GLOBAL_VAR->error_stock) {
				m_modelState = STATE::SERROR;
				CLOG->info("PK: {}, idx : {} STOCK ERROR, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
				msg.SetPortValue((unsigned int)OUT_PORT::ERROR_ON, nullptr);
			}
		}
		else if (m_modelState == STATE::SERROR) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} STOCK ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			msg.SetPortValue((unsigned int)OUT_PORT::ERROR_OFF, nullptr);
			m_count = 0;
		}

		break;
	}

	return true;
}

// TA함수
WTime Atomic_State::TimeAdvanceFn() {
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
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

// 모델의 상태변수를 문자열로 변환
const char* Atomic_State::getState2Str(Atomic_State::STATE type) {
	// switch 또는 if로 구현
	switch (type) {
	case Atomic_State::STATE::INIT:
		return "INIT";
	case Atomic_State::STATE::ACTIVE:
		return "ACTIVE";
	case Atomic_State::STATE::SERROR:
		return "ERROR";
	case Atomic_State::STATE::WAIT:
		return "WAIT";
	}
}

const char* Atomic_State::getModel2Str(int m_type)
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

void Atomic_State::m_dataUpdate() {
	if (WAISER->CurentSimulationTime().GetValue() < m_current_time) {
		m_endtime = GLOBAL_VAR->time;
	}
	else {
		m_endtime = WAISER->CurentSimulationTime().GetValue();
	}
	if (GLOBAL_VAR->SQLConnect == true) {
		GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"object_state_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, object_state, state_start_time, state_end_time) VALUES(1, " + std::to_string(m_pk) + ", '" + getState2Str(m_modelState) + "', " + std::to_string(m_current_time) + ", " + std::to_string(m_endtime) + ")");
		GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"state_time_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, \"current_time\", init_time, active_time, error_time, wait_time) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(m_endtime) + ", " + std::to_string(timeStore[0]) + ", " + std::to_string(timeStore[1]) + ", " + std::to_string(timeStore[2]) + ", " + std::to_string(timeStore[3]) + ")");
		GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"state_rate_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, \"current_time\", init_rate, active_rate, error_rate, wait_rate) VALUES(1, " + std::to_string(m_pk) + ", " + std::to_string(m_endtime) + ", " + std::to_string(100*timeStore[0]/m_endtime) + ", " + std::to_string(100*timeStore[1]/m_endtime) + ", " + std::to_string(100*timeStore[2]/m_endtime) + ", " + std::to_string(100*timeStore[3]/m_endtime) + ")");
		GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"buf_count_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, object_type, \"current_time\", buffer_count, stock_count) VALUES(1, " + std::to_string(m_pk) + ", '" + getModel2Str(m_type) + "', " + std::to_string(m_endtime) + ", " + std::to_string(GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer)) + ", " + std::to_string(GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock))+")");
	}
	timeStore[(int)m_modelState] = timeStore[(int)m_modelState]+ (m_endtime - m_current_time);
    GLOBAL_VAR->CsvStateInsert(m_pk, getState2Str(m_modelState), m_current_time, m_endtime);
	GLOBAL_VAR->CsvStateTimeInsert(m_pk, m_endtime, timeStore[0], timeStore[1], timeStore[2], timeStore[3]);
	GLOBAL_VAR->CsvStateRateInsert(m_pk, m_endtime, timeStore[0], timeStore[1], timeStore[2], timeStore[3]);
	GLOBAL_VAR->CsvBufferSize(m_pk, getModel2Str(m_type), m_endtime, GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer), GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock));
	m_current_time = WAISER->CurentSimulationTime().GetValue();
}
