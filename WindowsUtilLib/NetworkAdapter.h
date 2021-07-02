#pragma once
#include <vector>
#include <string>
namespace zzj
{
class NetworkAdapter
{
  public:
    static int GetNetworkAdapters(std::vector<NetworkAdapter>& adapters,bool isRealAdapter);
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