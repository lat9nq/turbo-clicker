#include "driver/dummy.h"

namespace Driver {
Dummy::Dummy(const std::stop_source &stop) : Driver(stop){};
Dummy::~Dummy() {
    Join();
};

void Dummy::ButtonDown() {
    printf("button down\n");
}

void Dummy::ButtonUp() {
    printf("button up\n");
}

} // namespace Driver