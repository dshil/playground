#include <iostream>

#include "catch2/catch.hpp"

#include "storage/idriver.h"
#include "types/weather_info.h"

using namespace owd;

namespace {

class MockDriver : public storage::IDriver {
public:
    ~MockDriver() {
    }

    int read(const std::string& city, types::WeatherInfo& info) override {
        info.city = city;
        return 0;
    }
};

} // namespace

TEST_CASE("read by city") {
    MockDriver mock_driver;

    storage::IDriver *driver = &mock_driver;

    types::WeatherInfo info;
    driver->read("nyc", info);

    REQUIRE(info.city == "nyc");
}
