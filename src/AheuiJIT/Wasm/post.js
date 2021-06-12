class AheuiJITImpl {
    constructor() {
        this.inited = false;
        this.ready = Module['RuntimeInitPromise'].promise;
        this._initRuntime = cwrap('initRuntime', null, [], { async: true });
        this._runAheuiCode = cwrap('runAheuiCode', 'number', ['string'], { async: true });
        this._setConfig = cwrap('setConfig', null, ['string', 'number'], { async: true });
        this.callbacks = {
            printChar: () => {},
            printNum: () => {},
            inputChar: () => {},
            inputNum: () => {}
        };
        this.config = {
            maxStorageSize: 4096,
            logIR: false,
            logDisasm: false,
            optIR: true,
            optAsm: false,
            numInterpretCycle: 256,
            interpretAfterFail: true,
            wasmMemorySize: 16777216 * 10
        };
    }

    async run(code) {
        if (!this.inited) {
            this.inited = true;
            await this._initRuntime();
        }
        await this.syncConfig();
        return await this._runAheuiCode(code);
    }

    async syncConfig() {
        await this._setConfig("maxStorageSize", this.config.maxStorageSize);
        await this._setConfig("logIR", this.config.logIR ? 1 : 0);
        await this._setConfig("logDisasm", this.config.logDisasm ? 1 : 0);
        await this._setConfig("optIR", this.config.optIR ? 1 : 0);
        await this._setConfig("optAsm", this.config.optAsm ? 1 : 0);
        await this._setConfig("numInterpretCycle", this.config.numInterpretCycle);
        await this._setConfig("interpretAfterFail", this.config.interpretAfterFail ? 1 : 0);
        await this._setConfig("wasmMemorySize", this.config.wasmMemorySize);
    }

    async setWasm(ptr, size) {
        const imports = {
            js: { 
                memory: wasmMemory,
                printChar: x => { this.printChar(x); }, 
                printNum: x => { this.printNum(x); },
                inputChar: () => { return this.inputChar(); },
                inputNum: () => { return this.inputNum(); }
            }
        };
        this.wasm = await WebAssembly.instantiate(Module.HEAP8.slice(ptr, ptr+size), imports);
        console.time("afterCompile");
    }

    async runWasm(ctx, sp) {
        this.wasm.instance.exports.main(ctx, sp);
    }

    printChar(word) {
        this.callbacks.printChar(String.fromCodePoint(word));
    }

    printNum(word) {
        this.callbacks.printNum(word);
    }

    inputChar() {
        return this.callbacks.inputChar().charCodeAt(0);
    }

    inputNum() {
        return this.callbacks.inputNum();
    }
}

Module['AheuiJIT'] = new AheuiJITImpl();

module.exports = Module['AheuiJIT'];