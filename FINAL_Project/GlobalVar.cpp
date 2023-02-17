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

void CGlobalVar::pushmap(int key, CProduct* product, std::map<int, std::queue<CProduct*>> *_buffer)
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

CProduct* CGlobalVar::popmap(int key, std::map<int, std::queue<CProduct*>> *_buffer)
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

CProduct* CGlobalVar::frontmap(int key, std::map<int, std::queue<CProduct*>>* _buffer)
{
	CProduct* product;
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _buffer->find(key);
	if (map_find_result != _buffer->end() && _buffer->at(key).size() != 0) {
		product = _buffer->at(key).front();
		return product;
	}
	else return nullptr;
}

CProduct* CGlobalVar::stockback(int key, std::map<int, std::queue<CProduct*>> *_stock)
{
	CProduct* product;
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _stock->find(key);
	if (map_find_result != _stock->end() && _stock->at(key).size() != 0) {
		product = _stock->at(key).back();
		return product;
	}
	else return nullptr;
	
}

int CGlobalVar::buffer_size(int key, std::map<int, std::queue<CProduct*>> *_buffer)
{
	std::map<int, std::queue<CProduct*>>::iterator map_find_result = _buffer->find(key);
	if (map_find_result != _buffer->end()) {
		return _buffer->at(key).size();
	}
	else return 0;
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
		file_name1= "../../state_time_list1.csv";
		file_name2= "../../state_rate_list1.csv";
		file_name3= "../../buf_count_list1.csv";
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
	m_file1 << scenario_num << "," << pk << "," << current_time << "," << init_time << "," << active_time << ","<< error_time << "," << pause_time << "\n";
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

void CGlobalVar::Makepgconn()
{
	PostgreSQLConnector* pg_conn = new PostgreSQLConnector();
	pg_conn->ConnectDB();
	pg_conn->SendQuery("TRUNCATE TABLE \"object_state_list" + std::to_string(scenario_num) + "\"");
	pgconn = pg_conn;
}
