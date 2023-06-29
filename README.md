# AheuiJIT

AheuiJIT is a just in time compiler for [Aheui language](https://aheui.readthedocs.io/en/latest/specs.en.html) built from scratch. It implements JIT compilation without using any fancy external libraries. In fact, only libraries it depends on is assembler and c++ standard library. It's about 10 times faster than the previously fastest implementations of Aheui language.

## Supported devices
- 64 bit windows, mac, linux (x86)
- Web browsers or nodejs runtime that supports WebAssembly ([web demo](https://sunho.io/AheuiJIT))
- Android (ARM 64)

## Features

- It passes all [Aheui test cases](https://github.com/aheui/snippets) except the max 64 bit integer print test case.
- It can run [aheui aheui](https://github.com/aheui/aheui.aheui) (aheui interpreter written in aheui)
- It can even run aheui aheui aheui.
- It's fast
    - It runs logo test case within 0.21 seconds.
    - It runs aheui aheui + quine testcase within 0.7 seconds. (which is 8 times faster existing JIT compiler rpaheui)
- Web version is pretty fast as well.
    - It runs logo testcase within 0.23 seconds.
 
# Korean description

# 아희짓

## 개요

아희짓은 [아희 언어](https://aheui.readthedocs.io/en/latest/introduction.html)를 위한 JIT (Just in Time) 컴파일러입니다. 어셈블러와 유틸 라이브러리외에 외부 라이브러리에 전혀 의존하지 않고 JIT을 바닥부터 구현합니다. 

## 지원 환경

- 64비트 windows, mac, linux (x86 아키텍쳐)
- 웹어셈블리를 지원하는 웹 브라우저 및 nodejs ([웹 데모](https://sunho.io/AheuiJIT))
- 안드로이드 (arm 64비트)

## 완성도

- [아희 테스트 케이스](https://github.com/aheui/snippets) 중 64 비트 정수 최대값 출력을 제외한 모든 테스트를 통과합니다.
    - 특히 저장소의 원소를 모두 소모하면 반대로 가는 스펙을 제대로 구현합니다.
- 아희아희를 원할하게 구동합니다. (quine과 logo가 작동하는 것을 확인했습니다.)
- 빠릅니다.
    - logo 아희 테스트를 0.21초 안에 완료합니다. (JIT 컴파일 시간 0.05초 포함)
    - 아희아희 + quine 테스트를 0.7초안에 완료합니다. (기존 jit 컴파일러인 rpaheui보다 8배 빠른 결과입니다.)
- 웹버전도 꽤나 빠릅니다.
    - logo 아희 테스트를 JIT 컴파일 시간을 제외하면 0.23초 안에 완료합니다.

## 사용법

### PC 버전

릴리즈 페이즈에 가서 운영체제에 맞는 압축파일을 다운받고 압축을 푸세요.
```
bin/AheuiJITCli 아희파일 
```
로 아희 프로그램을 실행해볼 수 있습니다.

inlcude와 lib폴더에는 정적 라이브러리로 빌드된 AheuiJIT이 있습니다.

### 웹 버전

```
npm install aheui-jit
```
하신 다음

```js
import AheuiJIT from 'aheui-jit';

AheuiJIT.callbacks.printChar = msg => { 
    console.log(msg);
};

AheuiJIT.callbacks.printNum = num => { 
    console.log(num.toString());
};

AheuiJIT.config.logIR = true;

AheuiJIT.ready.then(() => {
    return AheuiJIT.run("밤밣따빠밣밟따뿌\n빠맣파빨받밤뚜뭏\n돋밬탕빠맣붏두붇\n볻뫃박발뚷투뭏붖\n뫃도뫃희멓뭏뭏붘\n뫃봌토범더벌뿌뚜\n뽑뽀멓멓더벓뻐뚠\n뽀덩벐멓뻐덕더벅");
}).then(() => {
    console.log("finish");
});

```
이런식으로 쓰실 수 있습니다.

## 빌드 

C++17을 지원하는 컴파일러와 cmake을 깔고 아래 명령어를 실행하면 됩니다.

```
git submodul update --init
mkdir build
cd build
cmake ..
```

웹어셈블리 버전은 cmake .. 대신 emcmake cmake .. 을 사용하시면 됩니다.
