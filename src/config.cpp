#include <rift/config.hpp>

namespace rift::config {

    static RuntimeFuncMap runtimeFunctions;

    const RuntimeFuncMap& getRuntimeFunctions() {
        return runtimeFunctions;
    }

    void addRuntimeFunction(const std::string& name, const RuntimeFunc& func) {
        runtimeFunctions[name] = func;
    }
}


