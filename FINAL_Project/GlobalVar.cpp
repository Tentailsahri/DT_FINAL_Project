#include "GlobalVar.h"
#include "SimLogger.h"

CGlobalVar* CGlobalVar::pInstance_ = NULL;

CGlobalVar::CGlobalVar() {
	pgconn = nullptr;

}

CGlobalVar::~CGlobalVar(void) {
	
}

CGlobalVar* CGlobalVar::GetInstance() {
	if (pInstance_ == NULL) {
		pInstance_ = new CGlobalVar();
	}
	return pInstance_;
}

void CGlobalVar::mBufferPush(int idx, int key, CProduct* product, std::map<int, std::map<int, std::queue<CProduct*>>>* mbuffer)
{
	std::map<int, std::map<int, std::queue<CProduct*>>>::iterator map_find_result = mbuffer->find(idx);
	if (map_find_result != mbuffer->end()) {
		std::map<int, std::queue<CProduct*>>::iterator map_find_result1 = mbuffer->at(idx).find(key);
		if (map_find_result1 != mbuffer->at(idx).end()) {
			mbuffer->at(idx).at(key).push(product);
		}
		else {
			std::queue<CProduct*> m_buffer;
			std::pair<int, std::queue<CProduct*>> tmp_pair = std::make_pair(key, m_buffer);
			mbuffer->at(idx).insert(tmp_pair);
			mbuffer->at(idx).at(key).push(product);
		}
	}
	else {
		std::queue<CProduct*> m_buffer;
		std::map<int, std::queue<CProduct*>> _buffer;
		std::pair<int, std::queue<CProduct*>> tmp_pair = std::make_pair(key, m_buffer);
		_buffer.insert(tmp_pair);
		std::pair<int, std::map<int, std::queue<CProduct*>>> tmp_pair2 = std::make_pair(idx, _buffer);
		mbuffer->insert(tmp_pair2);
		mbuffer->at(idx).at(key).push(product);
	}
}

CProduct* CGlobalVar::mBufferPop(int idx, int key, std::map<int, std::map<int, std::queue<CProduct*>>>* mbuffer)
{
	CProduct* product;
	std::map<int, std::map<int, std::queue<CProduct*>>>::iterator map_find_result = mbuffer->find(idx);
	if (map_find_result != mbuffer->end()) {
		std::map<int, std::queue<CProduct*>>::iterator map_find_result1 = mbuffer->at(idx).find(key);
		if (map_find_result1 != mbuffer->at(idx).end()) {
				product = mbuffer->at(idx).at(key).front();
				mbuffer->at(idx).at(key).pop();
				return product;
		}
		else return nullptr;
	}
	else return nullptr;

}

CProduct* CGlobalVar::mBufferFront(int idx, int key, std::map<int, std::map<int, std::queue<CProduct*>>>* mbuffer)
{
	CProduct* product;
	std::map<int, std::map<int, std::queue<CProduct*>>>::iterator map_find_result = mbuffer->find(idx);
	if (map_find_result != mbuffer->end()) {
		std::map<int, std::queue<CProduct*>>::iterator map_find_result1 = mbuffer->at(idx).find(key);
		if (map_find_result1 != mbuffer->at(idx).end()) {
			if (!mbuffer->at(idx).at(key).empty()) {
				product = mbuffer->at(idx).at(key).front();
				return product;
			}
			else return nullptr;
		}
		else return nullptr;
	}
	else return nullptr;
}

int CGlobalVar::mBufferSize(int idx, int key, std::map<int, std::map<int, std::queue<CProduct*>>>* mbuffer)
{

	std::map<int, std::map<int, std::queue<CProduct*>>>::iterator map_find_result = mbuffer->find(idx);
	if (map_find_result != mbuffer->end()) {
		std::map<int, std::queue<CProduct*>>::iterator map_find_result1 = mbuffer->at(idx).find(key);
		if (map_find_result1 != mbuffer->at(idx).end()) {
			return mbuffer->at(idx).at(key).size();

		}
		else return 0;
	}
	else return 0;

}



void CGlobalVar::MapPush(int key, CProduct* product, std::map<int, std::queue<CProduct*>>* _buffer)
{
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _buffer->find(key);
	if (map_find_result != _buffer->end()) {
		_buffer->at(key).push(product);
	}
	else {
		std::queue<CProduct*> m_buffer;
		std::pair<int, std::queue<CProduct*>> tmp_pair = std::make_pair(key, m_buffer);
		_buffer->insert(tmp_pair);
		_buffer->at(key).push(product);
	}
}

