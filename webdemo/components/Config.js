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
        <div className="ConfigField"> <span>Initial interpreter cycles: </span>
         {numCycleInput}</div>
         <div className="ConfigField"> <span> Fallback to interpreter for a while when it ran out of storage: </span>
            <ToggleButton
                value={ interpretAfterFail }
                onToggle={(value) => {
                    setinterpretAfterFail(!value)
                }} />
        </div>
        <div className="ConfigField"> <span> Optimize in Intermediate Representation (IR) level: </span>
            <ToggleButton
                value={ optIR }
                onToggle={(value) => {
                    setOptIR(!value)
                }} />
        </div>
        <div className="ConfigField"> <span> Optimize in WebAssembly level (unstable): </span>
            <ToggleButton
                value={ optAsm }
                onToggle={(value) => {
                    setAsm(!value)
                }} />
        </div>
        <div className="ConfigField"> 
            <span> Log Intermediate Representation (IR) to Console: </span>
            <ToggleButton
                value={ logIR }
                onToggle={(value) => {
                    setLogIR(!value)
                }} />
        </div>
        <div className="ConfigField"> <span> Log WebAssembly to Console: </span>
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
