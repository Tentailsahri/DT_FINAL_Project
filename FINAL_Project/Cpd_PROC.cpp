#include "Cpd_PROC.h"

Cpd_PROC::Cpd_PROC(int proc_idx, int proc_subidx, int pk) {
	SetName("Cpd_PROC");// 모델 이름 지정
	
	AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
	AddInPort((unsigned int)IN_PORT::READY, "READY");
	for (int i = 0; i < proc_subidx; i++) {
		AddInPort((unsigned int)IN_PORT::PRODUCT+i, "PRODUCT" + std::to_string(i));
		AddOutPort((unsigned int)OUT_PORT::PAUSE+i, "PAUSE" + std::to_string(i));
		AddOutPort((unsigned int)OUT_PORT::READY+i, "READY" + std::to_string(i));
	}
	AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");
	

	WAtomModel* State = new Atomic_State(2, proc_idx, pk);
	WAtomModel* Send = new Atomic_Send(2, proc_idx, pk);

	AddComponent(State);
	AddComponent(Send);
	// 모델 생성
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	std::vector<WAtomModel*> Receive_vec;
	for (int i = 0; i < proc_subidx; i++) {
		WAtomModel* Receive = new Atomic_Receive(2, proc_idx, i, pk);
		Receive_vec.push_back(Receive);
		
	}
	for (int i = 0; i < proc_subidx; i++) {
		AddComponent(Receive_vec.at(i));
	    AddCoupling(this, (unsigned int)IN_PORT::PRODUCT+i, Receive_vec.at(i), (unsigned int)Atomic_Receive::IN_PORT::PRODUCT);
		AddCoupling(this, (unsigned int)IN_PORT::READY, Receive_vec.at(i), (unsigned int)Atomic_Receive::IN_PORT::READY);
		AddCoupling(this, (unsigned int)IN_PORT::PAUSE, Receive_vec.at(i), (unsigned int)Atomic_Receive::IN_PORT::PAUSE);
		AddCoupling(Receive_vec.at(i), (unsigned int)Atomic_Receive::OUT_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::RECEIVE);
		AddCoupling(Receive_vec.at(i), (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, this, (unsigned int)OUT_PORT::PAUSE+i);
		AddCoupling(Receive_vec.at(i), (unsigned int)Atomic_Receive::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY+i);
		AddCoupling(Receive_vec.at(i), (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, State, (unsigned int)Atomic_State::IN_PORT::PAUSE);
		AddCoupling(Receive_vec.at(i), (unsigned int)Atomic_Receive::OUT_PORT::READY, State, (unsigned int)Atomic_State::IN_PORT::READY);
		AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, Receive_vec.at(i), (unsigned int)Atomic_Receive::IN_PORT::SEND);
	}
	

	// 생성한 모델 연결
	

	// 모델 포트 연결
	
	
	AddCoupling(this, (unsigned int)IN_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::READY);
	AddCoupling(this, (unsigned int)IN_PORT::PAUSE, Send, (unsigned int)Atomic_Send::IN_PORT::PAUSE);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, this, (unsigned int)OUT_PORT::PRODUCT);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_ON, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_ON);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_OFF, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_OFF);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, State, (unsigned int)Atomic_State::IN_PORT::POP);
	
	

}

Cpd_PROC::Cpd_PROC(int proc_idx, int pk)
{
	SetName("Cpd_PROC");// 모델 이름 지정

	AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
	AddInPort((unsigned int)IN_PORT::READY, "READY");
	AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
	AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
	AddOutPort((unsigned int)OUT_PORT::READY, "READY");
	AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");


	WAtomModel* State = new Atomic_State(2, proc_idx, pk);
	WAtomModel* Send = new Atomic_Send(2, proc_idx, pk);
	WAtomModel* Receive = new Atomic_Receive(2, proc_idx, 0, pk);
	AddComponent(State);
	AddComponent(Send);
	AddComponent(Receive);
	// 모델 생성
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	


	// 생성한 모델 연결


	// 모델 포트 연결

	AddCoupling(this, (unsigned int)IN_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::PRODUCT);
	AddCoupling(this, (unsigned int)IN_PORT::READY, Receive, (unsigned int)Atomic_Receive::IN_PORT::READY);
	AddCoupling(this, (unsigned int)IN_PORT::PAUSE, Receive, (unsigned int)Atomic_Receive::IN_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::RECEIVE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, this, (unsigned int)OUT_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, State, (unsigned int)Atomic_State::IN_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, State, (unsigned int)Atomic_State::IN_PORT::READY);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::SEND);
	AddCoupling(this, (unsigned int)IN_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::READY);
	AddCoupling(this, (unsigned int)IN_PORT::PAUSE, Send, (unsigned int)Atomic_Send::IN_PORT::PAUSE);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, this, (unsigned int)OUT_PORT::PRODUCT);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_ON, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_ON);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_OFF, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_OFF);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, State, (unsigned int)Atomic_State::IN_PORT::SEND);
}
