// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
interface WasmModule {
}

type EmbindString = ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string;
export interface ClassHandle {
  isAliasOf(other: ClassHandle): boolean;
  delete(): void;
  deleteLater(): this;
  isDeleted(): boolean;
  // @ts-ignore - If targeting lower than ESNext, this symbol might not exist.
  [Symbol.dispose](): void;
  clone(): this;
}
export interface BoolVarVector extends ClassHandle, Iterable<BoolVar> {
  size(): number;
  get(_0: number): BoolVar | undefined;
  push_back(_0: BoolVar): void;
  resize(_0: number, _1: BoolVar): void;
  set(_0: number, _1: BoolVar): boolean;
}

export interface IntVarVector extends ClassHandle, Iterable<IntVar> {
  size(): number;
  get(_0: number): IntVar | undefined;
  push_back(_0: IntVar): void;
  resize(_0: number, _1: IntVar): void;
  set(_0: number, _1: IntVar): boolean;
}

export interface Int64Vector extends ClassHandle, Iterable<bigint> {
  push_back(_0: bigint): void;
  resize(_0: number, _1: bigint): void;
  size(): number;
  get(_0: number): bigint | undefined;
  set(_0: number, _1: bigint): boolean;
}

export interface Domain extends ClassHandle {
}

export interface BoolVar extends ClassHandle {
  WithName(_0: EmbindString): BoolVar;
  not(): BoolVar;
  Name(): string;
}

export interface IntVar extends ClassHandle {
}

export interface LinearExpr extends ClassHandle {
  mutableAdd(_0: LinearExpr): LinearExpr;
  immutableAdd(_0: LinearExpr): LinearExpr;
}

export interface Constraint extends ClassHandle {
  onlyEnforceIf(_0: BoolVar): Constraint;
  onlyEnforceIfAll(_0: BoolVarVector): Constraint;
}

export interface CpModelProto extends ClassHandle {
}

export interface CpModelBuilder extends ClassHandle {
  newBoolVar(): BoolVar;
  newIntVar(_0: Domain): IntVar;
  addAssumption(_0: BoolVar): void;
  addLessOrEqual(_0: LinearExpr, _1: LinearExpr): Constraint;
  addLessThan(_0: LinearExpr, _1: LinearExpr): Constraint;
  addGreaterOrEqual(_0: LinearExpr, _1: LinearExpr): Constraint;
  addGreaterThan(_0: LinearExpr, _1: LinearExpr): Constraint;
  addEquality(_0: LinearExpr, _1: LinearExpr): Constraint;
  addNotEqual(_0: LinearExpr, _1: LinearExpr): Constraint;
  addAllDifferent(_0: IntVarVector): Constraint;
  addBoolAnd(_0: BoolVarVector): Constraint;
  addBoolOr(_0: BoolVarVector): Constraint;
  addBoolVarHint(_0: BoolVar, _1: boolean): void;
  addIntVarHint(_0: IntVar, _1: bigint): void;
  clearAssumptions(): void;
  clearHints(): void;
  maximize(_0: LinearExpr): void;
  build(): CpModelProto;
}

export interface CpSolverStatusValue<T extends number> {
  value: T;
}
export type CpSolverStatus = CpSolverStatusValue<0>|CpSolverStatusValue<1>|CpSolverStatusValue<2>|CpSolverStatusValue<3>|CpSolverStatusValue<4>;

export interface CpSolverResponse extends ClassHandle {
  status(): CpSolverStatus;
  objectiveValue(): number;
}

export interface Model extends ClassHandle {
}

interface EmbindModule {
  BoolVarVector: {
    new(): BoolVarVector;
  };
  IntVarVector: {
    new(): IntVarVector;
  };
  Int64Vector: {
    new(): Int64Vector;
  };
  Domain: {
    new(_0: bigint, _1: bigint): Domain;
  };
  BoolVar: {};
  IntVar: {};
  LinearExpr: {};
  sumBoolVars(_0: BoolVarVector): LinearExpr;
  weightedSumBoolVars(_0: BoolVarVector, _1: Int64Vector): LinearExpr;
  Constraint: {};
  CpModelProto: {};
  CpModelBuilder: {
    new(): CpModelBuilder;
  };
  CpSolverStatus: {UNKNOWN: CpSolverStatusValue<0>, MODEL_INVALID: CpSolverStatusValue<1>, FEASIBLE: CpSolverStatusValue<2>, INFEASIBLE: CpSolverStatusValue<3>, OPTIMAL: CpSolverStatusValue<4>};
  CpSolverResponse: {};
  Model: {
    new(_0: (response: CpSolverResponse) => void, _1: (bound: number) => void, _2: boolean, _3: boolean, _4: boolean): Model;
  };
  solve(_0: CpModelProto): CpSolverResponse;
  solveWithModel(_0: CpModelProto, _1: Model | null): CpSolverResponse;
  solutionIntegerValueBoolVar(_0: CpSolverResponse, _1: BoolVar): bigint;
  solutionIntegerValueIntVar(_0: CpSolverResponse, _1: IntVar): bigint;
  solutionIntegerValueLinearExpr(_0: CpSolverResponse, _1: LinearExpr): bigint;
  newLinearExprBoolVar(_0: BoolVar): LinearExpr;
  newLinearExprIntVar(_0: IntVar): LinearExpr;
  newLinearExprConstant(_0: bigint): LinearExpr;
  getTotalMemory(): number;
  getFreeMemory(): number;
  getUsedMemory(): number;
}

export type MainModule = WasmModule & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
