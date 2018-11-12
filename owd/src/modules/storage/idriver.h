#ifndef OWD_STORAGE_IDRIVER_H_
#define OWD_STORAGE_IDRIVER_H_

#include <string>

#include "types/weather_info.h"

namespace owd {
namespace storage {

class IDriver {
public:
    virtual ~IDriver() {}
    virtual int read(const std::string& city, types::WeatherInfo& info) = 0;
};

} // namespace storage
} // namespace owd

#endif // OWD_STORAGE_IDRIVER_H_
