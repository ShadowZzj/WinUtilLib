
#include <Windows.h>
#include "call_json.h"
#include "BaseUtil.h"
#include "log.h"
using json = nlohmann::json;

bool CallJ::AddMethod(method_template method)
{
	return AddMethod("nul", method);
}

bool CallJ::AddMethod(std::string name, method_template method)
{
	CallJ::MethodInfo param(name, method);

	for (auto pair : methods)
		if (pair.first == name || pair.second == method)
			return false;

	methods.push_back(param);
	return true;
}

CallJ::MethodInfo CallJ::GetMethod(int index, bool& success)
{
	if (index >= methods.size()) {
		success = false;
		return CallJ::MethodInfo();
	}

	success = true;
	return methods[index];
}


bool CallJ::DeleteMethod(method_template method)
{
	for (auto i = methods.begin(); i != methods.end(); i++) {
		if (i->second == method) {
			methods.erase(i);
			return true;
		}
	}
	return false;
}

bool CallJ::DeleteMethod(std::string name)
{
	for (auto i = methods.begin(); i != methods.end(); i++) {
		if (i->first == name) {
			methods.erase(i);
			return true;
		}
	}
	return false;
}

bool CallJ::ExecCall(char* injson, int injsonlen, char** outjson, int* outjsonlen)
{
	try
	{
		json inj;
		nlohmann::detail::input_adapter inadapter(injson, (size_t)injsonlen);

		try
		{
			inj = json::parse(std::move(inadapter));
		}
		catch (...)
		{
			file_logger->error("Parse json fails in ExecCall");
			return false;
		}

		auto methodIndex_iter = inj.find(METHODINDEX_STR);
		auto methodParam_iter = inj.find(METHODPARAM_STR);
		//check input param
		if (!isIndexParamValid(methodIndex_iter, methodParam_iter, inj))
			return false;

		int iMethodIndex = methodIndex_iter->get<int>();
		bool success;

		CallJ::MethodInfo mt = GetMethod(iMethodIndex, success);
		if (!success)
			return false;
		file_logger->info("Accept IPC call method index:%d", iMethodIndex);

		std::string outStr;
		outStr.clear();

		json iter= methodParam_iter->get<json>();

		file_logger->info("MethodParameters: %s",iter.dump());

		std::tuple<int, std::string> stat;
		try {
			stat = mt.second(methodParam_iter, outStr);
		}
		catch (...) {
			file_logger->error("Method unhandled exception");
			std::get<0>(stat) = UnHandledException;
			std::get<1>(stat) = "UnHandledException, need to fix code";
		}

		std::string ret;
		ret.clear();

		try{
			json tmp;
			if (!outStr.empty())
				tmp = json::parse(outStr);
			SetRetJson(std::get<0>(stat), tmp, ret, std::get<1>(stat));
		}
		catch (...) {
			file_logger->error("Parse outStr json fails");
			return false;
		}

		char* temp = str::Dup(ret.data());
		if (temp == nullptr)
			return false;


		*outjson = temp;
		*outjsonlen = ret.size();
		file_logger->info("Return json: %s", temp);
		return true;
	}
	catch (const nlohmann::detail::exception & nde)
	{
		file_logger->error("nlohmann::json exception");
		return false;
	}
	catch (const std::exception & stde)
	{
		file_logger->error("Undandled exception in Execall");
		return false;
	}


}

void CallJ::SetRetJson(int iStatus, json& jmsg, std::string& retj, std::string errorMsg)
{
	json j;
	j[STATUS_STR] = iStatus;
	j[DATA] = jmsg;
	if (!errorMsg.empty())
		j[RETURNMSG_STR] = errorMsg;
	retj = j.dump();
}

bool CallJ::isIndexParamValid(nlohmann::json::iterator& indexIterator, nlohmann::json::iterator& paramIterator, json& inj)
{
	if (indexIterator == inj.end() ||
		paramIterator == inj.end())
	{
		return false;
	}
	if (!indexIterator->is_number_integer())
	{
		return false;
	}
	return true;
}

char* GetStrValueByKey(nlohmann::json::iterator& jparam, std::string key) {
	auto iter = jparam->find(key);
	if (iter != jparam->end())
		if (iter->is_string()) {
			std::string str = iter->get<std::string>();
			char* ret = str::Dup(str.c_str());
			return ret;
		}
	return nullptr;
}
wchar_t* GetWstrValueByKey(nlohmann::json::iterator& jparam, std::string key) {
	auto iter = jparam->find(key);
	if (iter != jparam->end())
		if (iter->is_string()) {

			std::string str = iter->get<std::string>();
			wchar_t* ret = str::Str2Wstr(str.c_str());
			return ret;
		}
	return nullptr;
}
