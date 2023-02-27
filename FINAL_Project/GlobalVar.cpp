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
			product = mbuffer->at(idx).at(key).front();
			return product;
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

int CGlobalVar::BufferSize(int key, std::map<int, std::queue<CProduct*>>* _buffer)
{
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _buffer->find(key);
	if (map_find_result != _buffer->end()) {
		return _buffer->at(key).size();
	}
	else return 0;
}

void CGlobalVar::ReadyMapPush(int key, bool mapstate)
{
	std::map<int, bool>::iterator map_find_result = readymap.find(key);
	if (map_find_result != readymap.end()) {
		readymap.at(key)=mapstate;
	}
	else {
		std::pair<int, bool> tmp_pair = std::make_pair(key, mapstate);
		readymap.insert(tmp_pair);
	}
}

bool CGlobalVar::ShowReadyMap(int key)
{
	std::map<int, bool>::iterator map_find_result = readymap.find(key);
	if (map_find_result != readymap.end()) {
		return readymap.at(key);
	}
	else return false;
}


void CGlobalVar::CsvMake() {
	std::ofstream file;
	std::ofstream file1;
	std::ofstream file2;
	std::ofstream file3;
	std::string file_name;
	std::string file_name1;
	std::string file_name2;
	std::string file_name3;

	if (GLOBAL_VAR->scenario_num == 1) {
		file_name = "../../object_state_list1.csv";
		file_name1 = "../../state_time_list1.csv";
		file_name2 = "../../state_rate_list1.csv";
		file_name3 = "../../buf_count_list1.csv";
	} else if (GLOBAL_VAR->scenario_num == 2) {
		file_name = "../../object_state_list2.csv";
		file_name1 = "../../state_time_list2.csv";
		file_name2 = "../../state_rate_list2.csv";
		file_name3 = "../../buf_count_list2.csv";
	} else if (GLOBAL_VAR->scenario_num == 3) {
		file_name = "../../object_state_list3.csv";
		file_name1 = "../../state_time_list3.csv";
		file_name2 = "../../state_rate_list3.csv";
		file_name3 = "../../buf_count_list3.csv";
	}
	m_file.open(file_name);
	m_file1.open(file_name1);
	m_file2.open(file_name2);
	m_file3.open(file_name3);

	m_file << "project_id" << "," << "object_id" << "," << "object_state" << "," << "state_start_time" << "," << "state_end_time" << "\n";
	m_file1 << "project_id" << "," << "object_id" << "," << "current_time" << "," << "init_time" << "," << "active_time" << "," << "error_time" << "," << "pause_time" << "\n";
	m_file2 << "project_id" << "," << "object_id" << "," << "current_time" << "," << "init_rate" << "," << "active_rate" << "," << "error_rate" << "," << "pause_rate" << "\n";
	m_file3 << "project_id" << "," << "object_id" << "," << "object_type" << "," << "current_time" << "," << "buffer_count" << "," << "stock_count" << "\n";
}

void CGlobalVar::CsvStateInsert(int pk, std::string state, double state_start_time, double state_end_time) {
	m_file << scenario_num << "," << pk << "," << state << "," << state_start_time << "," << state_end_time << "\n";
}

void CGlobalVar::CsvStateTimeInsert(int pk, double current_time, double init_time, double active_time, double error_time, double pause_time) {
	m_file1 << scenario_num << "," << pk << "," << current_time << "," << init_time << "," << active_time << "," << error_time << "," << pause_time << "\n";
}

void CGlobalVar::CsvStateRateInsert(int pk, double current_time, double init_time, double active_time, double error_time, double pause_time)
{
	m_file2 << scenario_num << "," << pk << "," << current_time << "," << (double)init_time/current_time*100 << "," << (double)active_time/current_time*100 << "," << (double)error_time/current_time*100 << "," << (double)pause_time/current_time*100 << "\n";
}
void CGlobalVar::CsvBufferSize(int pk, std::string type, double current_time, double buffersize, double stocksize)
{
	m_file3 << scenario_num << "," << pk << "," << type << "," << current_time << "," << buffersize << "," << stocksize << "\n";
}


void CGlobalVar::CsvFileClose() {
	m_file.close();
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
}

void CGlobalVar::Makepgconn()
{
	PostgreSQLConnector* pg_conn = new PostgreSQLConnector();
	pg_conn->ConnectDB();
	pgconn = pg_conn;
}
