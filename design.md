# IR

```
IRContext ctx;
Module* module = Module::Create(ctx, "main");
Function* func = Function::Create(ctx, module, "main", { ctx.makeTypeInt(32), ctx.makeTypeInt(32) });
BasicBlock* bb = BasicBlock::Create(ctx, func, "entry");
IRBuilder builder(bb);
Value* arg0 = func->getArg(0);
Value* arg1 = func->getArg(1);
Value* sum = builder.createBinOp(OpCode::Add, arg0, arg1);
func->retValue(sum);

```