CProduct* CGlobalVar::MapPop(int key, std::map<int, std::queue<CProduct*>>* _buffer)
{
	CProduct* product;
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _buffer->find(key);
	if (map_find_result != _buffer->end() && _buffer->at(key).size() != 0) {
		product = _buffer->at(key).front();
		_buffer->at(key).pop();
		return product;
	}
	else return nullptr;

}

CProduct* CGlobalVar::MapFront(int key, std::map<int, std::queue<CProduct*>>* _buffer)
{
	CProduct* product;
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _buffer->find(key);
	if (map_find_result != _buffer->end() && _buffer->at(key).size() != 0) {
		product = _buffer->at(key).front();
		return product;
	}
	else return nullptr;
}

CProduct* CGlobalVar::StockBack(int key, std::map<int, std::queue<CProduct*>>* _stock)
{
	CProduct* product;
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _stock->find(key);
	if (map_find_result != _stock->end() && _stock->at(key).size() != 0) {
		product = _stock->at(key).back();
		return product;
	}
	else return nullptr;

}

void CGlobalVar::m_subIdxMapUpdate(int pk) {
	std::map<int, int>::iterator map_find_result = m_subIdxMap.find(pk);
	if (map_find_result != m_subIdxMap.end()) {
		int a = m_subIdxMap.at(pk);
		m_subIdxMap.at(pk) = ++a;
	} else {
		std::pair<int, int> tmp_pair = std::make_pair(pk, 0);
		m_subIdxMap.insert(tmp_pair);
	}
}

int CGlobalVar::BufferSize(int key, std::map<int, std::queue<CProduct*>>* _buffer)
{
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _buffer->find(key);
	if (map_find_result != _buffer->end()) {
		return _buffer->at(key).size();
	}
	else return 0;
}



void CGlobalVar::CsvMake() {
	std::ofstream file2;
	std::ofstream file3;
	std::ofstream file4;
	std::ofstream file5;
	std::ofstream file6;
	std::ofstream file8;
	std::string file_name2;
	std::string file_name3;
	std::string file_name4;
	std::string file_name5;
	std::string file_name6;
	std::string file_name8;

	file_name2 = "../CSV/state_time_list" + std::to_string(scenario_num) + ".csv";
	file_name3 = "../CSV/state_rate_list" + std::to_string(scenario_num) + ".csv";
	file_name4 = "../CSV/buf_count_list" + std::to_string(scenario_num) + ".csv";
	file_name5 = "../CSV/product_flow_list" + std::to_string(scenario_num) + ".csv";
	file_name6 = "../CSV/object_state_list" + std::to_string(scenario_num) + ".csv";
	file_name8 = "../CSV/result_data" + std::to_string(scenario_num) + ".csv";

	m_file2.open(file_name2);
	m_file3.open(file_name3);
	m_file4.open(file_name4);
	m_file5.open(file_name5);
	m_file6.open(file_name6);
	m_file8.open(file_name8);

	m_file2 << "project_id" << "," << "object_id" << "," << "current_time" << "," << "init_time" << "," << "active_time" << "," << "error_time" << "," << "pause_time" << "\n";
	m_file3 << "project_id" << "," << "object_id" << "," << "current_time" << "," << "init_rate" << "," << "active_rate" << "," << "error_rate" << "," << "pause_rate" << "\n";
	m_file4 << "project_id" << "," << "object_id" << "," << "object_type" << "," << "current_time" << "," << "buffer_count" << "," << "stock_count" << "\n";
	m_file5 << "project_id" << "," << "object_id" << "," << "product_id" << "," << "in_time" << "," << "out_time" << "\n";
	m_file6 << "project_id" << "," << "object_id" << "," << "object_state" << "," << "state_start_time" << "," << "state_end_time" << "\n";

}

void CGlobalVar::CsvStateInsert(int pk, std::string state, double state_start_time, double state_end_time) {
	m_file6 << scenario_num << "," << pk << "," << state << "," << state_start_time << "," << state_end_time << "\n";
}

void CGlobalVar::CsvStateTimeInsert(int pk, double current_time, double init_time, double active_time, double error_time, double pause_time) {
	m_file2 << scenario_num << "," << pk << "," << current_time << "," << init_time << "," << active_time << "," << error_time << "," << pause_time << "\n";
}

void CGlobalVar::CsvStateRateInsert(int pk, double current_time, double init_time, double active_time, double error_time, double pause_time)
{
	m_file3 << scenario_num << "," << pk << "," << current_time << "," << (double)init_time / current_time * 100 << "," << (double)active_time / current_time * 100 << "," << (double)error_time / current_time * 100 << "," << (double)pause_time / current_time * 100 << "\n";
}
void CGlobalVar::CsvBufferSize(int pk, std::string state, double current_time, double buffersize, double stocksize)
{
	m_file4 << scenario_num << "," << pk << "," << state << "," << current_time << "," << buffersize << "," << stocksize << "\n";
}
void CGlobalVar::CsvProductFlowList(int pk, int product_id, double in_time, double out_time)
{
	m_file5 << scenario_num << "," << pk << "," << product_id << "," << in_time << "," << out_time << "\n";
}

