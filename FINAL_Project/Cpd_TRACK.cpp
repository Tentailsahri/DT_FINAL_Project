#include "Cpd_TRACK.h"

Cpd_TRACK::Cpd_TRACK(int track_idx, int pk) {
	SetName("Cpd_TRACK");// �� �̸� ����

	AddInPort((unsigned int)IN_PORT::PAUSE, "PAUSE");
	AddInPort((unsigned int)IN_PORT::READY, "READY");
	AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
	AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
	AddOutPort((unsigned int)OUT_PORT::READY, "READY");
	AddOutPort((unsigned int)OUT_PORT::PRODUCT, "PRODUCT");

	// �� ����
	// Ÿ�� : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	WAtomModel* Receive = new Atomic_Receive(1, track_idx, pk);
	WAtomModel* State = new Atomic_State(1, track_idx, pk);
	WAtomModel* Send = new Atomic_Send(1, track_idx, pk);

	// ������ �� ����
	AddComponent(Receive);
	AddComponent(State);
	AddComponent(Send);

	// �� ��Ʈ ����
	AddCoupling(this, (unsigned int)IN_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::PRODUCT);
	AddCoupling(this, (unsigned int)IN_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::READY);
	AddCoupling(this, (unsigned int)IN_PORT::PAUSE, Send, (unsigned int)Atomic_Send::IN_PORT::PAUSE);
	AddCoupling(this, (unsigned int)IN_PORT::READY, Receive, (unsigned int)Atomic_Receive::IN_PORT::READY);
	AddCoupling(this, (unsigned int)IN_PORT::PAUSE, Receive, (unsigned int)Atomic_Receive::IN_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::RECEIVE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, this, (unsigned int)OUT_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, State, (unsigned int)Atomic_State::IN_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, State, (unsigned int)Atomic_State::IN_PORT::READY);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, this, (unsigned int)OUT_PORT::PRODUCT);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::SEND);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, State, (unsigned int)Atomic_State::IN_PORT::SEND);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY);

}
