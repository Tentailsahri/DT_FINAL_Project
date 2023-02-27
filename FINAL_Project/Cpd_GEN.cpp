#include "Cpd_GEN.h"

Cpd_GEN::Cpd_GEN(int gen_idx, int pk) {
	
	SetName("Cpd_GEN");// 모델 이름 지정

	AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
	AddInPort((unsigned int)IN_PORT::READY, "READY");
	AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");

	// 모델 생성
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	WAtomModel* State = new Atomic_State(0, gen_idx, pk);
	WAtomModel* Send = new Atomic_Send(0, gen_idx, pk);
	WAtomModel* receive = new Atomic_Receive(0, gen_idx, pk);

	// 생성한 모델 연결
	AddComponent(State);
	AddComponent(Send);
	AddComponent(receive);

	// 모델 포트 연결
	AddCoupling(receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, State, (unsigned int)Atomic_State::IN_PORT::PAUSE);
	AddCoupling(receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, State, (unsigned int)Atomic_State::IN_PORT::READY);
	AddCoupling(this, (unsigned int)IN_PORT::PAUSE, Send, (unsigned int)Atomic_Send::IN_PORT::PAUSE);
	AddCoupling(this, (unsigned int)IN_PORT::PAUSE, receive, (unsigned int)Atomic_Receive::IN_PORT::PAUSE);
	AddCoupling(this, (unsigned int)IN_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::READY);
	AddCoupling(this, (unsigned int)IN_PORT::READY, receive, (unsigned int)Atomic_Receive::IN_PORT::READY);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, this, (unsigned int)OUT_PORT::PRODUCT);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_ON, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_ON);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_OFF, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_OFF);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::MAKE, Send, (unsigned int)Atomic_Send::IN_PORT::MAKE);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::MAKE, receive, (unsigned int)Atomic_Receive::IN_PORT::PRODUCT);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, receive, (unsigned int)Atomic_Receive::IN_PORT::SEND);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, State, (unsigned int)Atomic_State::IN_PORT::POP);

}
