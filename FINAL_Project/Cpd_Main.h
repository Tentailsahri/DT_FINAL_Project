#pragma once
#include <WCoupModel.h>
#include "Cpd_GEN.h"
#include "Cpd_TRACK.h"
#include "Cpd_PROC.h"
#include "Cpd_STOCK.h"
#include "GlobalVar.h"

class Cpd_Main : public WCoupModel	//커플모델
{
private:
	// 설비 연결 함수
	void coups(int num, int outPk, std::string outType, int inPk, std::string inType);
	void coupGenTrack(WCoupModel* GEN, WCoupModel* TRACK);
	void coupTrackProc(WCoupModel* TRACK, WCoupModel* PROC);
	void coupTrackProc(int num, WCoupModel* TRACK, WCoupModel* PROC);
	void coupProcTrack(WCoupModel* PROC, WCoupModel* TRACK);
	void coupTrackStock(WCoupModel* TRACK, WCoupModel* STOCK);
	void coupTrackTrack(WCoupModel* TRACK1, WCoupModel* TRACK2);

	// 모델 생성
	std::map<int, WCoupModel*> gen_cpd_map;
	std::map<int, WCoupModel*> track_cpd_map;
	std::map<int, WCoupModel*> proc_cpd_map;
	std::map<int, WCoupModel*> stock_cpd_map;
public:
	Cpd_Main(int road_num);
	int bufferPopNum=0;
	int tuplesCount = 0;
	int sendNum = 0;
	std::string sendType="";
	int receiveNum = 0;
	std::string receiveType = "";
	int subIdx = 0;
	std::map<int, std::queue<int>> receiveNumMap;
	void pushMap(int receive, int send);
};

