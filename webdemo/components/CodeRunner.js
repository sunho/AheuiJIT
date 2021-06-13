import Examples from './Examples';
import Config from './Config';
import AheuiJIT from 'aheui-jit';
import { useEffect, useState } from 'react';

const PAGE_SIZE = 2048;

class StdoutBuffer {
  constructor() {
    this.buffer = "";
    this.page = 1;
    this.onFlush = (x) => {};
  }

  addMsg(msg) {
    this.buffer += msg;
    this.update();
  }

  flush(){
    this.onFlush(this.buffer);
    this.buffer = "";
    this.page = 1;
  }

  update() {
    if (this.buffer.length > this.page * PAGE_SIZE) {
      //this.onFlush(this.buffer);
      this.page ++;
    }
  }
};

class StdinBuffer {
  constructor() {
    this.buffer = "";
    this.cursor = 0;
  }

  setBuffer(buf) {
    this.buffer = buf;
    this.cursor = 0;
  }

  readNum() {
    const isBlank = (x) => {
      return x === '\n' || x === ' ';
    }
    let meetNum = false;
    let start;
    for (;this.cursor < this.buffer.length; ++this.cursor) {
      const ch = this.buffer.charAt(this.cursor);
      if ('0' <= ch && ch <= '9') {
        if (!meetNum) {
          start = this.cursor;
        }
        meetNum = true;
      } else if (isBlank(ch)) {
        if (meetNum) {
          break;
        }
        continue;
      } else {
        break;
      }
    }
    if (!meetNum) {
      throw "No number";
    }
    return Number.parseInt(this.buffer.slice(start, this.cursor));
  }

  readChar() {
    if (this.cursor >= this.buffer.length) {
      return 'eof';
    }
    const ch = this.buffer.charAt(this.cursor);
    if (ch === '\\' && this.cursor + 1 < this.buffer.length && this.buffer.charAt(this.cursor+1) === '0') {
      this.cursor += 2;
      return 'eof';
    }
    ++this.cursor;
    return ch;
  }
};

const stdio = new StdoutBuffer();
const stdin = new StdinBuffer();

AheuiJIT.callbacks.printChar = msg => { 
  stdio.addMsg(msg);
};

AheuiJIT.callbacks.printNum = num => { 
  stdio.addMsg(num.toString());
};

AheuiJIT.callbacks.inputNum = () => { 
  return stdin.readNum();
};

AheuiJIT.callbacks.inputChar = () => { 
  return stdin.readChar();
};

function useInput({ className, defaultValue }) {
  const [value, setValue] = useState(defaultValue);
  const input = <textarea className={className} value={value} onChange={e => setValue(e.target.value)}  />;
  return [value, input, setValue];
}

function CodeRunner() {
  const [isReady, setIsReady] = useState(false);
  const [out, setOut] = useState('');
  const [code, codeInput, setCodeInput] = useInput({ className: "Code", defaultValue: "밤밣따빠밣밟따뿌\n빠맣파빨받밤뚜뭏\n돋밬탕빠맣붏두붇\n볻뫃박발뚷투뭏붖\n뫃도뫃희멓뭏뭏붘\n뫃봌토범더벌뿌뚜\n뽑뽀멓멓더벓뻐뚠\n뽀덩벐멓뻐덕더벅" });
  const [input, inputInput, setInput] = useInput({ className: "Input", defaultValue: "" });
  useEffect(() => {
    AheuiJIT.ready.then(() => {
      setIsReady(true);
    });
  },[]);

  useEffect(() => {
    stdio.onFlush = x => { setOut(x); };
  },[]);

  return (
    <>
      <div className="Ready">(ready: {isReady ? "yes" : "no"})</div>
      <div>
        <Examples setCode={(code) => { setCodeInput(code);}} setInput={(i) => { setInput(i); }}/>
      </div>
      <div className="Container">
        <div>
          <div>input</div>
          {inputInput}
        </div>
        <div>
          <div>code</div>
          {codeInput}
        </div>
      </div>
      <div>
      <div className="RunButton" onClick={() => { 
        if (!isReady || code.length === 0)
          return;
        stdin.setBuffer(input);
        AheuiJIT.run(code).then(() => {
          stdio.flush();
        });
       }}>RUN</div>
       </div>
      <div className="Out">{out}</div>

       <Config/>
    </>
  );
}

export default CodeRunner;