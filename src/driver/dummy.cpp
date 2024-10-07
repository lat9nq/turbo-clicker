#include "driver/dummy.h"

namespace Driver {
Dummy::Dummy(const std::stop_source &stop) : Driver(stop){};
Dummy::~Dummy() = default;

void Dummy::EmitDown() {
    printf("button down\n");
}

void Dummy::EmitUp() {
    printf("button up\n");
}

} // namespace Driver