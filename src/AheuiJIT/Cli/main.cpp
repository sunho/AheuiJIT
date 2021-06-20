#include <AheuiJIT/Runtime/Runtime.h>

#include <codecvt>
#include <fstream>
#include <iostream>
#include <sstream>

std::string readFile(const char* filename) {
    std::ifstream wif(filename);
    std::stringstream wss;
    wss << wif.rdbuf();
    return wss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage " << argv[0] << " filename" << std::endl;
        return 1;
    }

    aheuijit::Runtime rt(aheuijit::creataeStdioMachine());
    auto conf = rt.getConfig();
    conf.numInterpretCycle = 0;
    conf.logDisasm = true;
    conf.logIR = true;
    rt.setConfig(conf);
    std::string str = readFile(argv[1]);
    std::u16string codecode = aheuijit::covnert_utf8_to_utf16(str);
    return rt.run(codecode);
}
