#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <set>
#include <Windows.h>
void CopyToClipboard(const std::string &text)
{
	const size_t len = text.length() + 1;
	HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), text.c_str(), len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}
std::vector<std::string> EnumerateAllFiles()
{
	using std::experimental::filesystem::directory_iterator;
	std::vector<std::string> files;
	for(auto file:directory_iterator("../bot"))
	{
		const auto extension=file.path().extension();
		if(extension==".cpp" || extension==".h")
			files.push_back(file.path().filename().string());
	}
	return files;
}
bool DoesFileIncludeHeader(const std::string &file,const std::string &header)
{
	std::ifstream reader("../bot/"+file);
	for(std::string line;std::getline(reader,line);)
		if(line=="#include \""+header+"\"")
			return true;
	return false;
}
std::vector<std::string> OrderFiles()
{
	const auto allFiles=EnumerateAllFiles();
	std::vector<std::string> ordered;
	std::set<std::string> added;
	while(ordered.size()!=allFiles.size())
	{
		for(const auto &file:allFiles)
		{
			if(added.count(file)!=0)
				continue;
			bool ready=true;
			for(const auto &header:allFiles)
				if(DoesFileIncludeHeader(file,header) && added.count(header)==0)
					ready=false;
			if(ready)
			{
				ordered.push_back(file);
				added.insert(file);
				break;
			}
		}
	}
	return ordered;
}
int main()
{
	std::string result;
	for(const auto &file:OrderFiles())
	{
		std::ifstream reader("../bot/"+file);
		for(std::string line;std::getline(reader,line);)
			if(line.find("#include \"")!=0 && line.find("#pragma once")!=0)
				result+=line+"\n";
	}
	CopyToClipboard(result);
	return 0;
}
