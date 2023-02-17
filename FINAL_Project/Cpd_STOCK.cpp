#include "Cpd_STOCK.h"

Cpd_STOCK::Cpd_STOCK(int stock_idx, int pk) {
	SetName("Cpd_STOCK");// �� �̸� ����

	AddInPort((unsigned int)IN_PORT::PRODUCT, "PRODUCT");
	AddOutPort((unsigned int)OUT_PORT::PAUSE, "PAUSE");
	AddOutPort((unsigned int)OUT_PORT::READY, "READY");

	// �� ����
	// Ÿ�� : GEN = 0, TRACK = 1, PROC = 2, STOCK = 3
	WAtomModel* Receive = new Atomic_Receive(3, stock_idx, pk);
	WAtomModel* State = new Atomic_State(3, stock_idx, pk);

	// ������ �� ����
	AddComponent(Receive);
	AddComponent(State);

	// �� ��Ʈ ����
	AddCoupling(this, (unsigned int)IN_PORT::PRODUCT, Receive, (unsigned int)Atomic_Receive::IN_PORT::PRODUCT);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::PAUSE, this, (unsigned int)OUT_PORT::PAUSE);
	AddCoupling(Receive, (unsigned int)Atomic_Receive::OUT_PORT::READY, this, (unsigned int)OUT_PORT::READY);
	AddCoupling(State, (unsigned int)Atomic_State::OUT_PORT::SEND, Receive, (unsigned int)Atomic_Receive::IN_PORT::SEND);

}
