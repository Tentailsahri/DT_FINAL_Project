#pragma once
#include<string>
#include <libpq-fe.h>
#include <map>
#include<iostream>
#include<fstream>
#include<sstream>
#pragma comment(lib, "libpq.lib")
class PostgreSQLConnector
{
private:
	std::string m_DB_INFO_path;
	std::map<std::string, std::string> m_DB_INFO;
	PGconn* m_MainConnection;
	PGresult* m_MainSqlResult;
	ExecStatusType m_MainSqlStatus;
public:
	PostgreSQLConnector();
	PostgreSQLConnector(std::string info_file_loc);
	virtual ~PostgreSQLConnector();

	bool ConnectDB();
	bool SendQuery(std::string query);
	PGresult* GetSQLResult();
	std::string GetSQLStatus();
	bool CloseDB();
};