void CGlobalVar::CsvResultData() {

	avgTimeStore[0] = avgTimeStore[0] / allCount;
	avgTimeStore[1] = avgTimeStore[1] / allCount;
	avgTimeStore[2] = avgTimeStore[2] / allCount;
	avgTimeStore[3] = avgTimeStore[3] / allCount;
	m_file8 << "Scenario Num" << "," << "Time" << "," << "GEN Count" << "," << "TRACK Count"  "," << "PROC Count"  "," << "STOCK Count" << "ALL Count" << "\n";
	m_file8 << scenario_num << "," << time << "," << m_genCount << "," << m_trackCount << "," << m_procCount <<  "," << m_stockCount << "," << allCount << "\n\n";
	m_file8 << "State Avg Time" << "\n";
	m_file8 << "INIT TIME" << "," << "ACTIVE TIME" << "," << "ERROR TIME" << "," << "PAUSE TIME" << "\n";
	m_file8 << avgTimeStore[0] << "," << avgTimeStore[1] << "," << avgTimeStore[2] << "," << avgTimeStore[3] << "\n\n";

	m_file8 << "State Avg Rate" << "\n";
	m_file8 << "INIT TIME" << "," << "ACTIVE TIME" << "," << "ERROR TIME" << "," << "PAUSE TIME" << "\n";
	m_file8 << avgTimeStore[0] * 100 / time << "," << avgTimeStore[1] * 100 / time << "," << avgTimeStore[2] * 100 / time << "," << avgTimeStore[3] * 100 / time << "\n\n";

	m_file8 << "Product" << "\n";
	m_file8 << "All Product Count" << "," << "All Product Time Sum" << "," << "All Product Time Avg" << "\n";
	m_file8 << GLOBAL_VAR->m_sumPass << "," << GLOBAL_VAR->m_sumPassTime << "," << GLOBAL_VAR->m_sumPassTime / GLOBAL_VAR->m_sumPass << "\n\n";
	CLOG->info("�� ��� ���� : {} �� ����ð� : {} �������ð� : {}", GLOBAL_VAR->m_sumPass, GLOBAL_VAR->m_sumPassTime, GLOBAL_VAR->m_sumPassTime / GLOBAL_VAR->m_sumPass);

	m_file8 << "INIT TIME" << "\n";
	m_file8 << "GEN INIT" << "," << "TRACK INIT" << "," << "PROC INIT" << "," << "STOCK INIT" << "\n";
	m_file8 << GLOBAL_VAR->TA_STATE_INIT[0] << "," << GLOBAL_VAR->TA_STATE_INIT[1] << "," << GLOBAL_VAR->TA_STATE_INIT[2] << "," << GLOBAL_VAR->TA_STATE_INIT[3] << "\n\n";
}


void CGlobalVar::CsvFileClose() {
	m_file2.close();
	m_file3.close();
	m_file4.close();
	m_file5.close();
	m_file6.close();
	m_file8.close();
}

void CGlobalVar::Deletepgconn()
{
	delete pgconn;
}

