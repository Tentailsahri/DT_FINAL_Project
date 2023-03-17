#include "Cpd_Main.h"

Cpd_Main::Cpd_Main(int scenario_num)
{
	SetName("MainCouple");// 모델 이름 지정

	if (scenario_num == 1) {
		
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='GEN'");
		int genCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='TRACK'");
		int trackCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='PROC'");
		int procCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='STOCK'");
		int stocCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());

		// 생성한 모델 연결
		for (int i = 0; i < genCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i, new Cpd_GEN(i, i));
			gen_cpd_map.insert(tmp_pair);
			AddComponent(gen_cpd_map.at(i));
		}
		for (int i = 0; i < trackCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount, new Cpd_TRACK(i, i + genCount));
			track_cpd_map.insert(tmp_pair);
			AddComponent(track_cpd_map.at(i + genCount));
		}
		for (int i = 0; i < procCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount + trackCount, new Cpd_PROC(i, i + genCount + trackCount));
			proc_cpd_map.insert(tmp_pair);
			AddComponent(proc_cpd_map.at(i + genCount + trackCount));
		}
		for (int i = 0; i < stocCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount + trackCount + stocCount, new Cpd_STOCK(i, i + genCount + trackCount + stocCount));
			stock_cpd_map.insert(tmp_pair);
			AddComponent(stock_cpd_map.at(i + genCount + trackCount + stocCount));
		}

		// 모델 포트 연결
		coup(0, "GEN", 1, "TRACK");
		coup(1, "TRACK", 3, "PROC");
		coup(3, "PROC", 2, "TRACK");
		coup(2, "TRACK", 4, "STOCK");
	}
	else if (scenario_num == 2) {
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='GEN'");
		int genCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='TRACK'");
		int trackCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='PROC'");
		int procCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='STOCK'");
		int stocCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());

		// 생성한 모델 연결
		for (int i = 0; i < genCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i, new Cpd_GEN(i, i));
			gen_cpd_map.insert(tmp_pair);
			AddComponent(gen_cpd_map.at(i));
		}
		for (int i = 0; i < trackCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount, new Cpd_TRACK(i, i + genCount));
			track_cpd_map.insert(tmp_pair);
			AddComponent(track_cpd_map.at(i + genCount));
		}
		for (int i = 0; i < procCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount + trackCount, new Cpd_PROC(i, 2, i + genCount + trackCount));
			proc_cpd_map.insert(tmp_pair);
			AddComponent(proc_cpd_map.at(i + genCount + trackCount));
		}
		for (int i = 0; i < stocCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount + trackCount + stocCount, new Cpd_STOCK(i, i + genCount + trackCount + stocCount));
			stock_cpd_map.insert(tmp_pair);
			AddComponent(stock_cpd_map.at(i + genCount + trackCount + stocCount));
		}

		// 모델 포트 연결
		coup(0, "GEN", 3, "TRACK");
		coup(1, "GEN", 4, "TRACK");
		coups(0, 3, "TRACK", 9, "PROC");
		coups(1, 4, "TRACK", 9, "PROC");
		coup(9, "PROC", 5, "TRACK");
		coups(0, 5, "TRACK", 10, "PROC");
		coup(2, "GEN", 6, "TRACK");
		coups(1, 6, "TRACK", 10, "PROC");
		coup(10, "PROC", 7, "TRACK");
		coup(10, "PROC", 8, "TRACK");
		coup(7, "TRACK", 11, "STOCK");
		coup(8, "TRACK", 12, "STOCK");
	}
	else if (scenario_num == 3) {
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='GEN'");
		int genCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='TRACK'");
		int trackCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='PROC'");
		int procCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='STOCK'");
		int stocCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		

		// 생성한 모델 연결
		for (int i = 0; i < genCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i, new Cpd_GEN(i, i));
			gen_cpd_map.insert(tmp_pair);
			AddComponent(gen_cpd_map.at(i));
		}
		for (int i = 0; i < trackCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount, new Cpd_TRACK(i, i + genCount));
			track_cpd_map.insert(tmp_pair);
			AddComponent(track_cpd_map.at(i + genCount));
		}
		for (int i = 0; i < procCount; i++) {
			GLOBAL_VAR->pgconn->SendQuery("SELECT send_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE receive_object_id=" + std::to_string(26+i));
			bufferPopNum = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount + trackCount, new Cpd_PROC(i, bufferPopNum, i + genCount + trackCount));
			proc_cpd_map.insert(tmp_pair);
			AddComponent(proc_cpd_map.at(i + genCount + trackCount));
		}
		for (int i = 0; i < stocCount; i++) {
			std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount + trackCount + procCount, new Cpd_STOCK(i, i + genCount + trackCount + procCount));
			stock_cpd_map.insert(tmp_pair);
			AddComponent(stock_cpd_map.at(i + genCount + trackCount + procCount));
		}

		// 모델 포트 연결
		coup(0, "GEN", 7, "TRACK");
		coup(1, "GEN", 9, "TRACK");
		coup(2, "GEN", 10, "TRACK");
		coup(3, "GEN", 11, "TRACK");
		coup(4, "GEN", 13, "TRACK");
		coup(5, "GEN", 22, "TRACK");
		coup(6, "GEN", 23, "TRACK");
		coup(7, "TRACK", 8, "TRACK");
		coup(11, "TRACK", 12, "TRACK");
		coup(18, "TRACK", 19, "TRACK");
		coup(14, "TRACK", 15, "TRACK");
		coup(15, "TRACK", 16, "TRACK");
		coups(0, 8, "TRACK", 26, "PROC");
		coups(1, 9, "TRACK", 26, "PROC");
		coups(2, 10, "TRACK", 26, "PROC");
		coups(3, 12, "TRACK", 26, "PROC");
		coups(0, 17, "TRACK", 28, "PROC");
		coups(1, 19, "TRACK", 28, "PROC");
		coups(0, 16, "TRACK", 29, "PROC");
		coups(1, 20, "TRACK", 29, "PROC");
		coups(2, 21, "TRACK", 29, "PROC");
		coups(0, 22, "TRACK", 30, "PROC");
		coups(1, 23, "TRACK", 30, "PROC");
		coup(13, "TRACK", 27, "PROC");
		coup(26, "PROC", 14, "TRACK");
		coup(26, "PROC", 17, "TRACK");
		coup(27, "PROC", 18, "TRACK");
		coup(28, "PROC", 20, "TRACK");
		coup(29, "PROC", 24, "TRACK");
		coup(29, "PROC", 25, "TRACK");
		coup(30, "PROC", 21, "TRACK");
		coup(24, "TRACK", 31, "STOCK");
		coup(25, "TRACK", 32, "STOCK");
	}
}

void Cpd_Main::coup(int outPk, std::string outType, int inPk, std::string inType) {
	if (outType == "GEN" && inType == "TRACK")
		coupGenTrack(gen_cpd_map.at(outPk), track_cpd_map.at(inPk));
	else if (outType == "TRACK" && inType == "PROC")
		coupTrackProc(track_cpd_map.at(outPk), proc_cpd_map.at(inPk));
	else if (outType == "PROC" && inType == "TRACK")
		coupProcTrack(proc_cpd_map.at(outPk), track_cpd_map.at(inPk));
	else if (outType == "TRACK" && inType == "STOCK")
		coupTrackStock(track_cpd_map.at(outPk), stock_cpd_map.at(inPk));
	else if (outType == "TRACK" && inType == "TRACK")
		coupTrackTrack(track_cpd_map.at(outPk), track_cpd_map.at(inPk));
}

void Cpd_Main::coups(int num, int outPk, std::string outType, int inPk, std::string inType) {
	if (outType == "TRACK" && inType == "PROC")
		coupTrackProc(num, track_cpd_map.at(outPk), proc_cpd_map.at(inPk));
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