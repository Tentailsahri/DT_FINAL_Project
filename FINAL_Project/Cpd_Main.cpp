#include "Cpd_Main.h"

Cpd_Main::Cpd_Main(int scenario_num)
{
	SetName("MainCouple");// 모델 이름 지정

	GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='GEN'");
	int genCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
	GLOBAL_VAR->m_genCount = genCount;
	GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='TRACK'");
	int trackCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
	GLOBAL_VAR->m_trackCount = trackCount;
	GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='PROC'");
	int procCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
	GLOBAL_VAR->m_procCount = procCount;
	GLOBAL_VAR->pgconn->SendQuery("SELECT * FROM \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE object_type='STOCK'");
	int stockCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
	GLOBAL_VAR->m_stockCount = stockCount;

	GLOBAL_VAR->allCount = genCount + trackCount + procCount + stockCount;

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
		GLOBAL_VAR->pgconn->SendQuery("SELECT send_object_id FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" WHERE receive_object_id=" + std::to_string(genCount + trackCount + i));
		bufferPopNum = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount + trackCount, new Cpd_PROC(i, bufferPopNum, i + genCount + trackCount));
		proc_cpd_map.insert(tmp_pair);
		AddComponent(proc_cpd_map.at(i + genCount + trackCount));
	}
	for (int i = 0; i < stockCount; i++) {
		std::pair<int, WCoupModel*> tmp_pair = std::make_pair(i + genCount + trackCount + procCount, new Cpd_STOCK(i, i + genCount + trackCount + procCount));
		stock_cpd_map.insert(tmp_pair);
		AddComponent(stock_cpd_map.at(i + genCount + trackCount + procCount));
	}


	GLOBAL_VAR->pgconn->SendQuery("SELECT p.send_object_id, send_obj.object_type, p.receive_object_id, receive_obj.object_type FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" AS p LEFT JOIN \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" AS send_obj ON p.send_object_id = send_obj.object_id LEFT JOIN \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" AS receive_obj ON p.receive_object_id = receive_obj.object_id");
		tuplesCount = PQntuples(GLOBAL_VAR->pgconn->GetSQLResult());
		for (int i = 0; i < tuplesCount; i++) {
			GLOBAL_VAR->pgconn->SendQuery("SELECT p.send_object_id, send_obj.object_type, p.receive_object_id, receive_obj.object_type FROM \"obj_coup_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" AS p LEFT JOIN \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" AS send_obj ON p.send_object_id = send_obj.object_id LEFT JOIN \"object_list" + std::to_string(GLOBAL_VAR->scenario_num) + "\" AS receive_obj ON p.receive_object_id = receive_obj.object_id");
			sendNum = std::stoi(PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 0));
			sendType = PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 1);
			receiveNum = std::stoi(PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 2));
			receiveType = PQgetvalue(GLOBAL_VAR->pgconn->GetSQLResult(), i, 3);
			pushMap(receiveNum, sendNum);
			subIdx = receiveNumMap.at(receiveNum).size() - 1;
			coups(subIdx, sendNum, sendType, receiveNum, receiveType);
		}
	
}

void Cpd_Main::pushMap(int receive, int send)
{
	std::map<int, std::queue<int>>::iterator map_find_result = receiveNumMap.find(receive);
	if (map_find_result != receiveNumMap.end()) {
		receiveNumMap.at(receive).push(send);
	}
	else {
		std::queue<int> receivequeue;
		std::pair<int, std::queue<int>> tmp_pair = std::make_pair(receive, receivequeue);
		receiveNumMap.insert(tmp_pair);
		receiveNumMap.at(receive).push(send);
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
	if (outType == "GEN" && inType == "TRACK")
		coupGenTrack(gen_cpd_map.at(outPk), track_cpd_map.at(inPk));
	if (outType == "TRACK" && inType == "PROC")
		coupTrackProc(num, track_cpd_map.at(outPk), proc_cpd_map.at(inPk));
	else if (outType == "PROC" && inType == "TRACK")
		coupProcTrack(proc_cpd_map.at(outPk), track_cpd_map.at(inPk));
	else if (outType == "TRACK" && inType == "STOCK")
		coupTrackStock(track_cpd_map.at(outPk), stock_cpd_map.at(inPk));
	else if (outType == "TRACK" && inType == "TRACK")
		coupTrackTrack(track_cpd_map.at(outPk), track_cpd_map.at(inPk));
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