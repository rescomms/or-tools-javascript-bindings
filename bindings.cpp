#include <emscripten/emscripten.h>
#include <emscripten/heap.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/threading.h>
#include <emscripten/proxying.h>
#include "ortools/sat/cp_model.h"
#include "ortools/sat/cp_model_solver.h"
#include <stdio.h>
#include <malloc.h>
#include <vector>
#include <future>
#include <utility>
#include <optional>

using namespace emscripten;
using namespace operations_research;
using namespace sat;

LinearExpr sumBoolVars(const std::vector<BoolVar>& vars) {
    return LinearExpr::Sum(vars);
}

LinearExpr weightedSumBoolVars(const std::vector<BoolVar>& vars, const std::vector<int64_t>& coeffs) {
    return LinearExpr::WeightedSum(vars, coeffs);
}

// WithName takes an absl::string_view (== std::string_view under C++17), which embind
// cannot marshal. Accept a std::string and let it convert implicitly.
BoolVar boolVarWithName(BoolVar& self, const std::string& name) {
    return self.WithName(name);
}

LinearExpr newLinearExprBoolVar(const BoolVar& var) {
    return LinearExpr(var);
}

LinearExpr newLinearExprIntVar(const IntVar& var) {
    return LinearExpr(var);
}

LinearExpr newLinearExprConstant(int64_t constant) {
    return LinearExpr(constant);
}

LinearExpr immutableAdd(const LinearExpr& first, const LinearExpr& second) {
    LinearExpr result;
    result += first;
    result += second;
    return result;
}

int64_t solutionIntegerValueBoolVar(const CpSolverResponse& response, const BoolVar& var) {
    return SolutionIntegerValue(response, var);
}

int64_t solutionIntegerValueIntVar(const CpSolverResponse& response, const IntVar& var) {
    return SolutionIntegerValue(response, var);
}

int64_t solutionIntegerValueLinearExpr(const CpSolverResponse& response, const LinearExpr& expr) {
    return SolutionIntegerValue(response, expr);
}

Constraint addAllDifferent(CpModelBuilder* builder, const std::vector<IntVar>& vars) {
    return builder->AddAllDifferent(vars);
}

Constraint addBoolAnd(CpModelBuilder* builder, const std::vector<BoolVar>& literals) {
    return builder->AddBoolAnd(literals);
}

Constraint addBoolOr(CpModelBuilder* builder, const std::vector<BoolVar>& literals) {
    return builder->AddBoolOr(literals);
}

Constraint onlyEnforceIfAll(Constraint& constraint, const std::vector<BoolVar>& literals) {
    return constraint.OnlyEnforceIf(literals);
}

static ProxyingQueue queue;

template <typename T>
auto mainThreadifyCallback(const val& callback) {
    return [callback=std::move(callback)](const T arg) {
        if (!emscripten_is_main_runtime_thread()) {
            bool proxied = queue.proxySync(emscripten_main_runtime_thread_id(), [&callback, &arg](){
                callback(arg);
            });
            assert(proxied);
            return;
        }
        callback(arg);
    };
}

EMSCRIPTEN_DECLARE_VAL_TYPE(SolutionCallback);
EMSCRIPTEN_DECLARE_VAL_TYPE(BoundCallback);
EMSCRIPTEN_DECLARE_VAL_TYPE(SolutionModelParameters);

// Creates a model that allows running a callback for each intermediate solution and best objective bound found. The callbacks must be syncronous
Model* newIntermediateSolutionModel(const SolutionCallback& solutionCallback, const BoundCallback& boundCallback, const SolutionModelParameters& options) {
    Model *model = new Model();
    if (!model) {
        throw "Model creation failed";
    }
    if (solutionCallback.typeOf().as<std::string>() != "function") {
        throw "Solution callback is not a function";
    }
    if (boundCallback.typeOf().as<std::string>() != "function") {
        throw "Bound callback is not a function";
    }

    auto runSolutionCallbackInMainThread = mainThreadifyCallback<CpSolverResponse>(solutionCallback);
    auto runBoundCallbackInMainThread = mainThreadifyCallback<double>(boundCallback);

    SatParameters params;
    if (options.hasOwnProperty("maxTime")) {
        params.set_max_time_in_seconds(options["maxTime"].as<int>());
    }
    params.set_fill_tightened_domains_in_response(options["enableDomainTightening"].as<bool>());
    params.set_log_search_progress(options["enableLogging"].as<bool>());
    model->Add(NewSatParameters(params));
    model->Add(NewFeasibleSolutionObserver(runSolutionCallbackInMainThread));
    model->Add(NewBestBoundCallback(runBoundCallbackInMainThread));
    return model;
}

CpSolverResponse solveWithModel(const CpModelProto& model_proto, Model* model) {
    // Run the solver in a new thread so that the main thread is free to execute callbacks
    std::future<CpSolverResponse> f = std::async(std::launch::async, &SolveCpModel, model_proto, model);
    // Keep executing callbacks until the solver has finished
    do {
        queue.execute();
    } while(f.wait_for(std::chrono::milliseconds(200)) != std::future_status::ready);
    return f.get();
}

