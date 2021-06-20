#include "Translator.h"

using namespace aheuijit;

bool Translator::decodeNOP(Builder &b, const Token &token) {
    return true;
}

bool Translator::decodeADD(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 2);
    Local *rhs = b.pop(cur.pointer.queue);
    Local *lhs = b.pop(cur.pointer.queue);
    Local *value = b.add(lhs, rhs);
    b.push(cur.pointer.queue, value);
    return true;
}

bool Translator::decodeSUB(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 2);
    Local *rhs = b.pop(cur.pointer.queue);
    Local *lhs = b.pop(cur.pointer.queue);
    Local *value = b.sub(lhs, rhs);
    b.push(cur.pointer.queue, value);
    return true;
}

bool Translator::decodeMUL(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 2);
    Local *rhs = b.pop(cur.pointer.queue);
    Local *lhs = b.pop(cur.pointer.queue);
    Local *value = b.mul(lhs, rhs);
    b.push(cur.pointer.queue, value);
    return true;
}

bool Translator::decodeDIV(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 2);
    Local *rhs = b.pop(cur.pointer.queue);
    Local *lhs = b.pop(cur.pointer.queue);
    Local *value = b.div(lhs, rhs);
    b.push(cur.pointer.queue, value);
    return true;
}

bool Translator::decodeMOD(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 2);
    Local *rhs = b.pop(cur.pointer.queue);
    Local *lhs = b.pop(cur.pointer.queue);
    Local *value = b.mod(lhs, rhs);
    b.push(cur.pointer.queue, value);
    return true;
}

bool Translator::decodeCMP(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 2);
    Local *rhs = b.pop(cur.pointer.queue);
    Local *lhs = b.pop(cur.pointer.queue);
    Local *value = b.cmp(lhs, rhs);
    b.push(cur.pointer.queue, value);
    return true;
}

bool Translator::decodeCHANGE(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 2);
    Local *top = b.pop(cur.pointer.queue);
    Local *next = b.pop(cur.pointer.queue);
    if (cur.pointer.queue) {
        b.pushQueueFront(top);
        b.pushQueueFront(next);

    } else {
        b.pushStack(top);
        b.pushStack(next);
    }
    return true;
}

bool Translator::decodeDUP(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 1);
    Local *val = b.pop(cur.pointer.queue);
    if (cur.pointer.queue) {
        b.pushQueueFront(val);
        b.pushQueueFront(val);
    } else {
        b.pushStack(val);
        b.pushStack(val);
    }
    return true;
}

bool Translator::decodeSELECT(Builder &b, const Token &token) {
    const Storage store = token.storage.toStorage();
    const bool newQueue = !store.stack;
    cur.pointer.queue = newQueue;
    Constant *val = b.createConstant(store.toImm());
    b.setStore(val);
    return true;
}

bool Translator::decodePUSH(Builder &b, const Token &token) {
    Value *value;
    if (token.storage == TokenStorage::OO) {
        value = b.inputNum(nullptr);
    } else if (token.storage == TokenStorage::HIET) {
        value = b.inputChar(nullptr);
    } else {
        value = b.createConstant(token.storage.toNumConst());
    }
    b.push(cur.pointer.queue, value);
    return true;
}

bool Translator::decodeMOV(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 1);
    Local *value = b.pop(cur.pointer.queue);
    Local *backup = b.getStore(nullptr);
    Storage store = token.storage.toStorage();
    Constant *storeValue = b.createConstant(store.toImm());
    b.setStore(storeValue);
    b.push(!store.stack, value);
    b.setStore(backup);
    return true;
}

bool Translator::decodePOP(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 1);
    Local *value = b.pop(cur.pointer.queue);
    if (token.storage == TokenStorage::OO) {
        b.outputNum(value);
    } else if (token.storage == TokenStorage::HIET) {
        b.outputChar(value);
    }
    return true;
}

bool Translator::decodeEXIT(Builder &b, const Token &token) {
    b.setExitTerminal();
    return false;
}

bool Translator::decodeCBZ(Builder &b, const Token &token) {
    b.checkStorage(cur.pointer.queue, 1);
    Local *pred = b.pop(cur.pointer.queue);
    b.setConditionalTerminal(pred);
    return false;
}
