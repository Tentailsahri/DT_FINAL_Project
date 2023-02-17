#include "Cpd_PROC.h"

Cpd_PROC::Cpd_PROC(int proc_idx, int pk) {
	SetName("Cpd_PROC");// 모델 이름 지정
	
	AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
	AddInPort((unsigned int)IN_PORT::READY, "READY");
	AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
	AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
	AddOutPort((unsigned int)OUT_PORT::READY, "READY");
	AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");

	// 모델 생성
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	WAtomModel* Receive = new Atomic_Receive(2, proc_idx, pk);
	WAtomModel* State = new Atomic_State(2, proc_idx, pk);
	WAtomModel* Send = new Atomic_Send(2, proc_idx, pk);

	// 생성한 모델 연결
	AddComponent(Receive);
	AddComponent(State);
	AddComponent(Send);

	// 모델 포트 연결
	
	AddCoupling(this, (unsigned int)IN_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::PRODUCT);
	AddCoupling(this, (unsigned int)IN_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::READY);
	AddCoupling(this, (unsigned int)IN_PORT::PAUSE, Send, (unsigned int)Atomic_Send::IN_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::READY);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, this, (unsigned int)OUT_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, State, (unsigned int)Atomic_State::IN_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, State, (unsigned int)Atomic_State::IN_PORT::READY);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, this, (unsigned int)OUT_PORT::PRODUCT);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::SEND);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_ON, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_ON);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_OFF, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_OFF);

}
