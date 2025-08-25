#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/threading.h>
#include <emscripten/proxying.h>
#include "ortools/sat/cp_model.h"
#include <stdio.h>
#include <vector>

using namespace emscripten;
using namespace operations_research;
using namespace sat;

LinearExpr sumBoolVars(const std::vector<BoolVar>& vars) {
    return LinearExpr::Sum(vars);
}

LinearExpr weightedSumBoolVars(const std::vector<BoolVar>& vars, const std::vector<int64_t>& coeffs) {
    return LinearExpr::WeightedSum(vars, coeffs);
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

int64_t solutionIntegerValueBoolVar(const CpSolverResponse& response, const BoolVar& var) {
    return SolutionIntegerValue(response, var);
}

int64_t solutionIntegerValueIntVar(const CpSolverResponse& response, const IntVar& var) {
    return SolutionIntegerValue(response, var);
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

static ProxyingQueue queue;

Model* newIntermediateSolutionModel(const val& callback, bool enableLogging) {
    Model *model = new Model();
    if (!model) {
        throw "Model creation failed";
    }
    if (callback.typeOf().as<std::string>() != "function") {
        throw "Callback is not a function";
    }

    auto runCallbackInMainThread = [&](const CpSolverResponse response){
        std::cout << "Running callback" << std::endl;
        if (!emscripten_is_main_runtime_thread()) {
            std::cout << "Not main thread" << std::endl;
            bool proxied = queue.proxySync(emscripten_main_runtime_thread_id(), [&](){
                std::cout << "Main thread activated" << std::endl;
                callback(response);
            });
            assert(proxied);
            return;
        }
        std::cout << "Main thread" << std::endl;
        callback(response);
    };

    SatParameters params;
    params.set_log_search_progress(enableLogging);
    std::cout << params.log_search_progress() << std::endl;
    std::cout << params.log_to_stdout() << std::endl;
    model->Add(NewSatParameters(params));
    model->Add(NewFeasibleSolutionObserver(runCallbackInMainThread));
    return model;
}

struct SolveArgs {
    const CpModelProto& model_proto;
    Model* model;
}

void* solveWithModel(void* arg) {
    auto args = (struct SolveArgs*)arg;
    CpSolverResponse result = SolveCpModel(args->model_proto, args->model);
    return result;
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
        .function("not", &BoolVar::Not);
    
    class_<IntVar>("IntVar");

    function("sumBoolVars", &sumBoolVars);
    function("weightedSumBoolVars", &weightedSumBoolVars);
}

EMSCRIPTEN_BINDINGS(model) {
    class_<LinearExpr>("LinearExpr")
        .function("add", &LinearExpr::operator+=);
    
    class_<Constraint>("Constraint")
        .function("onlyEnforceIf", select_overload<Constraint(BoolVar)>(&Constraint::OnlyEnforceIf));

    class_<CpModelProto>("CpModelProto");
    
    class_<CpModelBuilder>("CpModelBuilder")
        .constructor<>()
        .function("newBoolVar", &CpModelBuilder::NewBoolVar)
        .function("newIntVar", &CpModelBuilder::NewIntVar)
        .function("addLessOrEqual", &CpModelBuilder::AddLessOrEqual)
        .function("addLessThan", &CpModelBuilder::AddLessThan)
        .function("addGreaterOrEqual", &CpModelBuilder::AddGreaterOrEqual)
        .function("addEquality", &CpModelBuilder::AddEquality)
        .function("addAllDifferent", &addAllDifferent, allow_raw_pointers())
        .function("addBoolAnd", &addBoolAnd, allow_raw_pointers())
        .function("addBoolOr", &addBoolOr, allow_raw_pointers())
        .function("addBoolVarHint", select_overload<void(BoolVar, bool)>(&CpModelBuilder::AddHint))
        .function("addIntVarHint", select_overload<void(IntVar, int64_t)>(&CpModelBuilder::AddHint))
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

    function("solve", &Solve);
    function("solveWithModel", &SolveCpModel, allow_raw_pointers());
    function("solutionIntegerValueBoolVar", &solutionIntegerValueBoolVar);
    function("solutionIntegerValueIntVar", &solutionIntegerValueIntVar);

    function("newLinearExprBoolVar", &newLinearExprBoolVar);
    function("newLinearExprIntVar", &newLinearExprIntVar);
    function("newLinearExprConstant", &newLinearExprConstant);
}
