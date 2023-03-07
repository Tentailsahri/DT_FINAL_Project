#pragma once
#include <WCoupModel.h>
#include "Cpd_GEN.h"
#include "Cpd_TRACK.h"
#include "Cpd_PROC.h"
#include "Cpd_STOCK.h"

class Cpd_Main : public WCoupModel	//Ŀ�ø�
{
private:
	// ���� ���� �Լ�
	void coupGenTrack(WCoupModel* GEN, WCoupModel* TRACK);
	void coupTrackProc(WCoupModel* TRACK, WCoupModel* PROC);
	void coupTrackProc(int num, WCoupModel* TRACK, WCoupModel* PROC);
	void coupProcTrack(WCoupModel* PROC, WCoupModel* TRACK);
	void coupTrackStock(WCoupModel* TRACK, WCoupModel* STOCK);
	void coupTrackTrack(WCoupModel* TRACK1, WCoupModel* TRACK2);
public:
	Cpd_Main(int road_num);
};

