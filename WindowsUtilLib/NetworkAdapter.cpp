#include "NetworkAdapter.h"
#include "WMIHelper.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#include "SystemHelper.h"

using namespace zzj;
using namespace std;
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma warning(disable : 4996)

int NetworkAdapter::GetNetworkAdapters(vector<NetworkAdapter> &adapters, bool isRealAdapter)
{
    if (zzj::SystemInfo::GetWindowsVersion() == 7)
    {
        return GetNetworkAdaptersByWin7(adapters, isRealAdapter);
    }
    else
    {
        return GetNetworkAdaptersByWin10(adapters, isRealAdapter);
    }

}

int zzj::NetworkAdapter::GetNetworkAdaptersByWin10(std::vector<NetworkAdapter> &adapters, bool isRealAdapter)
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
        return result;
    }

    netInfo.WMIExecQuery("WQL", "SELECT * FROM MSFT_NetAdapter");
    if (S_OK != hRes)
    {
        result = -2;
        return result;
    }

    while (S_OK == netInfo.WMIGetNextObject())
    {
        hRes = netInfo.WMIGetProperty(L"Virtual", var);
        if (S_OK != hRes || V_VT(&var) != VT_BOOL)
        {
            continue;
        }
        isVirtual = var.boolVal; // VARIANT_BOOL: FALSE:0 TRUE:-1
        netInfo.WMIReleaseProperty(var);
        if (-1 == isVirtual && isRealAdapter) // Continue if the adapter is virtual
            goto exit;

        hRes = netInfo.WMIGetProperty(L"InterfaceDescription", var);
        if (S_OK != hRes || V_VT(&var) != VT_BSTR)
        {
            goto exit;
        }
        adapter.description = var.bstrVal;
        netInfo.WMIReleaseProperty(var);

        hRes = netInfo.WMIGetProperty(L"PermanentAddress", var);
        if (S_OK != hRes || V_VT(&var) != VT_BSTR)
        {
            goto exit;
        }
        adapter.macAddr = var.bstrVal;
        netInfo.WMIReleaseProperty(var);

        hRes = netInfo.WMIGetProperty(L"Name", var);
        if (S_OK != hRes || V_VT(&var) != VT_BSTR)
        {
            goto exit;
        }
        adapter.name = var.bstrVal;
        netInfo.WMIReleaseProperty(var);

        adapters.push_back(adapter);

    exit:
        netInfo.WMIReleaseThisObject();
        if (0 != result)
            break;
    }

    return result;
}

int zzj::NetworkAdapter::GetNetworkAdaptersByWin7(std::vector<NetworkAdapter> &adapters, bool isRealAdapter)
{
    zzj::WMIWrapper netInfo;
    HRESULT hRes;
    VARIANT var;
    int result = 0;
    wstring tmp;
    NetworkAdapter adapter;
    VARIANT_BOOL isVirtual;

    hRes = netInfo.WMIConnectServer(L"ROOT\\CIMV2");
    if (S_OK != hRes)
    {
        result = -1;
        return result;
    }

    netInfo.WMIExecQuery("WQL", "SELECT * FROM Win32_NetworkAdapter");
    if (S_OK != hRes)
    {
        result = -2;
        return result;
    }

    while (S_OK == netInfo.WMIGetNextObject())
    {
        hRes = netInfo.WMIGetProperty(L"PhysicalAdapter", var);
        if (S_OK != hRes || V_VT(&var) != VT_BOOL)
        {
            goto exit;
        }
        isVirtual = var.boolVal; // VARIANT_BOOL: FALSE:0 TRUE:-1
        netInfo.WMIReleaseProperty(var);
        if (-1 == isVirtual && isRealAdapter) // Continue if the adapter is virtual
            goto exit;

        hRes = netInfo.WMIGetProperty(L"Description", var);
        if (S_OK != hRes || V_VT(&var) != VT_BSTR)
        {
            goto exit;
        }
        adapter.description = var.bstrVal;
        netInfo.WMIReleaseProperty(var);

        hRes = netInfo.WMIGetProperty(L"MACAddress", var);
        if (S_OK != hRes || V_VT(&var) != VT_BSTR)
        {
            goto exit;
        }
        adapter.macAddr = var.bstrVal;
        netInfo.WMIReleaseProperty(var);

        hRes = netInfo.WMIGetProperty(L"Name", var);
        if (S_OK != hRes || V_VT(&var) != VT_BSTR)
        {
            goto exit;
        }
        adapter.name = var.bstrVal;
        netInfo.WMIReleaseProperty(var);

        adapters.push_back(adapter);

    exit:
        netInfo.WMIReleaseThisObject();
        if (0 != result)
            break;
    }

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
    int sockHandle = 0;
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
    sockHandle                                     = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    AddrLen                                    = sizeof(Addr);
    result                                            = connect(sockHandle, (sockaddr *)&Addr, AddrLen);
    if (0!=result)
    {
        result = -2;
        goto exit;
    }
    result                                           = getsockname(sockHandle, (sockaddr *)&Addr, &AddrLen);
    if (0 != result)
    {
        result = -3;
        goto exit;
    }
    source_address                           = inet_ntoa(((struct sockaddr_in *)&Addr)->sin_addr);
    ipAddr = source_address;
exit:
    if (sockHandle)
    closesocket(sockHandle);
    WSACleanup();
    return result;
}

