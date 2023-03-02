#include "Cpd_STOCK.h"

Cpd_STOCK::Cpd_STOCK(int stock_idx, int pk) {
	SetName("Cpd_STOCK");// �� �̸� ����

	AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
	AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
	AddOutPort((unsigned int)OUT_PORT::READY, "READY");

	// �� ����
	// Ÿ�� : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	WAtomModel* Receive = new Atomic_Receive(3, stock_idx, 0, pk);
	WAtomModel* State = new Atomic_State(3, stock_idx, pk);
	WAtomModel* Send = new Atomic_Send(3, stock_idx, pk);

	// ������ �� ����
	AddComponent(Receive);
	AddComponent(State);
	AddComponent(Send);

	// �� ��Ʈ ����
	AddCoupling(this, (unsigned int)IN_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::PRODUCT);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, this, (unsigned int)OUT_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, Send, (unsigned int)Atomic_Send::IN_PORT::RECEIVE);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_ON, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_ON);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::ERROR_OFF, Send, (unsigned int)Atomic_Send::IN_PORT::ERROR_OFF);
	AddCoupling(Send, (unsigned int)Atomic_Send::OUT_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::SEND);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY);
}
