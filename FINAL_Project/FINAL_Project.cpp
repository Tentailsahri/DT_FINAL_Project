#include <WAISERSim.h>
#include <SimLogger.h>
#include "Cpd_Main.h"
#include "json/json.h"
#include "GlobalVar.h"
#include "PostgreSQLConnector.h"

int main()
{
	try {
		Json::Value json_data;
		std::ifstream ifs("Param.json");
		ifs >> json_data;
		GLOBAL_VAR->SQLConnect = json_data["SQLConnect"].asBool();
		GLOBAL_VAR->scenario_num = json_data["scenario_num"].asInt();
		GLOBAL_VAR->time = json_data["time"].asInt();
		GLOBAL_VAR->TA_STATE_INIT[0] = json_data["TA_STATE_INIT[0]"].asDouble();
		GLOBAL_VAR->TA_STATE_INIT[1] = json_data["TA_STATE_INIT[1]"].asDouble();
		GLOBAL_VAR->TA_STATE_INIT[2] = json_data["TA_STATE_INIT[2]"].asDouble();
		GLOBAL_VAR->TA_STATE_INIT[3] = json_data["TA_STATE_INIT[3]"].asDouble();
		GLOBAL_VAR->TA_STATE_ACTIVE[0] = json_data["TA_STATE_ACTIVE[0]"].asDouble();
		GLOBAL_VAR->TA_STATE_ACTIVE[2] = json_data["TA_STATE_ACTIVE[2]"].asDouble();
		GLOBAL_VAR->TA_STATE_ACTIVE[3] = json_data["TA_STATE_ACTIVE[3]"].asDouble();
		GLOBAL_VAR->TA_STATE_ERROR[0] = json_data["TA_STATE_ERROR[0]"].asDouble();
		GLOBAL_VAR->TA_STATE_ERROR[2] = json_data["TA_STATE_ERROR[2]"].asDouble();
		GLOBAL_VAR->TA_STATE_ERROR[3] = json_data["TA_STATE_ERROR[3]"].asDouble();
		GLOBAL_VAR->m_maxbuffer[0] = json_data["m_maxbuffer_Generator"].asInt();
		GLOBAL_VAR->m_maxbuffer[1] = json_data["m_maxbuffer_Track"].asInt();
		GLOBAL_VAR->m_maxbuffer[2] = json_data["m_maxbuffer_Process"].asInt();
		GLOBAL_VAR->m_maxbuffer[3] = json_data["m_maxbuffer_Stock"].asInt();
	} catch (...) {
		printf("runtime error");
	}
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
			GLOBAL_VAR->ResetTable();
		}
		// 시나리오 번호
		Cpd_Main* model = new Cpd_Main(GLOBAL_VAR->scenario_num);
		WAISER->SetModel(model);
		WAISER->Run(GLOBAL_VAR->time);
		WAISER->Destroy();
		if (GLOBAL_VAR->SQLConnect == true) {
			GLOBAL_VAR->Deletepgconn();
		}
		GLOBAL_VAR->CsvFileClose();
		WAISER->WaitSimulationEnd();
	}
	catch (std::runtime_error& e)
	{
		WAISER->WaitSimulationEnd();
		WAISER->Destroy();
		GLOBAL_VAR->CsvFileClose();
		if (GLOBAL_VAR->SQLConnect == true) {
			GLOBAL_VAR->Deletepgconn();
		}
		CLOG->critical("Exception: {}", e.what());
	}
	catch (...)
	{
		WAISER->WaitSimulationEnd();
		WAISER->Destroy();
		GLOBAL_VAR->CsvFileClose();
		if (GLOBAL_VAR->SQLConnect == true) {
			GLOBAL_VAR->Deletepgconn();
		}
		CLOG->critical("Unknown exception");
	}

	return 0;
}
