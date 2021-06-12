#include "export.h"

#include <AheuiJIT/Runtime/Runtime.h>

#include <iostream>

static aheuijit::Runtime* rt;

void initRuntime() {
    rt = new aheuijit::Runtime(aheuijit::createWasmMachine());
}

void setConfig(const char* name_, int value) {
    aheuijit::RuntimeConfig conf = rt->getConfig();
    std::string name = name_;
    if (name == "optIR") {
        conf.optIR = value;
    } else if (name == "optAsm") {
        conf.optAsm = value;
    } else if (name == "logIR") {
        conf.logIR = value;
    } else if (name == "logDisasm") {
        conf.logDisasm = value;
    } else if (name == "maxStorageSize") {
        conf.maxStorageSize = value;
    } else if (name == "wasmMemorySize") {
        conf.wasmMemorySize = value;
    } else if (name == "numInterpretCycle") {
        conf.numInterpretCycle = value;
    } else if (name == "interpretAfterFail") {
        conf.interpretAfterFail = value;
    } else {
        std::cout << "Invalid config field" << std::endl;
        std::terminate();
    }
    rt->setConfig(conf);
}

int runAheuiCode(const char* code) {
    std::u16string u16code = aheuijit::covnert_utf8_to_utf16(code);
    return rt->run(u16code);
}