#include "PostgreSQLConnector.h"

PostgreSQLConnector::PostgreSQLConnector() {
	m_MainConnection = nullptr;
	m_MainSqlResult = nullptr;
	m_MainSqlStatus = ExecStatusType::PGRES_EMPTY_QUERY;
	m_DB_INFO_path = "./DBDATA.txt";
}

PostgreSQLConnector::PostgreSQLConnector(std::string info_file_loc)
{
	m_MainConnection = nullptr;
	m_MainSqlResult = nullptr;
	m_MainSqlStatus = ExecStatusType::PGRES_EMPTY_QUERY;
	m_DB_INFO_path = info_file_loc;
	m_DB_INFO.clear();
}

PostgreSQLConnector::~PostgreSQLConnector()
{
	m_DB_INFO.clear();
	CloseDB();
}

bool PostgreSQLConnector::ConnectDB()
{
	std::ifstream ifs(m_DB_INFO_path, std::ios_base::in);
	std::string read_line;
	int row_num = 0;
	while (row_num < 5) {
		ifs >> read_line;
		std::string keyword;
		std::string db_info;
		std::stringstream ss(read_line);
		int col_num = 0;
		while (std::getline(ss, db_info, ':')) {
			if (col_num == 0) {
				keyword = db_info;
				m_DB_INFO.insert(std::make_pair(keyword, ""));
			}
			else if (col_num == 1) {
				m_DB_INFO[keyword] = db_info;
				break;
			}
			col_num++;
		}
		row_num++;
	}
	const char* _db_param_keywords[] = { "hostaddr", "port", "dbname", "user", "password", "client_encoding", 0 };
	const char* _db_param_values[] = { "192.168.0.18", "5432", "final_DB", "postgres", "1234", "UTF-8", 0 };
	m_MainConnection = PQconnectdbParams(_db_param_keywords, _db_param_values, 0);
	PQsetClientEncoding(m_MainConnection, "euc-kr");
	if (PQstatus(m_MainConnection) != ConnStatusType::CONNECTION_OK) {
		printf("%s", PQerrorMessage(m_MainConnection));
		CloseDB();
		return false;
	}
	printf("Connect to databaase sucesss\n");
	return true;
}

bool PostgreSQLConnector::SendQuery(std::string query)
{
	if (PQstatus(m_MainConnection) != ConnStatusType::CONNECTION_OK) {
		printf("%s", PQerrorMessage(m_MainConnection));
		return false;
	}
	if (m_MainConnection != nullptr) {
		PQclear(m_MainSqlResult);
		m_MainSqlResult = nullptr;
	}

	m_MainSqlResult = PQexec(m_MainConnection, query.c_str());
	m_MainSqlStatus = PQresultStatus(m_MainSqlResult);
	if (m_MainSqlStatus != PGRES_COMMAND_OK && m_MainSqlStatus != PGRES_TUPLES_OK) {
		printf("%s", PQerrorMessage(m_MainConnection));
	}
	return true;
}

PGresult* PostgreSQLConnector::GetSQLResult()
{
	if (m_MainSqlStatus == PGRES_TUPLES_OK) {
		return m_MainSqlResult;
	}

	else return nullptr;
}

std::string PostgreSQLConnector::GetSQLStatus()
{

	if (m_MainSqlStatus == PGRES_EMPTY_QUERY) {
		return "PGRES_EMPTY_QUERY=0";
	}
	else if (m_MainSqlStatus == PGRES_COMMAND_OK) {
		return "PGRES_COMMAND_OK";
	}
	else if (m_MainSqlStatus == PGRES_TUPLES_OK) {
		return "PGRES_TUPLES_OK";
	}
	else if (m_MainSqlStatus == PGRES_COPY_OUT) {
		return "PGRES_COPY_OUT";
	}
	else if (m_MainSqlStatus == PGRES_COPY_IN) {
		return "PGRES_COPY_IN";
	}
	else if (m_MainSqlStatus == PGRES_BAD_RESPONSE) {
		return "PGRES_BAD_RESPONSE";
	}
	else if (m_MainSqlStatus == PGRES_NONFATAL_ERROR) {
		return "PGRES_NONFATAL_ERROR";
	}
	else if (m_MainSqlStatus == PGRES_FATAL_ERROR) {
		return "PGRES_FATAL_ERROR";
	}
	else if (m_MainSqlStatus == PGRES_COPY_BOTH) {
		return "PGRES_COPY_BOTH";
	}
	else if (m_MainSqlStatus == PGRES_SINGLE_TUPLE) {
		return "PGRES_SINGLE_TUPLE";
	}
	else return "NONE";
}

bool PostgreSQLConnector::CloseDB()
{
	if (m_MainSqlResult != nullptr) {
		PQclear(m_MainSqlResult);
		m_MainSqlResult = nullptr;
		printf("SQL result clear\n");
	}
	if (m_MainConnection != nullptr) {
		PQfinish(m_MainConnection);
		m_MainConnection = nullptr;
		printf("CLOSE MAIN CONNECTION\n");
	}
	return true;
}
