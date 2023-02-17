#pragma once
#include <WObject.h>
// 최대 차량 생성 수

class CProduct : public WObject
{
public:
	// 과거 설비 인덱스
	int m_pastIdx;
	// 과거 설비 타입
	std::string m_pastType;
	// 현재 설비 인덱스
	int m_curIdx;
	// 현재 설비 타입
	std::string m_curType;
	// 제네레이터 생산 번호
	// 제네레이터 생산 번호 규칙은 제네레이터 인덱스 * 1000 + 제네레이터 생산 카운트
	int m_genID;
	// 제네레이터 인덱스
	int m_genIdx;
	// 생성 시간
	double m_genTime;
	// 통과 시간
	double m_passTime;


	CProduct(int genID, double genTime);
	CProduct(CProduct& copy);
};

