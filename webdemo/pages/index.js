import dynamic from 'next/dynamic'; // Next.js dynamic
const CodeRunner = dynamic(() => import('../components/CodeRunner'), { loading: () => <div>Loading</div>, ssr: false}, );

function Index() {
  return (
    <div className="App">
       <a href="https://github.com/sunho/AheuiJIT" className="Source">소스코드 (github)</a>
        <div className="Logo">AheuiJIT web demo</div>
        <CodeRunner/>
    </div>
  );
}

export default Index;