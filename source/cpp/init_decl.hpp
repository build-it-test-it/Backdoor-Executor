#pragma once

namespace RobloxExecutor {
namespace SystemState {
    // Forward declarations of methods implemented in init.cpp
    // These will override the inline declarations in init.hpp
    bool Initialize(const InitOptions& options);
    void Shutdown();
}
}
