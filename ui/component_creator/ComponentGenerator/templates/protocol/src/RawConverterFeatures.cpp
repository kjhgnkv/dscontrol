#include "DSHost/Protocol/%name%/RawConverter.h"

namespace DS {
namespace protocols {
namespace %name% {

bool RawTo%name%ConverterFeatures::findMessageInStream(const FIFOBuffer& buffer, size_t& displ, size_t& size) const
{
    // TODO: implement message search in a byte stream here

    return true;
}

} } } // namespace DS::protocols::%name%

