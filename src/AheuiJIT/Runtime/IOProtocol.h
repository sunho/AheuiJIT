#pragma once

#include <AheuiJIT/IR/Value.h>

#include <fstream>
#include <iostream>

namespace aheuijit {

struct IOProtocol {
    virtual ~IOProtocol() {
    }
    virtual void printChar(char ch) = 0;
    virtual void printNum(Word word) = 0;
    virtual int inputChar() = 0;
    virtual Word inputNum() = 0;
};

struct StdIOProtocol : public IOProtocol {
    ~StdIOProtocol() = default;
    void printChar(char ch) override;
    void printNum(Word word) override;
    int inputChar() override;
    Word inputNum() override;
};

struct FileIOProtocol : public IOProtocol {
    FileIOProtocol() = delete;
    FileIOProtocol(std::ifstream& input, std::ostream& output);
    ~FileIOProtocol() = default;

    void printChar(char ch) override;
    void printNum(Word word) override;
    int inputChar() override;
    Word inputNum() override;

  private:
    std::ifstream& input;
    std::ostream& output;
};

};  // namespace aheuijit
