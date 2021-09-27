#pragma once

#include <Poco/TextEncoding.h>
#include <map>
#include <scopedPtr.h>

namespace dsmonitor
{
struct TextEncodingFactoryBase
{
	virtual ds::utils::ScopedPtr<Poco::TextEncoding> create() = 0;
	virtual ~TextEncodingFactoryBase() { }
};
	
struct TextEncoder
{
    static void getSystemEncoding();
	static void setUTF8Encoding();
	static bool convertToUTF8(const std::string& inText, std::string& outText);

private:
	static void systemTextEncodingFactory(const std::string& systemEncoding);
	static bool isWin7();
	
	static ds::utils::ScopedPtr<Poco::TextEncoding> ms_systemEncoder;
	static std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> > msc_textEncodingArray[];
};

template <typename TE>
struct TextEncodingFactory : public TextEncodingFactoryBase
{
	virtual ds::utils::ScopedPtr<Poco::TextEncoding> create()
	{
		return ds::utils::ScopedPtr<Poco::TextEncoding>(new TE);
	}
	virtual ~TextEncodingFactory() { }
};

}