import samples from "./samples.json";

function Examples(props) {
    return (<div className="SampleContainer" >
        <div>코드 샘플 불러오기:</div>
        {samples.map(x => (
        <div className="SampleButton" onClick={() => {props.setCode(x.code); if(x.input) props.setInput(x.input);}}>{x.name}</div>
    ))}
    </div>);
}

export default Examples;
