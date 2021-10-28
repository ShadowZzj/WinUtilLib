#ifndef ZZJFILE
#define ZZJFILE

#include <Windows.h>
#include <functional>
#include <map>
#include <string>
namespace zzj
{

class FileProperty 
{
  
  public:
    FileProperty(const std::string &imageName);
    ~FileProperty();

  public:
    std::string GetFileDescription();
    std::string GetFileVersion();
    std::string GetInternalName();
    std::string GetCompanyName();
    std::string GetLegalCopyright();
    std::string GetOriginalFilename();
    std::string GetProductName();
    std::string GetProductVersion();

  private:
    bool _queryValue(const std::string &valueName, const std::string &moduleName, std::string &RetStr);

  private:
    std::string m_imageName;
};

class FileInfo
{
  public:
    FileInfo(const std::string &imageName);
    ~FileInfo();

  public:
    std::int64_t GetFileSize();

  private:
    std::string m_imageName;
};

class File
{

  public:
    File(const std::string &imageName);
    ~File();
    std::unique_ptr<zzj::FileProperty> GetFileProperty();
    std::unique_ptr<zzj::FileInfo> GetFileInfo();
  private:
    std::string m_imageName;
};

} // namespace zzj
#endif //
