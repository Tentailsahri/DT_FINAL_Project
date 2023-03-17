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

	std::map<int, std::map<int, std::queue<CProduct*>>> p_buffer;

	std::map<int, std::vector<bool>> readymap;

	void mBufferPush(int idx, int key, CProduct* product, std::map<int, std::map<int, std::queue<CProduct*>>>* mbuffer);
	CProduct* mBufferPop(int idx, int key, std::map<int, std::map<int, std::queue<CProduct*>>>* mbuffer);
	CProduct* mBufferFront(int idx, int key, std::map<int, std::map<int, std::queue<CProduct*>>>* mbuffer);
	int mBufferSize(int idx, int key, std::map<int, std::map<int, std::queue<CProduct*>>>* mbuffer);
	void MapPush(int key, CProduct* product, std::map<int, std::queue<CProduct*>> *_buffer);
	CProduct* MapPop(int key, std::map<int, std::queue<CProduct*>> *_buffer);
	CProduct* MapFront(int key, std::map<int, std::queue<CProduct*>>* _buffer);
	int BufferSize(int key, std::map<int, std::queue<CProduct*>> *_buffer);

	CProduct* StockBack(int key, std::map<int, std::queue<CProduct*>> *_stock);

	// subIdx 자동할당 관련
	std::map<int, int> m_subIdxMap;
	void m_subIdxMapUpdate(int pk);

	PostgreSQLConnector* pgconn;

	// CSV 출력 관련
	void CsvMake();
	void CsvStateInsert(int pk, std::string state, double state_start_time, double state_end_time);
	void CsvStateTimeInsert(int pk, double current_time, double init_time, double active_time, double error_time, double pause_time);
	void CsvStateRateInsert(int pk, double current_time, double init_time, double active_time, double error_time, double pause_time);
	void CsvBufferSize(int pk, std::string state, double current_time, double buffersize, double srocksize);
	void CsvProductFlowList(int pk, int product_id, double in_time, double out_time);
	void CsvFileClose();
	
	// SQL 연결 관련
	void Makepgconn();
	void Deletepgconn();
	void ResetTable();

	// CSV 파일 오픈 관련
	std::ofstream m_file;
	std::ofstream m_file1;
	std::ofstream m_file2;
	std::ofstream m_file3;
	std::ofstream m_file4;
	std::ofstream m_file5;
	std::ofstream m_file6;
	std::ofstream m_file7;
	std::ofstream m_file8;

	// 임시버퍼 최대 크기
	int m_maxbuffer[4] = { 10,10,10,10 };
	
	// 시나리오 넘버
	int	scenario_num = 1;

	// 공정 오류율(x회마다 고장)
	int error_gen = 5;
	int error_proc = 5;
	int error_stock = 5;
	int error_all = 5;
	int time = 100;

	// SQL 연결(기본값 true)
	bool SQLConnect = true;

	// param 사용
	bool useJson = true;

	// TA값 글로벌로 조정.
	double TA_STATE_INIT[4] = { 5,5,5,5 };
	double TA_STATE_ACTIVE[4] = { 5,WTime::infinity_,5,5 };
	double TA_STATE_ERROR[4] = { 5,0,5,5 };
	double TA_SEND_SEND_TIME[4] = { TA_STATE_ACTIVE[0] / 5, 2, TA_STATE_ACTIVE[2], TA_STATE_ACTIVE[3] };
};

#define GLOBAL_VAR	(CGlobalVar::GetInstance())