#include "Cpd_Main.h"

Cpd_Main::Cpd_Main(int scenario_num)
{
	SetName("MainCouple");// 모델 이름 지정

	if (scenario_num == 1) {
		// 모델 생성
		WCoupModel* gen = new Cpd_GEN(0,0);
		WCoupModel* track0 = new Cpd_TRACK(0,1);
		WCoupModel* track1 = new Cpd_TRACK(1,2);
		WCoupModel* proc = new Cpd_PROC(0,3);
		WCoupModel* stock = new Cpd_STOCK(0,4);

		// 생성한 모델 연결
		AddComponent(gen);
		AddComponent(track0);
		AddComponent(track1);
		AddComponent(proc);
		AddComponent(stock);

		// 모델 포트 연결
		coupGenTrack(gen, track0);
		coupTrackProc(track0, proc);
		coupProcTrack(proc, track1);
		coupTrackStock(track1, stock);

	} else if (scenario_num == 2) {

	} else if (scenario_num == 3) {

	}

}

void Cpd_Main::coupGenTrack(WCoupModel* GEN, WCoupModel* TRACK) {
	AddCoupling(GEN, (unsigned int)Cpd_GEN::OUT_PORT::PRODUCT, TRACK, (unsigned int)Cpd_TRACK::IN_PORT::PRODUCT);
	AddCoupling(TRACK, (unsigned int)Cpd_TRACK::OUT_PORT::PAUSE, GEN, (unsigned int)Cpd_GEN::IN_PORT::PAUSE);
	AddCoupling(TRACK, (unsigned int)Cpd_TRACK::OUT_PORT::READY, GEN, (unsigned int)Cpd_GEN::IN_PORT::READY);
}

void Cpd_Main::coupTrackProc(WCoupModel* TRACK, WCoupModel* PROC) {
	AddCoupling(PROC, (unsigned int)Cpd_PROC::OUT_PORT::PAUSE, TRACK, (unsigned int)Cpd_TRACK::IN_PORT::PAUSE);
	AddCoupling(PROC, (unsigned int)Cpd_PROC::OUT_PORT::READY, TRACK, (unsigned int)Cpd_TRACK::IN_PORT::READY);
	AddCoupling(TRACK, (unsigned int)Cpd_TRACK::OUT_PORT::PRODUCT, PROC, (unsigned int)Cpd_PROC::IN_PORT::PRODUCT);
}

void Cpd_Main::coupProcTrack(WCoupModel* PROC, WCoupModel* TRACK) {
	AddCoupling(TRACK, (unsigned int)Cpd_TRACK::OUT_PORT::PAUSE, PROC, (unsigned int)Cpd_PROC::IN_PORT::PAUSE);
	AddCoupling(TRACK, (unsigned int)Cpd_TRACK::OUT_PORT::READY, PROC, (unsigned int)Cpd_PROC::IN_PORT::READY);
	AddCoupling(PROC, (unsigned int)Cpd_PROC::OUT_PORT::PRODUCT, TRACK, (unsigned int)Cpd_TRACK::IN_PORT::PRODUCT);
}

void Cpd_Main::coupTrackStock(WCoupModel* TRACK, WCoupModel* STOCK) {
	AddCoupling(STOCK, (unsigned int)Cpd_STOCK::OUT_PORT::PAUSE, TRACK, (unsigned int)Cpd_TRACK::IN_PORT::PAUSE);
	AddCoupling(STOCK, (unsigned int)Cpd_STOCK::OUT_PORT::READY, TRACK, (unsigned int)Cpd_TRACK::IN_PORT::READY);
	AddCoupling(TRACK, (unsigned int)Cpd_TRACK::OUT_PORT::PRODUCT, STOCK, (unsigned int)Cpd_STOCK::IN_PORT::PRODUCT);
}

void Cpd_Main::coupTrackTrack(WCoupModel* TRACK1, WCoupModel* TRACK2) {
	AddCoupling(TRACK2, (unsigned int)Cpd_TRACK::OUT_PORT::PAUSE, TRACK1, (unsigned int)Cpd_TRACK::IN_PORT::PAUSE);
	AddCoupling(TRACK2, (unsigned int)Cpd_TRACK::OUT_PORT::READY, TRACK1, (unsigned int)Cpd_TRACK::IN_PORT::READY);
	AddCoupling(TRACK1, (unsigned int)Cpd_TRACK::OUT_PORT::PRODUCT, TRACK2, (unsigned int)Cpd_TRACK::IN_PORT::PRODUCT);
}