#include <WAISERSim.h>
#include <SimLogger.h>
#include "Cpd_Main.h"
#include "json/json.h"
#include "GlobalVar.h"
#include "PostgreSQLConnector.h"

int main()
{
	//try {
	//	Json::Value json_data;
	//	std::ifstream ifs("Param.json");
	//	ifs >> json_data;
	//	GLOBAL_VAR->plan = json_data["PLAN"].asInt();
	//	GLOBAL_VAR->GEN_TIME = json_data["GEN_TIME"].asDouble();
	//	GLOBAL_VAR->seq_1 = json_data["SEQ_1"].asDouble();
	//	GLOBAL_VAR->seq_2 = json_data["SEQ_2"].asDouble();
	//	GLOBAL_VAR->seq_3 = json_data["SEQ_3"].asDouble();
	//	GLOBAL_VAR->yellow = json_data["YELLOW"].asDouble();
	//	GLOBAL_VAR->seq_type = json_data["SEQ_TYPE"].asInt();
	//	GLOBAL_VAR->random = json_data["RANDOM"].asInt();
	//	GLOBAL_VAR->max_gen_car = json_data["MAX_GEN_CAR"].asInt();
	//	GLOBAL_VAR->total = GLOBAL_VAR->seq_1 + GLOBAL_VAR->seq_2 + GLOBAL_VAR->seq_3 + 3 * GLOBAL_VAR->yellow;
	//	if (GLOBAL_VAR->seq_type == 1) {
	//		GLOBAL_VAR->INIT_T = 0;
	//	} else if(GLOBAL_VAR->seq_type == 2) {
	//		GLOBAL_VAR->INIT_T = GLOBAL_VAR->seq_1 + GLOBAL_VAR->yellow;
	//	} else if (GLOBAL_VAR->seq_type == 3) {
	//		if (GLOBAL_VAR->plan == 1) {
	//			GLOBAL_VAR->INIT_T = GLOBAL_VAR->seq_1 + GLOBAL_VAR->seq_2 + 2 * GLOBAL_VAR->yellow;
	//		}else if (GLOBAL_VAR->plan == 1) {
	//			GLOBAL_VAR->INIT_T = GLOBAL_VAR->seq_1 + GLOBAL_VAR->seq_3 + 2 * GLOBAL_VAR->yellow;
	//		}
	//	}
	//} catch (...) {
	//	printf("runtime error");
	//}
	try {
		// 시뮬레이션 기능 활성화
		START_CONSOLELOG;
		//START_ENGINELOG;
		//START_DEFAULT_MODELLOG;
		
		
		WAISER->Reset();
		WAISER->Randomize();
		// CSV파일 생성
		GLOBAL_VAR->CsvMake();
		// SQL 연결
		if (GLOBAL_VAR->SQLConnect == true) {
			GLOBAL_VAR->Makepgconn();
		}
		// 시나리오 번호
		Cpd_Main* model = new Cpd_Main(GLOBAL_VAR->scenario_num);
		WAISER->SetModel(model);
		WAISER->Run(GLOBAL_VAR->time);
		WAISER->Destroy();
		//GLOBAL_VAR->Deletepgconn();
		GLOBAL_VAR->CsvFileClose();

	}
	catch (std::runtime_error& e)
	{
		WAISER->WaitSimulationEnd();
		WAISER->Destroy();
		GLOBAL_VAR->CsvFileClose();
		GLOBAL_VAR->Deletepgconn();
		CLOG->critical("Exception: {}", e.what());

	}
	catch (...)
	{
		WAISER->WaitSimulationEnd();
		WAISER->Destroy();
		GLOBAL_VAR->CsvFileClose();
		GLOBAL_VAR->Deletepgconn();
		CLOG->critical("Unknown exception");
	}

	return 0;
}