#define KB 1024
int zzj::NetworkAdapter::GetAllIpv4(std::vector<std::string> &ipAddrs)
{
    int result = 0;
    IP_ADAPTER_ADDRESSES *adapter_addresses(NULL);
    IP_ADAPTER_ADDRESSES *adapter(NULL);

    // Start with a 16 KB buffer and resize if needed -
    // multiple attempts in case interfaces change while
    // we are in the middle of querying them.
    DWORD adapter_addresses_buffer_size = 16 * KB;
    for (int attempts = 0; attempts != 3; ++attempts)
    {
        adapter_addresses = (IP_ADAPTER_ADDRESSES *)malloc(adapter_addresses_buffer_size);

        DWORD error = ::GetAdaptersAddresses(AF_UNSPEC,
                                             GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                                                 GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME,
                                             NULL, adapter_addresses, &adapter_addresses_buffer_size);

        if (ERROR_SUCCESS == error)
        {
            // We're done here, people!
            break;
        }
        else if (ERROR_BUFFER_OVERFLOW == error)
        {
            // Try again with the new size
            free(adapter_addresses);
            adapter_addresses = NULL;
            continue;
        }
        else
        {
            // Unexpected error code - log and throw
            free(adapter_addresses);
            adapter_addresses = NULL;
            result            = -1;
            goto exit;
        }
    }

    // Iterate through all of the adapters
    for (adapter = adapter_addresses; NULL != adapter; adapter = adapter->Next)
    {
        // Skip loopback adapters
        if (IF_TYPE_SOFTWARE_LOOPBACK == adapter->IfType)
        {
            continue;
        }

        // Parse all IPv4 and IPv6 addresses
        for (IP_ADAPTER_UNICAST_ADDRESS *address = adapter->FirstUnicastAddress; NULL != address;
             address                             = address->Next)
        {
            auto family = address->Address.lpSockaddr->sa_family;
            if (AF_INET == family)
            {
                // IPv4
                SOCKADDR_IN *ipv4 = reinterpret_cast<SOCKADDR_IN *>(address->Address.lpSockaddr);

                char str_buffer[INET_ADDRSTRLEN] = {0};
                inet_ntop(AF_INET, &(ipv4->sin_addr), str_buffer, INET_ADDRSTRLEN);
                ipAddrs.push_back(str_buffer);
            }
            else if (AF_INET6 == family)
            {
                // IPv6
                //SOCKADDR_IN6 *ipv6 = reinterpret_cast<SOCKADDR_IN6 *>(address->Address.lpSockaddr);

                //char str_buffer[INET6_ADDRSTRLEN] = {0};
                //inet_ntop(AF_INET6, &(ipv6->sin6_addr), str_buffer, INET6_ADDRSTRLEN);

                //std::string ipv6_str(str_buffer);

                //// Detect and skip non-external addresses
                //bool is_link_local(false);
                //bool is_special_use(false);

                //if (0 == ipv6_str.find("fe"))
                //{
                //    char c = ipv6_str[2];
                //    if (c == '8' || c == '9' || c == 'a' || c == 'b')
                //    {
                //        is_link_local = true;
                //    }
                //}
                //else if (0 == ipv6_str.find("2001:0:"))
                //{
                //    is_special_use = true;
                //}

                //if (!(is_link_local || is_special_use))
                //{
                //    ipAddrs.mIpv6.push_back(ipv6_str);
                //}
                continue;
            }
            else
            {
                // Skip all other types of addresses
                continue;
            }
        }
    }

    // Cleanup
exit:
    if (adapter_addresses)
        free(adapter_addresses);
    adapter_addresses = NULL;
    return result;
}
