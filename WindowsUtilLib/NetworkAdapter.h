#pragma once
#include <vector>
#include <string>
namespace zzj
{
class NetworkAdapter
{
  public:
    static int GetNetworkAdapters(std::vector<NetworkAdapter>& adapters,bool isRealAdapter);

    //property
    std::wstring description;
    std::wstring macAddr;
    std::wstring name;
  private:
};
}