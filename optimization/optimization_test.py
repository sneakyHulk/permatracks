import sympy as sp
import math
import numpy as np
import itertools
import matplotlib.pyplot as plt
import scipy.optimize
import scipy

import cProfile

x, y, z = sp.symbols('x, y, z', real=True)
f = sp.Matrix([x ** 2 - 1, y ** 2 - 1, z ** 2 - 1])
print(f)
j = f.jacobian([x, y, z])
print(j)

lam_f = sp.utilities.lambdify([x, y, z], f.T.tolist()[0])
lam_f_jac = sp.utilities.lambdify([x, y, z], j.T.tolist())


### Levenberg–Marquardt
def objective_function(x):
    return lam_f(*x) - np.array([8, 8, 8])


def objective_function_jacobian(x):
    return np.array(lam_f_jac(*x))


def run1():
    res = scipy.optimize.least_squares(objective_function, [1, 1, 1], method='lm', jac=objective_function_jacobian)
    print("x:", res.x)
    print("y:", np.array(lam_f(*res.x)))


cProfile.run('run1()')

import pyswarms as ps

swarm_size = 20
options = {'c1': 1.5, 'c2': 1.5, 'w': 0.5}

constraints = (np.array([0, 0, 0]),
               np.array([10, 10, 10]))


def opt_func(X):
    target = np.array([8, 8, 8])
    dist = np.linalg.vector_norm([lam_f(*x) for x in X] - target, axis=1)
    return np.array(dist)


optimizer = ps.single.GlobalBestPSO(n_particles=swarm_size,
                                    dimensions=3,
                                    options=options,
                                    bounds=constraints)

def run2():
    cost, x = optimizer.optimize(opt_func, iters=10)

    print("x:", x)
    print("y:", np.array(lam_f(*x)))

cProfile.run('run2()')


