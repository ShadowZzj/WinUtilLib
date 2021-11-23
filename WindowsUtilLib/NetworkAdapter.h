#pragma once
#include <vector>
#include <string>
namespace zzj
{
class NetworkAdapter
{
  public:

    //https:// docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-networkadapter
    //https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/hh968170(v=vs.85)

    static int GetNetworkAdapters(std::vector<NetworkAdapter>& adapters,bool isRealAdapter);
    static int GetNetworkAdaptersByWin10(std::vector<NetworkAdapter> &adapters, bool isRealAdapter);
    static int GetNetworkAdaptersByWin7(std::vector<NetworkAdapter> &adapters, bool isRealAdapter);
    static int GetOutIpAddress(std::string& ipAddr);
    static int GetAllIpv4(std::vector<std::string> &ipAddrs);
    //property
    std::wstring description;
    std::wstring macAddr;
    std::wstring name;
    std::string ipAddr;
  private:
};
}