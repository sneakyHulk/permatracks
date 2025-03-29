#include <iostream>
#include <unsupported/Eigen/LevenbergMarquardt>

extern "C" {
#include <test_model.h>
}

struct Functor : public Eigen::DenseFunctor<double> {
    Eigen::Vector<double, 3> B;
    // Constructor
    Functor(Eigen::Vector<double, 3> &&B) : Eigen::DenseFunctor<double>(3, 3), B(B) {
    }

    // Number of residuals
    int operator()(InputType const &x, ValueType &fvec) const {
        test_model(fvec.array().data(), nullptr, x(0), x(1), x(2));

        fvec -= B;

        return 0;
    }

    int df(InputType const &x, JacobianType &fjac) const {
        test_model_jacobian(nullptr, fjac.array().data(), x(0), x(1), x(2));

        return 0;
    }
};

int main() {
    Eigen::Vector<double, 3> B;
    B << 8., 8., 8.;
    Functor functor(std::move(B));
    Eigen::LevenbergMarquardt<Functor> lm(functor);

    lm.setXtol(1.0e-6);
    lm.setFtol(1.0e-6);

    Eigen::Vector<double, Eigen::Dynamic> x(3);
    x << 1.0, 1.0, 1.0; // Initial guess


    auto status = lm.minimize(x);

    std::cout << "Solver status: " << status << std::endl;
    std::cout << "Solution: " << x.transpose() << std::endl;
}
