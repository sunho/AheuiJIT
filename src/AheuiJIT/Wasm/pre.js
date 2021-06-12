class Deferred {
    constructor() {
        this.promise = new Promise((resolve, reject)=> {
        this.reject = reject
        this.resolve = resolve
        })
    }
}
  
Module['RuntimeInitPromise'] = new Deferred();

Module['onRuntimeInitialized'] = function() { 
    Module['RuntimeInitPromise'].resolve();
};