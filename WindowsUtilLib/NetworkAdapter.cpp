#include "NetworkAdapter.h"
#include "WMIHelper.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Winsock2.h>
using namespace zzj;
using namespace std;
int NetworkAdapter::GetNetworkAdapters(vector<NetworkAdapter> &adapters, bool isRealAdapter)
{
    zzj::WMIWrapper netInfo;
    HRESULT hRes;
    VARIANT var;
    int result = 0;
    wstring tmp;
    NetworkAdapter adapter;
    VARIANT_BOOL isVirtual;

    hRes = netInfo.WMIConnectServer(L"ROOT\\StandardCimv2");
    if (S_OK != hRes)
    {
        result = -1;
        goto exit;
    }

    netInfo.WMIExecQuery("WQL", "SELECT * FROM MSFT_NetAdapter");
    if (S_OK != hRes)
    {
        result = -2;
        goto exit;
    }

    while (S_OK == netInfo.WMIGetNextObject())
    {
        hRes = netInfo.WMIGetProperty(L"Virtual", var);
        if (S_OK != hRes)
        {
            result = -3;
            goto exit;
        }
        isVirtual = var.boolVal;              // VARIANT_BOOL: FALSE:0 TRUE:-1
        if (-1 == isVirtual && isRealAdapter) // Continue if the adapter is virtual
            continue;

        hRes = netInfo.WMIGetProperty(L"InterfaceDescription", var);
        if (S_OK != hRes)
        {
            result = -3;
            goto exit;
        }
        adapter.description = var.bstrVal;

        hRes = netInfo.WMIGetProperty(L"PermanentAddress", var);
        if (S_OK != hRes)
        {
            result = -3;
            goto exit;
        }
        adapter.macAddr = var.bstrVal;

        hRes = netInfo.WMIGetProperty(L"Name", var);
        if (S_OK != hRes)
        {
            result = -2;
            goto exit;
        }
        adapter.name = var.bstrVal;
        adapters.push_back(adapter);
    }

exit:
    VariantClear(&var);
    return result;
}

int zzj::NetworkAdapter::GetOutIpAddress(std::string &ipAddr)
{
    WSADATA wsaData;
    int result = 0;
    const char *destination_address;
    sockaddr_storage Addr;
    unsigned long addr;
    WORD wVersionRequested = MAKEWORD(2, 1);
    char *source_address;
    int Handle;
    int AddrLen;

    result = WSAStartup(wVersionRequested, &wsaData);
    if (0 != result)
    {
        result = -1;
        goto exit;
    }
    destination_address                            = "8.8.8.8";
    Addr                                           = {0};
    addr                                           = inet_addr(destination_address);
    ((struct sockaddr_in *)&Addr)->sin_addr.s_addr = addr;
    ((struct sockaddr_in *)&Addr)->sin_family      = AF_INET;
    ((struct sockaddr_in *)&Addr)->sin_port        = htons(9); // 9 is discard port
    Handle                                     = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    AddrLen                                    = sizeof(Addr);
    result                                            = connect(Handle, (sockaddr *)&Addr, AddrLen);
    if (0!=result)
    {
        result = -2;
        goto exit;
    }
    result                                           = getsockname(Handle, (sockaddr *)&Addr, &AddrLen);
    if (0 != result)
    {
        result = -3;
        goto exit;
    }
    source_address                           = inet_ntoa(((struct sockaddr_in *)&Addr)->sin_addr);
    ipAddr = source_address;
exit:
    WSACleanup();
    return result;
}
