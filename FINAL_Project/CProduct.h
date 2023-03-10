#pragma once
#include <WObject.h>
// 최대 차량 생성 수

class CProduct : public WObject
{
public:
	// 과거 설비 인덱스
	int m_pastPk;
	// 과거 설비 타입
	std::string m_pastType;
	// 현재 설비 인덱스
	int m_curPk;
	// 현재 설비 타입
	std::string m_curType;
	// 제품 생산 번호
	// 제품 생산 번호 규칙은 설비 pk * 1000 + 생산 카운트
	int m_genID;
	// 제네레이터 인덱스
	int m_genPk;
	// 생성 시간
	double m_genTime;
	// 통과 시간
	double m_passTime;

	int m_targetPk;


	CProduct(int genID, double genTime);
	CProduct(CProduct& copy);
};

