import AheuiJIT from 'aheui-jit';
import ToggleButton from 'react-toggle-button'

import { useEffect, useState } from 'react';


function useInput({ type /*...*/ }) {
    const [value, setValue] = useState("50000");
    const input = <input value={value} onChange={e => setValue(e.target.value)}  type={type}/>;
    return [value, input];
  }

function Config() {
  const [logIR, setLogIR] = useState(false);
  const [logDisasm, setLogDisasm] = useState(false);
  const [optIR, setOptIR] = useState(true);
  const [optAsm, setAsm] = useState(false);
  const [numCycle, numCycleInput] = useInput({type:"number"})
  const [interpretAfterFail, setinterpretAfterFail] = useState(false);

  useEffect(() => {
    AheuiJIT.config.logIR = logIR;
    AheuiJIT.config.logDisasm = logDisasm;
    AheuiJIT.config.optIR = optIR;
    AheuiJIT.config.optAsm = optAsm;
    AheuiJIT.config.numInterpretCycle = Number.parseInt(numCycle);
    AheuiJIT.config.interpretAfterFail = interpretAfterFail;
  });

  return (
    <div className="Config">
        <div className="ConfigField"> <span>인터프리터 사이클 수 (초기에 인터프리터 모드로 실행할 명령어 갯수): </span>
         {numCycleInput}</div>
         <div className="ConfigField"> <span> 인터프리터 fallback (저장소에 원소가 다 떨어졌을 잠시 동안 인터프리터 모드로 실행합니다): </span>
            <ToggleButton
                value={ interpretAfterFail }
                onToggle={(value) => {
                    setinterpretAfterFail(!value)
                }} />
        </div>
        <div className="ConfigField"> <span>IR 최적화 (중간 언어레벨에서 최적화합니다):</span>
            <ToggleButton
                value={ optIR }
                onToggle={(value) => {
                    setOptIR(!value)
                }} />
        </div>
        <div className="ConfigField"> <span> wasm 최적화 (웹어셈블리레벨에서 최적화합니다; 불안정): </span>
            <ToggleButton
                value={ optAsm }
                onToggle={(value) => {
                    setAsm(!value)
                }} />
        </div>
        <div className="ConfigField"> 
            <span> IR 로깅 (중간 언어를 콘솔에 출력합니다): </span>
            <ToggleButton
                value={ logIR }
                onToggle={(value) => {
                    setLogIR(!value)
                }} />
        </div>
        <div className="ConfigField"> <span>wat 로깅 (jit된 웹어셈블리를 콘솔에 출력합니다): </span>
            <ToggleButton
                value={ logDisasm }
                onToggle={(value) => {
                    setLogDisasm(!value)
                }} />
        </div>
    </div>
  );
}

export default Config;
