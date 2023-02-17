#include "Atomic_State.h"
#include <SimLogger.h>
#include "WAISERSim.h"

Atomic_State::Atomic_State(int type, int idx, int pk) {
	// ¸ðµ¨ ÀÌ¸§ ¼³Á¤
	SetName("Atomic_State");
	// ÀÔ,Ãâ·Â Æ÷Æ® ¼³Á¤
	// Å¸ÀÔ : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	switch (type) {
	case 0:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddOutPort((unsigned int)OUT_PORT::ERROR_ON, "ERROR_ON");
		AddOutPort((unsigned int)OUT_PORT::ERROR_OFF, "ERROR_OFF");
		AddOutPort((unsigned int)OUT_PORT::MAKE, "MAKE");
		break;
	case 1:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		break;
	case 2:
		AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
		AddInPort((unsigned int)IN_PORT::READY, "READY");
		AddOutPort((unsigned int)OUT_PORT::ERROR_ON, "ERROR_ON");
		AddOutPort((unsigned int)OUT_PORT::ERROR_OFF, "ERROR_OFF");
		break;
	case 3:
		AddOutPort((unsigned int)OUT_PORT::SEND, "SEND");
		break;
	}
	// ÃÊ±â ¸ðµ¨ »óÅÂ ¼³Á¤
	m_modelState = STATE::INIT;
	// ¸ðµ¨ º¯¼ö ÃÊ±âÈ­
	m_type = type;
	m_idx = idx;
	m_count = 0;
	m_genCount = 0;
	m_pk = pk;
	m_current_time = 0;

}
Atomic_State::~Atomic_State()
{

}
// ¿ÜºÎ »óÅÂ ÃµÀÌ ÇÔ¼ö
bool Atomic_State::ExtTransFn(const WMessage& msg) {
	// Å¸ÀÔ : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	//m_dataUpdate();
	switch (m_type) {
	case 0:
		if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if (m_modelState == STATE::ACTIVE) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (m_modelState == STATE::PAUSE) {
=======
				m_modelState = STATE::WAIT;
				CLOG->info("PK: {}, idx : {} GEN WAIT, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
=======
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
=======
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
			} else {
				Continue();
			}
		} else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
<<<<<<< HEAD
<<<<<<< HEAD
			if (m_modelState == STATE::WAIT) {
>>>>>>> ?ˆí˜•??
=======
			if (m_modelState == STATE::PAUSE) {
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
=======
			if (m_modelState == STATE::PAUSE) {
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
				m_modelState = STATE::ACTIVE;
				CLOG->info("PK: {}, idx : {} GEN ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		break;
	case 1:
		if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if (m_modelState == STATE::ACTIVE) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} TRACK PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (m_modelState == STATE::PAUSE) {
=======
				m_modelState = STATE::WAIT;
				CLOG->info("PK: {}, idx : {} TRACK WAIT, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
=======
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} TRACK PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
=======
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} TRACK PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
			} else {
				Continue();
			}
		} else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
<<<<<<< HEAD
<<<<<<< HEAD
			if (m_modelState == STATE::WAIT) {
>>>>>>> ?ˆí˜•??
=======
			if (m_modelState == STATE::PAUSE) {
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
=======
			if (m_modelState == STATE::PAUSE) {
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
				m_modelState = STATE::ACTIVE;
				CLOG->info("PK: {}, idx : {} TRACK ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		break;
	case 2:
		if (msg.GetPort() == (unsigned int)IN_PORT::PAUSE) {
			if (m_modelState == STATE::ACTIVE) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} PROC PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else {
				Continue();
			}
		}
		else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
			if (m_modelState == STATE::PAUSE) {
=======
				m_modelState = STATE::WAIT;
				CLOG->info("PK: {}, idx : {} PROC WAIT, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
=======
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} PROC PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
=======
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} PROC PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
			} else {
				Continue();
			}
		} else if (msg.GetPort() == (unsigned int)IN_PORT::READY) {
<<<<<<< HEAD
<<<<<<< HEAD
			if (m_modelState == STATE::WAIT) {
>>>>>>> ?ˆí˜•??
=======
			if (m_modelState == STATE::PAUSE) {
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
=======
			if (m_modelState == STATE::PAUSE) {
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
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

// ³»ºÎ »óÅÂ ÃµÀÌ ÇÔ¼ö
bool Atomic_State::IntTransFn() {
	// Å¸ÀÔ : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	//m_dataUpdate();
	switch (m_type) {
<<<<<<< HEAD
		//case 0:
		//	if (m_modelState == STATE::INIT) {
		//		m_modelState = STATE::ACTIVE;
		//		CLOG->info("PK: {}, idx : {} GEN ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		//	} else if (m_modelState == STATE::ACTIVE) {
		//		m_count++;
		//		m_genCount++;
		//		auto genID = m_idx * 1000 + m_genCount;
		//		CProduct* cproduct = new CProduct(genID, WAISER->CurentSimulationTime().GetValue());
		//		CProduct* product = new CProduct(*cproduct);
		//		CLOG->info("PK: {}, idx : {} GEN {}¹ø Á¦Ç° »ý»ê, at t = {}", m_pk, m_idx, genID,WAISER->CurentSimulationTime().GetValue());
		//		GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->buffer);
		//		if (GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer) >= GLOBAL_VAR->m_maxbuffer_Generator) {
		//			m_modelState = STATE::PAUSE;
		//			CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		//		} else if (m_count >= GLOBAL_VAR->error_gen) {
		//			m_modelState = STATE::SERROR;
		//			CLOG->info("PK: {}, idx : {} GEN ERROR, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		//			
		//		}
		//	} else if (m_modelState == STATE::SERROR) {
		//		m_modelState = STATE::ACTIVE;
		//		CLOG->info("PK: {}, idx : {} GEN ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
		//		m_count = 0;
		//	}
		//	break;
=======
	//case 0:
	//	if (m_modelState == STATE::INIT) {
	//		m_modelState = STATE::ACTIVE;
	//		CLOG->info("PK: {}, idx : {} GEN ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
	//	} else if (m_modelState == STATE::ACTIVE) {
	//		m_count++;
	//		m_genCount++;
	//		auto genID = m_idx * 1000 + m_genCount;
	//		CProduct* cproduct = new CProduct(genID, WAISER->CurentSimulationTime().GetValue());
	//		CProduct* product = new CProduct(*cproduct);
	//		CLOG->info("PK: {}, idx : {} GEN {}¹ø Á¦Ç° »ý»ê, at t = {}", m_pk, m_idx, genID,WAISER->CurentSimulationTime().GetValue());
	//		GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->buffer);
	//		if (GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer) >= GLOBAL_VAR->m_maxbuffer_Generator) {
	//			m_modelState = STATE::PAUSE;
	//			CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
	//		} else if (m_count >= GLOBAL_VAR->error_gen) {
	//			m_modelState = STATE::SERROR;
	//			CLOG->info("PK: {}, idx : {} GEN ERROR, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
	//			
	//		}
	//	} else if (m_modelState == STATE::SERROR) {
	//		m_modelState = STATE::ACTIVE;
	//		CLOG->info("PK: {}, idx : {} GEN ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
	//		m_count = 0;
	//	}
	//	break;
>>>>>>> ?ˆí˜•??
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
			if (GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer) != 0) {
				CProduct* product = GLOBAL_VAR->popmap(m_pk, &GLOBAL_VAR->buffer);
				GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->stock);
				auto a = GLOBAL_VAR->stockback(m_pk, &GLOBAL_VAR->stock);
				CLOG->info("PK: {}, idx : {} Stock Size : {}", m_pk, m_idx, GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock));
				if (a != nullptr) {
					CLOG->info("PK: {}, idx : {} STOCK {}¹ø Á¦Ç° ÀûÀç ¿Ï·á, at t = {}", m_pk, m_idx, a->m_genID, WAISER->CurentSimulationTime().GetValue());
				}
			}
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
		if (WAISER->CurentSimulationTime().GetValue() >= GLOBAL_VAR->time) {
			CLOG->info("Á¦ÀÛ °³¼ö : {} Æò±Õ Á¦ÀÛ ½Ã°£ : {}", GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock), (double)GLOBAL_VAR->time / GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->stock));
		}
		break;
	}

	return true;
}

// Ãâ·Â ÇÔ¼ö
bool Atomic_State::OutputFn(WMessage& msg) {
	// Å¸ÀÔ : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	//m_dataUpdate();
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
			CLOG->info("PK: {}, idx : {} GEN {}¹ø Á¦Ç° »ý»ê, at t = {}", m_pk, m_idx, genID, WAISER->CurentSimulationTime().GetValue());
			GLOBAL_VAR->pushmap(m_pk, product, &GLOBAL_VAR->buffer);
			CLOG->info("PK: {}, idx : {} GEN MAKE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			msg.SetPortValue((unsigned int)(unsigned int)OUT_PORT::MAKE, nullptr);
			if (GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer) >= GLOBAL_VAR->m_maxbuffer_Generator) {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			}
			else if (m_count >= GLOBAL_VAR->error_gen) {
=======
				m_modelState = STATE::WAIT;
				CLOG->info("PK: {}, idx : {} GEN WAIT, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
=======
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
=======
				m_modelState = STATE::PAUSE;
				CLOG->info("PK: {}, idx : {} GEN PAUSE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
>>>>>>> parent of e24e3a4 (PAUSEë¥¼ WAITë¡œ ê³ ì¹¨)
			} else if (m_count >= GLOBAL_VAR->error_gen) {
>>>>>>> ?ˆí˜•??
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
			if (GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer) != 0) {
				m_count++;
				CLOG->info("PK: {}, idx : {} PROC BUFFER SIZE = {}", m_pk, m_idx, GLOBAL_VAR->buffer_size(m_pk, &GLOBAL_VAR->buffer));
			}
			if (m_count >= GLOBAL_VAR->error_proc) {
				m_modelState = STATE::SERROR;
				CLOG->info("PK: {}, idx : {} PROC ERROR, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
				msg.SetPortValue((unsigned int)(unsigned int)OUT_PORT::ERROR_ON, nullptr);
			}
		}
		else if (m_modelState == STATE::SERROR) {
			m_modelState = STATE::ACTIVE;
			CLOG->info("PK: {}, idx : {} PROC ACTIVE, at t = {}", m_pk, m_idx, WAISER->CurentSimulationTime().GetValue());
			msg.SetPortValue((unsigned int)(unsigned int)OUT_PORT::ERROR_OFF, nullptr);
			m_count = 0;
		}
		else Continue();
		break;
	case 3:
		if (m_modelState == STATE::ACTIVE) {
			msg.SetPortValue((unsigned int)(unsigned int)OUT_PORT::SEND, nullptr);
		}
		break;
	}

	return true;
}

// TAÇÔ¼ö
WTime Atomic_State::TimeAdvanceFn() {
	// Å¸ÀÔ : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
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

// ¸ðµ¨ÀÇ »óÅÂº¯¼ö¸¦ ¹®ÀÚ¿­·Î º¯È¯
const char* Atomic_State::getState2Str(Atomic_State::STATE type) {
	// switch ¶Ç´Â if·Î ±¸Çö
	switch (type) {
	case Atomic_State::STATE::INIT:
		return "INIT";
	case Atomic_State::STATE::ACTIVE:
		return "ACTIVE";
	case Atomic_State::STATE::SERROR:
		return "ERROR";
	case Atomic_State::STATE::PAUSE:
		return "WAIT";
	}
}

void Atomic_State::m_dataUpdate() {
	if (GLOBAL_VAR->SQLConnect == true) {
		GLOBAL_VAR->pgconn->SendQuery("INSERT INTO \"object_state_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" (project_id, object_id, object_state, state_start_time, state_end_time) VALUES(1, " + std::to_string(m_pk) + ", '" + getState2Str(m_modelState) + "', " + std::to_string(m_current_time) + ", " + std::to_string(WAISER->CurentSimulationTime().GetValue()) + ")");
	}
	GLOBAL_VAR->CsvStateInsert(m_pk, getState2Str(m_modelState), m_current_time, WAISER->CurentSimulationTime().GetValue());
	m_current_time = WAISER->CurentSimulationTime().GetValue();
}
