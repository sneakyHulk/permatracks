import sympy as sp
import math
import numpy as np
import itertools
import matplotlib.pyplot as plt
import scipy.optimize
import scipy

x, y, z = sp.symbols('x, y, z', real=True)
f = sp.Matrix([x ** 2 + 1 + y, y ** 2 + 1, z ** 2 + 1])
print(f)
j = f.jacobian([x, y, z])
print(j)

lam_f = sp.utilities.lambdify([x, y, z], f.T.tolist()[0])
lam_f_jac = sp.utilities.lambdify([x, y, z], j.T.tolist())


def objective_function(x):
    return lam_f(*x)


def objective_function_jacobian(x):
    return np.array(lam_f_jac(*x))


print(lam_f(1, 2, 3))
print(lam_f_jac(1, 2, 3))

print(scipy.optimize.least_squares(objective_function, [1, 1, 1], method='lm', jac=objective_function_jacobian))
