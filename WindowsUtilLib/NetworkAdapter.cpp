#include "NetworkAdapter.h"
#include "WMIHelper.h"

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
        isVirtual = var.boolVal;//VARIANT_BOOL: FALSE:0 TRUE:-1
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
