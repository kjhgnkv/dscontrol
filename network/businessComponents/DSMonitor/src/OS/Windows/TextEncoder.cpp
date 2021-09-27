#include "TextEncoder.h"

#include <Poco/TextConverter.h>

#include <Poco/ASCIIEncoding.h>

#include <Poco/Latin1Encoding.h>
#include <Poco/Latin2Encoding.h>
#include <Poco/Latin9Encoding.h>

#include <Poco/UTF8Encoding.h>
#include <Poco/UTF16Encoding.h>
#include <Poco/UTF32Encoding.h>

#include <Poco/Windows1250Encoding.h>
#include <Poco/Windows1251Encoding.h>
#include <Poco/Windows1252Encoding.h>

#include <algorithm>

#include <stringapiset.h>
#include <winnls.h>
#include <locale.h>

namespace dsmonitor
{
	ds::utils::ScopedPtr<Poco::TextEncoding> TextEncoder::ms_systemEncoder;
	
	std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> > TextEncoder::msc_textEncodingArray[] = 
	{
		std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("ascii"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::ASCIIEncoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("windows-1250"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Windows1250Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("cp1250"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Windows1250Encoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("windows-1251"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Windows1251Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("cp1251"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Windows1251Encoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("windows-1252"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Windows1252Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("cp1252"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Windows1252Encoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("iso-8859-1"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin1Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("latin1"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin1Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("latin-1"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin1Encoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("iso-8859-2"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin2Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("latin2"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin2Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("latin-2"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin2Encoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("iso-8859-15"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin9Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("latin9"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin9Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("latin-9"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::Latin9Encoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("utf-8"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::UTF8Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("utf8"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::UTF8Encoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("utf-16"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::UTF16Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("utf16"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::UTF16Encoding>))
			
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("utf-32"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::UTF32Encoding>))
		, std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >("utf32"
			, ds::utils::ScopedPtr<TextEncodingFactoryBase>(new TextEncodingFactory<Poco::UTF32Encoding>))
	};
	
void TextEncoder::systemTextEncodingFactory(const std::string& systemEncoding)
{
	std::string lowerSystemEncoding(systemEncoding.size(), 0);
    std::transform(systemEncoding.begin(), systemEncoding.end(), lowerSystemEncoding.begin(), ::tolower);
	
	for(int i = 0
		; i < (sizeof(msc_textEncodingArray) / sizeof(std::pair<std::string, ds::utils::ScopedPtr<TextEncodingFactoryBase> >))
		; i++)
	{
		if(msc_textEncodingArray[i].first.find(lowerSystemEncoding) != std::string::npos)
		{
			ms_systemEncoder = msc_textEncodingArray[i].second->create();
			return;
		}
	}
}

bool TextEncoder::isWin7() {
    DWORD version = GetVersion();
    DWORD major = (DWORD) (LOBYTE(LOWORD(version)));
    DWORD minor = (DWORD) (HIBYTE(LOWORD(version)));

    return (major == 6) && (minor <= 1);
}

void TextEncoder::getSystemEncoding()
{
	if(!isWin7())
		return;
	
	char codePage[10];
	GetLocaleInfo(GetSystemDefaultLCID()
		, LOCALE_IDEFAULTANSICODEPAGE
		, codePage
		, sizeof(codePage));
		
	systemTextEncodingFactory(codePage);
}

void TextEncoder::setUTF8Encoding()
{
	if(!isWin7())
		return;
	
	setlocale(LC_ALL, ".UTF8");
}

bool TextEncoder::convertToUTF8(const std::string& inText, std::string& outText)
{	
	if(!isWin7()
		|| !TextEncoder::ms_systemEncoder)
	{
		outText = inText;
		return false;
	}
	
	Poco::UTF8Encoding utf8;
	Poco::TextConverter converter(*TextEncoder::ms_systemEncoder.get(), utf8);
	
	converter.convert(inText, outText);
	
	return true;
}
}