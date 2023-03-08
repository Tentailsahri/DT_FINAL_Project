#include "Cpd_Main.h"

Cpd_Main::Cpd_Main(int scenario_num)
{
	SetName("MainCouple");// 모델 이름 지정

	if (scenario_num == 1) {
		// 모델 생성
		WCoupModel* gen = new Cpd_GEN(0,0);
		WCoupModel* track0 = new Cpd_TRACK(0,1);
		WCoupModel* track1 = new Cpd_TRACK(1,2);
		WCoupModel* proc = new Cpd_PROC(0, 3);
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

	} 
	else if (scenario_num == 2) {
		// 모델 생성
		std::vector<WCoupModel*> gen_cpd_vec;
		std::vector<WCoupModel*> track_cpd_vec;
		std::vector<WCoupModel*> proc_cpd_vec;
		std::vector<WCoupModel*> stock_cpd_vec;

		int genCount = 3;
		int trackCount = 6;
		int procCount = 2;
		int stocCount = 2;

		// 생성한 모델 연결
		for (int i = 0; i < genCount; i++) {
			gen_cpd_vec.push_back(new Cpd_GEN(i, i));
			AddComponent(gen_cpd_vec.at(i));
		}
		for (int i = 0; i < trackCount; i++) {
			track_cpd_vec.push_back(new Cpd_TRACK(i, i + genCount));
			AddComponent(track_cpd_vec.at(i));
		}
		for (int i = 0; i < procCount; i++) {
		    proc_cpd_vec.push_back(new Cpd_PROC(i, 2, i + genCount + trackCount));
			AddComponent(proc_cpd_vec.at(i));
		}
		for (int i = 0; i < stocCount; i++) {
			stock_cpd_vec.push_back(new Cpd_STOCK(i, i + genCount + trackCount + stocCount));
			AddComponent(stock_cpd_vec.at(i));
		}
		
		// 모델 포트 연결
		coupGenTrack(gen_cpd_vec.at(0), track_cpd_vec.at(0));
		coupGenTrack(gen_cpd_vec.at(1), track_cpd_vec.at(1));
		for (int i = 0; i < 2; i++) {
			coupTrackProc(i, track_cpd_vec.at(i), proc_cpd_vec.at(0));
		}
		for (int i = 0; i < 2; i++) {
			coupTrackProc(i, track_cpd_vec.at(2 + i), proc_cpd_vec.at(1));
		}
		coupProcTrack(proc_cpd_vec.at(0), track_cpd_vec.at(2));
		coupGenTrack(gen_cpd_vec.at(2), track_cpd_vec.at(3));
		coupProcTrack(proc_cpd_vec.at(1), track_cpd_vec.at(4));
		coupProcTrack(proc_cpd_vec.at(1), track_cpd_vec.at(5));
		coupTrackStock(track_cpd_vec.at(4), stock_cpd_vec.at(0));
		coupTrackStock(track_cpd_vec.at(5), stock_cpd_vec.at(1));
	} 
	else if (scenario_num == 3) {

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

void Cpd_Main::coupTrackProc(int num, WCoupModel* TRACK, WCoupModel* PROC) {
	AddCoupling(PROC, (unsigned int)Cpd_PROC::OUT_PORT::PAUSE+num, TRACK, (unsigned int)Cpd_TRACK::IN_PORT::PAUSE);
	AddCoupling(PROC, (unsigned int)Cpd_PROC::OUT_PORT::READY+num, TRACK, (unsigned int)Cpd_TRACK::IN_PORT::READY);
	AddCoupling(TRACK, (unsigned int)Cpd_TRACK::OUT_PORT::PRODUCT, PROC, (unsigned int)Cpd_PROC::IN_PORT::PRODUCT+num);
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