size_t getTotalMemory() {
  return emscripten_get_heap_size();
}

size_t getFreeMemory() {
  struct mallinfo i = mallinfo();
  return i.fordblks;
}

size_t getUsedMemory() {
  struct mallinfo i = mallinfo();
  return i.uordblks;
}

EMSCRIPTEN_BINDINGS(std) {
    register_vector<BoolVar>("BoolVarVector");
    register_vector<IntVar>("IntVarVector");
    register_vector<int64_t>("Int64Vector");
}

EMSCRIPTEN_BINDINGS(variables) {
    class_<Domain>("Domain")
        .constructor<int64_t, int64_t>();
    
    class_<BoolVar>("BoolVar")
        .function("WithName", &boolVarWithName)
        .function("not", &BoolVar::Not)
        .function("Name", &BoolVar::Name);
    
    class_<IntVar>("IntVar");

    function("sumBoolVars", &sumBoolVars);
    function("weightedSumBoolVars", &weightedSumBoolVars);
}

EMSCRIPTEN_BINDINGS(model) {
    class_<LinearExpr>("LinearExpr")
        .function("mutableAdd", &LinearExpr::operator+=)
        .function("immutableAdd", &immutableAdd);
    
    class_<Constraint>("Constraint")
        .function("onlyEnforceIf", select_overload<Constraint(BoolVar)>(&Constraint::OnlyEnforceIf))
        .function("onlyEnforceIfAll", &onlyEnforceIfAll);
        
    class_<CpModelProto>("CpModelProto");
    
    class_<CpModelBuilder>("CpModelBuilder")
        .constructor<>()
        .function("newBoolVar", &CpModelBuilder::NewBoolVar)
        .function("newIntVar", &CpModelBuilder::NewIntVar)
        .function("addAssumption", &CpModelBuilder::AddAssumption)
        .function("addLessOrEqual", &CpModelBuilder::AddLessOrEqual)
        .function("addLessThan", &CpModelBuilder::AddLessThan)
        .function("addGreaterOrEqual", &CpModelBuilder::AddGreaterOrEqual)
        .function("addGreaterThan", &CpModelBuilder::AddGreaterThan)
        .function("addEquality", &CpModelBuilder::AddEquality)
        .function("addNotEqual", &CpModelBuilder::AddNotEqual)
        .function("addAllDifferent", &addAllDifferent, allow_raw_pointers())
        .function("addBoolAnd", &addBoolAnd, allow_raw_pointers())
        .function("addBoolOr", &addBoolOr, allow_raw_pointers())
        .function("addBoolVarHint", select_overload<void(BoolVar, bool)>(&CpModelBuilder::AddHint))
        .function("addIntVarHint", select_overload<void(IntVar, int64_t)>(&CpModelBuilder::AddHint))
        .function("clearAssumptions", &CpModelBuilder::ClearAssumptions)
        .function("clearHints", &CpModelBuilder::ClearHints)
        .function("maximize", select_overload<void(const LinearExpr&)>(&CpModelBuilder::Maximize))
        .function("build", &CpModelBuilder::Build);

    enum_<CpSolverStatus>("CpSolverStatus")
        .value("UNKNOWN", CpSolverStatus::UNKNOWN)
        .value("MODEL_INVALID", CpSolverStatus::MODEL_INVALID)
        .value("FEASIBLE", CpSolverStatus::FEASIBLE)
        .value("INFEASIBLE", CpSolverStatus::INFEASIBLE)
        .value("OPTIMAL", CpSolverStatus::OPTIMAL);

    class_<CpSolverResponse>("CpSolverResponse")
        .function("status", &CpSolverResponse::status)
        .function("objectiveValue", &CpSolverResponse::objective_value);
    
    class_<Model>("Model")
        .constructor(&newIntermediateSolutionModel, return_value_policy::take_ownership());

    register_type<SolutionCallback>("(response: CpSolverResponse) => void");
    register_type<BoundCallback>("(bound: number) => void");
    register_type<SolutionModelParameters>("{ enableLogging: boolean, enableDomainTightening: boolean, maxTime?: number }");

    function("solve", &Solve);
    function("solveWithModel", &solveWithModel, allow_raw_pointers());
    function("stopSearch", &stopSearch);
    function("solutionIntegerValueBoolVar", &solutionIntegerValueBoolVar);
    function("solutionIntegerValueIntVar", &solutionIntegerValueIntVar);
    function("solutionIntegerValueLinearExpr", &solutionIntegerValueLinearExpr);

    function("newLinearExprBoolVar", &newLinearExprBoolVar);
    function("newLinearExprIntVar", &newLinearExprIntVar);
    function("newLinearExprConstant", &newLinearExprConstant);

    function("getTotalMemory", &getTotalMemory);
    function("getFreeMemory", &getFreeMemory);
    function("getUsedMemory", &getUsedMemory);
}
