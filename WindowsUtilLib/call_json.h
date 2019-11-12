#pragma once
#include <vector>
#include <utility>
/*
in param
{
	"MethodIndex":1
	"MethodParam":{...}
}

out param
{
	"Status":1
	"ReturnMsg":{...}
}

Status:
1---exec success
2---exec fail
3---not supported method
4---Method Index and Param not compatible
*/
#include "json.hpp"
using json = nlohmann::json;

class CallJ
{
public:
	enum ErrorCode {

	};

	typedef int (*method_template)(nlohmann::json::iterator& jparam, std::string& retj);
	typedef std::pair<std::string, method_template> MethodInfo;

	//if name or method has been in methods, return false 
	bool AddMethod(method_template method);
	bool AddMethod(std::string name, method_template method);
	MethodInfo GetMethod(int index, bool& success);
	bool DeleteMethod(method_template method);
	bool DeleteMethod(std::string name);
	bool ExecCall(char *injson, int injsonlen, char **outjson, int *outjsonlen);
	
	void SetRetJson(int iStatus, json& jmsg, std::string& retj);
private:
	const char* METHODINDEX_STR = "MethodIndex";
	const char* METHODPARAM_STR = "MethodParam";
	const char* STATUS_STR = "Status";
	const char* RETURNMSG_STR = "ReturnMsg";

	std::vector<MethodInfo> methods;
	bool isIndexParamValid(nlohmann::json::iterator &indexIterator, nlohmann::json::iterator &paramIterator, json& inj);

};

