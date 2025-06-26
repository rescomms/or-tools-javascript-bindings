// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
declare namespace RuntimeExports {
    let HEAPF32: any;
    let HEAPF64: any;
    let HEAP_DATA_VIEW: any;
    let HEAP8: any;
    let HEAPU8: any;
    let HEAP16: any;
    let HEAPU16: any;
    let HEAP32: any;
    let HEAPU32: any;
    let HEAP64: any;
    let HEAPU64: any;
}
interface WasmModule {
}

export interface ClassHandle {
  isAliasOf(other: ClassHandle): boolean;
  delete(): void;
  deleteLater(): this;
  isDeleted(): boolean;
  clone(): this;
}
export interface BoolVarVector extends ClassHandle {
  size(): number;
  get(_0: number): BoolVar | undefined;
  push_back(_0: BoolVar): void;
  resize(_0: number, _1: BoolVar): void;
  set(_0: number, _1: BoolVar): boolean;
}

export interface IntVarVector extends ClassHandle {
  size(): number;
  get(_0: number): IntVar | undefined;
  push_back(_0: IntVar): void;
  resize(_0: number, _1: IntVar): void;
  set(_0: number, _1: IntVar): boolean;
}

export interface Int64Vector extends ClassHandle {
  push_back(_0: bigint): void;
  resize(_0: number, _1: bigint): void;
  size(): number;
  get(_0: number): bigint | undefined;
  set(_0: number, _1: bigint): boolean;
}

export interface Domain extends ClassHandle {
}

export interface BoolVar extends ClassHandle {
  not(): BoolVar;
}

export interface IntVar extends ClassHandle {
}

export interface LinearExpr extends ClassHandle {
  add(_0: LinearExpr): LinearExpr;
}

export interface Constraint extends ClassHandle {
  onlyEnforceIf(_0: BoolVar): Constraint;
}

export interface CpModelProto extends ClassHandle {
}

export interface CpModelBuilder extends ClassHandle {
  newBoolVar(): BoolVar;
  newIntVar(_0: Domain): IntVar;
  addLessOrEqual(_0: LinearExpr, _1: LinearExpr): Constraint;
  addLessThan(_0: LinearExpr, _1: LinearExpr): Constraint;
  addGreaterOrEqual(_0: LinearExpr, _1: LinearExpr): Constraint;
  addEquality(_0: LinearExpr, _1: LinearExpr): Constraint;
  addAllDifferent(_0: IntVarVector): Constraint;
  addBoolAnd(_0: BoolVarVector): Constraint;
  addBoolOr(_0: BoolVarVector): Constraint;
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
    new(_0: any): Model;
  };
  solve(_0: CpModelProto): CpSolverResponse;
  solveWithModel(_0: CpModelProto, _1: Model | null): CpSolverResponse;
  solutionIntegerValueBoolVar(_0: CpSolverResponse, _1: BoolVar): bigint;
  solutionIntegerValueIntVar(_0: CpSolverResponse, _1: IntVar): bigint;
  newLinearExprBoolVar(_0: BoolVar): LinearExpr;
  newLinearExprIntVar(_0: IntVar): LinearExpr;
  newLinearExprConstant(_0: bigint): LinearExpr;
}

export type MainModule = WasmModule & typeof RuntimeExports & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
