class AheuiJITImpl {
    constructor() {
        this.inited = false;
        this.ready = Module['RuntimeInitPromise'].promise;
        this._initRuntime = cwrap('initRuntime', null, [], { async: true });
        this._runAheuiCode = cwrap('runAheuiCode', 'number', ['string'], { async: true });
        this._setConfig = cwrap('setConfig', null, ['string', 'number'], { async: true });
        this.jitMap = new Map();
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
            interpretAfterFail: false,
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

    async setWasm(id, ptr, size) {
        const imports = {
            js: { 
                memory: wasmMemory,
                printChar: x => { this.printChar(x); }, 
                printNum: x => { this.printNum(x); },
                inputChar: () => { return this.inputChar(); },
                inputNum: () => { return this.inputNum(); }
            }
        };
        this.jitMap.set(id, await WebAssembly.instantiate(Module.HEAP8.slice(ptr, ptr+size), imports));
        console.time("afterCompile");
    }

    async runWasm(id, ctx, sp, bb) {
        const wasm  = this.jitMap.get(id);
        wasm.instance.exports.main(ctx, sp, bb);
    }

    printChar(word) {
        this.callbacks.printChar(String.fromCodePoint(word));
    }

    printNum(word) {
        this.callbacks.printNum(word);
    }

    inputChar() {
        const out = this.callbacks.inputChar();
        if (out === 'eof') {
            return -1;
        }
        return out.charCodeAt(0);
    }

    inputNum() {
        return this.callbacks.inputNum();
    }
}

Module['AheuiJIT'] = new AheuiJITImpl();

module.exports = Module['AheuiJIT'];