#pragma once
#include "WAISERSim.h"
#include "CProduct.h"
#include "PostgreSQLConnector.h"

class CGlobalVar {
private:
	static CGlobalVar* pInstance_;
protected:
	CGlobalVar();
public:
	~CGlobalVar(void);
	static CGlobalVar* GetInstance();

	
	std::map<int, std::queue<CProduct*>> buffer;
	std::map<int, std::queue<CProduct*>> procbuffer;
	std::map<int, std::queue<CProduct*>> stock;

	void pushmap(int key, CProduct* product, std::map<int, std::queue<CProduct*>> *_buffer);
	CProduct* popmap(int key, std::map<int, std::queue<CProduct*>> *_buffer);
	CProduct* stockback(int key, std::map<int, std::queue<CProduct*>> *_stock);
	CProduct* frontmap(int key, std::map<int, std::queue<CProduct*>>* _buffer);
	int buffer_size(int key, std::map<int, std::queue<CProduct*>> *_buffer);

	PostgreSQLConnector* pgconn;

	// CSV 출력 관련
	void CsvMake();
	void CsvStateInsert(int pk, std::string state, double state_start_time, double state_end_time);
	void CsvFileClose();
	
	// SQL 연결 관련
	void Makepgconn();
	void Deletepgconn();

	std::ofstream m_file;

	int m_maxbuffer_Generator = 10;
	int m_maxbuffer_Receive = 10;
	int m_maxbuffer_Process = 10;
	int m_maxbuffer_Stock = 10;
	int	scenario_num = 1;
	int track_speed = 1;

	// 공정 오류율(x회마다 고장)
	int error_gen = 5;
	int error_proc = 5;
	int error_stock = 5;
	int time = 100;

	bool SQLConnect = false;

	double TA_STATE_INIT[4] = { 5,5,5,5 };

};

#define GLOBAL_VAR	(CGlobalVar::GetInstance())