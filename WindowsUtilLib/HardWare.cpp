#include "HardWare.h"
#include "StrUtil.h"
#include "WMIHelper.h"
#include <Windows.h>
#include <intrin.h>
using namespace zzj;

std::wstring CPU::GetCPUBrandString()
{
    int cpuInfo[4] = {-1};
    char cpuBrandString[0x40]{};
    // Get the information associated with each extended ID.
    __cpuid(cpuInfo, 0x80000000);
    for (int i = 0x80000002; i <= 0x80000004; ++i)
    {
        __cpuid(cpuInfo, i);
        // Interpret CPU brand string
        if (i == 0x80000002)
            memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000003)
            memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000004)
            memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));
    }

    return str::Str2Wstr(std::string(cpuBrandString));
}

std::vector<std::wstring> HardDrive::GetHardDriveSerialNumber()
{
    WMIWrapper hdHandler;
    HRESULT hRes;
    VARIANT var;
    std::vector<std::wstring> ret;

    hRes = hdHandler.WMIConnectServer(L"root\\cimv2");
    if (S_OK != hRes)
    {
        ret.clear();
        goto exit;
    }

    hRes = hdHandler.WMIExecQuery("WQL", "SELECT SerialNumber FROM Win32_PhysicalMedia");
    if (S_OK != hRes)
    {
        ret.clear();
        goto exit;
    }

    while (S_OK == hdHandler.WMIGetNextObject())
    {
        hRes = hdHandler.WMIGetProperty(L"SerialNumber", var);
        if (S_OK != hRes)
        {
            ret.clear();
            goto exit;
        }

        std::wstring tmp;
        tmp = (NULL == var.bstrVal) ? L"" : var.bstrVal; 
        ret.push_back(tmp);
    }

exit:
    VariantClear(&var);
    return ret;
}

std::wstring Bios::GetBiosSerialNumber()
{
    WMIWrapper hdHandler;
    HRESULT hRes;
    VARIANT var;
    std::wstring ret;

    hRes = hdHandler.WMIConnectServer(L"root\\cimv2");
    if (S_OK != hRes)
    {
        ret.clear();
        goto exit;
    }

    hRes = hdHandler.WMIExecQuery("WQL", "SELECT * FROM Win32_Bios");
    if (S_OK != hRes)
    {
        ret.clear();
        goto exit;
    }

    while (S_OK == hdHandler.WMIGetNextObject())
    {
        hRes = hdHandler.WMIGetProperty(L"SerialNumber", var);
        if (S_OK != hRes)
        {
            ret.clear();
            goto exit;
        }


        ret = (NULL == var.bstrVal) ? L"" : var.bstrVal;
        
    }

exit:
    VariantClear(&var);
    return ret;
}
