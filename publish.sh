rm -rf pkg
mkdir pkg
cp src/AheuiJIT/Wasm/package.json pkg
mkdir pkg/dist
cp wasm/AheuiJITWasm.js pkg/dist/index.js