void CGlobalVar::ResetTable()
{
	pgconn->SendQuery("CREATE TABLE IF NOT EXISTS public.object_state_list" + std::to_string(scenario_num) + \
		"(project_id integer,	object_id integer, object_state character varying(50) COLLATE pg_catalog.\"default\",	\
		state_start_time double precision, \
		state_end_time double precision, \
		CONSTRAINT object_state_list" + std::to_string(scenario_num) + \
		"_object_id_fk FOREIGN KEY(object_id)	\
		REFERENCES public.object_list" + std::to_string(scenario_num) + \
		"(object_id)MATCH SIMPLE	\
		ON UPDATE NO ACTION	\
		ON DELETE NO ACTION, \
		CONSTRAINT object_state_list" + std::to_string(scenario_num) + \
		"_project_id_fk FOREIGN KEY(project_id)	\
		REFERENCES public.project_list" + std::to_string(scenario_num) + \
		"(project_id)MATCH SIMPLE	\
		ON UPDATE NO ACTION	\
		ON DELETE NO ACTION	\
	)");
	pgconn->SendQuery("TRUNCATE TABLE \"object_state_list" + std::to_string(scenario_num) + "\"");

	pgconn->SendQuery("CREATE TABLE IF NOT EXISTS public.buf_count_list" + std::to_string(scenario_num) + \
		"(project_id integer, object_id integer, object_type character varying(50) COLLATE pg_catalog.\"default\", \
		\"current_time\" double precision, \
		buffer_count integer, \
		stock_count integer, \
		CONSTRAINT buf_count_list" + std::to_string(scenario_num) + \
		"_object_id_fk FOREIGN KEY(object_id) \
		REFERENCES public.object_list" + std::to_string(scenario_num) + \
		"(object_id) MATCH SIMPLE \
		ON UPDATE NO ACTION \
		ON DELETE NO ACTION, \
		CONSTRAINT buf_count_list" + std::to_string(scenario_num) + \
		"_project_id_fk FOREIGN KEY(project_id) \
		REFERENCES public.project_list" + std::to_string(scenario_num) + \
		"(project_id)MATCH SIMPLE \
		ON UPDATE NO ACTION \
		ON DELETE NO ACTION \
	)");
	pgconn->SendQuery("TRUNCATE TABLE \"buf_count_list" + std::to_string(scenario_num) + "\"");

	pgconn->SendQuery("CREATE TABLE IF NOT EXISTS public.state_time_list" + std::to_string(scenario_num) + \
		"(project_id integer, object_id integer, \"current_time\" double precision, \
		init_time double precision, \
		active_time double precision, \
		error_time double precision, \
		wait_time double precision, \
		CONSTRAINT state_time_list" + std::to_string(scenario_num) + \
		"_object_id_fk FOREIGN KEY(object_id) \
		REFERENCES public.object_list" + std::to_string(scenario_num) + \
		"(object_id)MATCH SIMPLE \
		ON UPDATE NO ACTION \
		ON DELETE NO ACTION, \
		CONSTRAINT state_time_list" + std::to_string(scenario_num) + \
		"_project_id_fk FOREIGN KEY(project_id) \
		REFERENCES public.project_list" + std::to_string(scenario_num) + \
		"(project_id)MATCH SIMPLE \
		ON UPDATE NO ACTION \
		ON DELETE NO ACTION \
	)");
	pgconn->SendQuery("TRUNCATE TABLE \"state_time_list" + std::to_string(scenario_num) + "\"");

	pgconn->SendQuery("CREATE TABLE IF NOT EXISTS public.state_rate_list" + std::to_string(scenario_num) + \
		"(project_id integer, object_id integer, \"current_time\" double precision, \
		init_rate double precision, \
		active_rate double precision, \
		error_rate double precision, \
		wait_rate double precision, \
		CONSTRAINT state_rate_list" + std::to_string(scenario_num) + \
		"_object_id_fk FOREIGN KEY(object_id) \
		REFERENCES public.object_list" + std::to_string(scenario_num) + \
		"(object_id)MATCH SIMPLE \
		ON UPDATE NO ACTION \
		ON DELETE NO ACTION, \
		CONSTRAINT state_rate_list" + std::to_string(scenario_num) + \
		"_project_id_fk FOREIGN KEY(project_id) \
		REFERENCES public.project_list" + std::to_string(scenario_num) + \
		"(project_id)MATCH SIMPLE \
		ON UPDATE NO ACTION \
		ON DELETE NO ACTION \
	)");
	pgconn->SendQuery("TRUNCATE TABLE \"state_rate_list" + std::to_string(scenario_num) + "\"");

	pgconn->SendQuery("CREATE TABLE IF NOT EXISTS public.product_flow_list" + std::to_string(scenario_num) + \
		"(project_id integer, object_id integer, product_id integer, in_time double precision, \
		out_time double precision, \
		CONSTRAINT product_flow_list" + std::to_string(scenario_num) + \
		"_object_id_fk FOREIGN KEY(object_id) \
		REFERENCES public.object_list" + std::to_string(scenario_num) + \
		"(object_id) MATCH SIMPLE \
		ON UPDATE NO ACTION \
		ON DELETE NO ACTION, \
		CONSTRAINT product_flow_list" + std::to_string(scenario_num) + \
		"_project_id_fk FOREIGN KEY(project_id) \
		REFERENCES public.project_list" + std::to_string(scenario_num) + \
		"(project_id) MATCH SIMPLE \
		ON UPDATE NO ACTION \
		ON DELETE NO ACTION \
	)");
	pgconn->SendQuery("TRUNCATE TABLE \"product_flow_list" + std::to_string(scenario_num) + "\"");
}

void CGlobalVar::Makepgconn()
{
	PostgreSQLConnector* pg_conn = new PostgreSQLConnector();
	pg_conn->ConnectDB();
	pgconn = pg_conn;
}
