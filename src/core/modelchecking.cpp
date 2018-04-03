#include "modelchecking.h"
#include "storm/api/storm.h"
#include "storm/environment/Environment.h"
#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"

template<typename ValueType>
using CheckTask = storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>;

// Thin wrapper for model checking
template<typename ValueType>
std::shared_ptr<storm::modelchecker::CheckResult> modelCheckingSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, CheckTask<ValueType> const& task) {
    return storm::api::verifyWithSparseEngine<ValueType>(model, task);
}


std::vector<double> computeTransientProbabilities(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Ctmc<double>> ctmc, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, double timeBound) {
    return storm::modelchecker::helper::SparseCtmcCslHelper::computeAllTransientProbabilities(env, ctmc->getTransitionMatrix(), ctmc->getBackwardTransitions(), ctmc->getInitialStates(), phiStates, psiStates, ctmc->getExitRateVector(), timeBound);
}


// Thin wrapper for computing prob01 states
template<typename ValueType>
std::pair<storm::storage::BitVector, storm::storage::BitVector> computeProb01(storm::models::sparse::Dtmc<ValueType> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
    return storm::utility::graph::performProb01(model, phiStates, psiStates);
}

template<typename ValueType>
std::pair<storm::storage::BitVector, storm::storage::BitVector> computeProb01min(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
    return storm::utility::graph::performProb01Min(model, phiStates, psiStates);
}

template<typename ValueType>
std::pair<storm::storage::BitVector, storm::storage::BitVector> computeProb01max(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
    return storm::utility::graph::performProb01Max(model, phiStates, psiStates);
}

// Define python bindings
void define_modelchecking(py::module& m) {

    // CheckTask
    py::class_<CheckTask<double>, std::shared_ptr<CheckTask<double>>>(m, "CheckTask", "Task for model checking")
    //m.def("create_check_task", &storm::api::createTask, "Create task for verification", py::arg("formula"), py::arg("only_initial_states") = false);
        .def(py::init<storm::logic::Formula const&, bool>(), py::arg("formula"), py::arg("only_initial_states") = false)
        .def("set_produce_schedulers", &CheckTask<double>::setProduceSchedulers, "Set whether schedulers should be produced (if possible)", py::arg("produce_schedulers") = true)
    ;
    py::class_<CheckTask<storm::RationalFunction>, std::shared_ptr<CheckTask<storm::RationalFunction>>>(m, "ParametricCheckTask", "Task for parametric model checking")
    //m.def("create_check_task", &storm::api::createTask, "Create task for verification", py::arg("formula"), py::arg("only_initial_states") = false);
        .def(py::init<storm::logic::Formula const&, bool>(), py::arg("formula"), py::arg("only_initial_states") = false)
        .def("set_produce_schedulers", &CheckTask<storm::RationalFunction>::setProduceSchedulers, "Set whether schedulers should be produced (if possible)", py::arg("produce_schedulers") = true)
    ;

    // Model checking
    m.def("_model_checking_sparse_engine", &modelCheckingSparseEngine<double>, "Perform model checking", py::arg("model"), py::arg("task"));
    m.def("_parametric_model_checking_sparse_engine", &modelCheckingSparseEngine<storm::RationalFunction>, "Perform parametric model checking", py::arg("model"), py::arg("task"));
    m.def("compute_transient_probabilities", &computeTransientProbabilities, "Compute transient probabilities");
    m.def("_compute_prob01states_double", &computeProb01<double>, "Compute prob-0-1 states", py::arg("model"), py::arg("phi_states"), py::arg("psi_states"));
    m.def("_compute_prob01states_rationalfunc", &computeProb01<storm::RationalFunction>, "Compute prob-0-1 states", py::arg("model"), py::arg("phi_states"), py::arg("psi_states"));
    m.def("_compute_prob01states_min_double", &computeProb01min<double>, "Compute prob-0-1 states (min)", py::arg("model"), py::arg("phi_states"), py::arg("psi_states"));
    m.def("_compute_prob01states_max_double", &computeProb01max<double>, "Compute prob-0-1 states (max)", py::arg("model"), py::arg("phi_states"), py::arg("psi_states"));
    m.def("_compute_prob01states_min_rationalfunc", &computeProb01min<storm::RationalFunction>, "Compute prob-0-1 states (min)", py::arg("model"), py::arg("phi_states"), py::arg("psi_states"));
    m.def("_compute_prob01states_max_rationalfunc", &computeProb01max<storm::RationalFunction>, "Compute prob-0-1 states (max)", py::arg("model"), py::arg("phi_states"), py::arg("psi_states"));
}
