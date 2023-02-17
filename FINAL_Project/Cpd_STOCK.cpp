#include "Cpd_STOCK.h"

Cpd_STOCK::Cpd_STOCK(int stock_idx, int pk) {
	SetName("Cpd_STOCK");// 모델 이름 지정

	AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
	AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
	AddOutPort((unsigned int)OUT_PORT::READY, "READY");

	// 모델 생성
	// 타입 : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	WAtomModel* Receive = new Atomic_Receive(3, stock_idx, pk);
	WAtomModel* State = new Atomic_State(3, stock_idx, pk);

	// 생성한 모델 연결
	AddComponent(Receive);
	AddComponent(State);

	// 모델 포트 연결
	AddCoupling(this, (unsigned int)IN_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::PRODUCT);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, this, (unsigned int)OUT_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::SEND, Receive, (unsigned int)Atomic_Receive::IN_PORT::SEND);

}
