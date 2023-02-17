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
	std::string file_name;
	if (GLOBAL_VAR->scenario_num == 1) {
		file_name = "../../object_state_list1.csv";
	} else if (GLOBAL_VAR->scenario_num == 2) {
		file_name = "../../object_state_list2.csv";
	} else if (GLOBAL_VAR->scenario_num == 3) {
		file_name = "../../object_state_list3.csv";
	}
	m_file.open(file_name);

	m_file << "project_id" << "," << "object_id" << "," << "object_state" << "," << "state_start_time" << "," << "state_end_time" << "\n";
}

void CGlobalVar::CsvStateInsert(int pk, std::string state, double state_start_time, double state_end_time) {
	m_file << scenario_num << "," << pk << "," << state << "," << state_start_time << "," << state_end_time << "\n";
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
