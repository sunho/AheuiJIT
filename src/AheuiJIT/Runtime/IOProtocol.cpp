#include "IOProtocol.h"

using namespace aheuijit;

void StdIOProtocol::printChar(char ch) {
    putchar(ch);
}

void StdIOProtocol::printNum(Word word) {
    printf("%lld", word);
}

int StdIOProtocol::inputChar() {
    return getchar();
}

Word StdIOProtocol::inputNum() {
    Word word;
    scanf("%lld", &word);
    return word;
}

FileIOProtocol::FileIOProtocol(std::ifstream& input, std::ostream& output)
    : input(input), output(output) {
}

void FileIOProtocol::printChar(char ch) {
    output << ch;
}

void FileIOProtocol::printNum(Word word) {
    output << word;
}

int FileIOProtocol::inputChar() {
    char ch;
    if (input.eof()) {
        return -1;
    }
    input.get(ch);
    putchar(ch);
    return reinterpret_cast<unsigned char&>(ch);
}

Word FileIOProtocol::inputNum() {
    Word word;
    input >> word;
    return word;
}
