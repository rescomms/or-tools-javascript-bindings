#include <vector>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "ortools/sat/cp_model.h"
#include "ortools/sat/util.h"
#include <stdio.h>

using namespace emscripten;

operations_research::sat::LinearExpr sumBoolVars(const std::vector<operations_research::sat::BoolVar>& vars) {
    return operations_research::sat::LinearExpr::Sum(vars);
}

operations_research::sat::LinearExpr weightedSumBoolVars(const std::vector<operations_research::sat::BoolVar>& vars, const std::vector<int64_t>& coeffs) {
    return operations_research::sat::LinearExpr::WeightedSum(vars, coeffs);
}

operations_research::sat::LinearExpr newLinearExprBoolVar(const operations_research::sat::BoolVar& var) {
    return operations_research::sat::LinearExpr(var);
}

operations_research::sat::LinearExpr newLinearExprConstant(int64_t constant) {
    return operations_research::sat::LinearExpr(constant);
}

int64_t solutionIntegerValue(const operations_research::sat::CpSolverResponse& response, const operations_research::sat::BoolVar& var) {
    return operations_research::sat::SolutionIntegerValue(response, var);
}

operations_research::sat::Model* newIntermediateSolutionModel(const val& callBack) {
    operations_research::sat::Model *model = new operations_research::sat::Model();
    if (!model) {
        return nullptr;
    }
    if (callBack.typeOf().as<std::string>() != "function") {
        printf("Callback is not a function.\n");
        return nullptr;
    }
    model->Add(operations_research::sat::NewFeasibleSolutionObserver(callBack));
    return model;
}

EMSCRIPTEN_BINDINGS(stl) {
    register_vector<operations_research::sat::BoolVar>("BoolVarVector");
    register_vector<int64_t>("Int64Vector");

    function("sumBoolVars", &sumBoolVars);
    function("weightedSumBoolVars", &weightedSumBoolVars);

    function("newLinearExprBoolVar", &newLinearExprBoolVar);
    function("newLinearExprConstant", &newLinearExprConstant);
}

EMSCRIPTEN_BINDINGS(variables) {
    class_<operations_research::Domain>("Domain")
        .constructor<int64_t, int64_t>();
    
    class_<operations_research::sat::BoolVar>("BoolVar")
        .function("not", &operations_research::sat::BoolVar::Not);
    
    class_<operations_research::sat::IntVar>("IntVar");
}

EMSCRIPTEN_BINDINGS(model) {
    class_<operations_research::sat::LinearExpr>("LinearExpr")
        .function("add", &operations_research::sat::LinearExpr::operator+=);
    
    class_<operations_research::sat::Constraint>("Constraint")
        .function("onlyEnforceIf", select_overload<operations_research::sat::Constraint(operations_research::sat::BoolVar)>(&operations_research::sat::Constraint::OnlyEnforceIf));

    class_<operations_research::sat::CpModelProto>("CpModelProto");
    
    class_<operations_research::sat::CpModelBuilder>("CpModelBuilder")
        .constructor<>()
        .function("newBoolVar", &operations_research::sat::CpModelBuilder::NewBoolVar)
        .function("newIntVar", &operations_research::sat::CpModelBuilder::NewIntVar)
        .function("addLessOrEqual", &operations_research::sat::CpModelBuilder::AddLessOrEqual)
        .function("addLessThan", &operations_research::sat::CpModelBuilder::AddLessThan)
        .function("addGreaterOrEqual", &operations_research::sat::CpModelBuilder::AddGreaterOrEqual)
        .function("addEquality", &operations_research::sat::CpModelBuilder::AddEquality)
        .function("maximize", select_overload<void(const operations_research::sat::LinearExpr&)>(&operations_research::sat::CpModelBuilder::Maximize))
        .function("build", &operations_research::sat::CpModelBuilder::Build);

    enum_<operations_research::sat::CpSolverStatus>("CpSolverStatus")
        .value("UNKNOWN", operations_research::sat::CpSolverStatus::UNKNOWN)
        .value("MODEL_INVALID", operations_research::sat::CpSolverStatus::MODEL_INVALID)
        .value("FEASIBLE", operations_research::sat::CpSolverStatus::FEASIBLE)
        .value("INFEASIBLE", operations_research::sat::CpSolverStatus::INFEASIBLE)
        .value("OPTIMAL", operations_research::sat::CpSolverStatus::OPTIMAL);

    class_<operations_research::sat::CpSolverResponse>("CpSolverResponse")
        .function("status", &operations_research::sat::CpSolverResponse::status)
        .function("objectiveValue", &operations_research::sat::CpSolverResponse::objective_value);
    
    class_<operations_research::sat::Model>("Model");
    function("newIntermediateSolutionModel", &newIntermediateSolutionModel, return_value_policy::take_ownership());

    function("solve", &operations_research::sat::Solve);
    function("solveWithModel", &operations_research::sat::SolveCpModel, allow_raw_pointers());
    function("solutionIntegerValue", &solutionIntegerValue);
}
