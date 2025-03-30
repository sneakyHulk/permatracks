import sympy as sp
import sys

### create variables and build functions:

x, y, z = sp.symbols('x, y, z', real=True)

fun = x + y + z
print("fun:", fun)

sys.stdin.readline()

### substitute variables with values:

fun2 = fun.subs(z, 3)
print("fun2:", fun2)

sys.stdin.readline()

### create lamda functions form functions:

lam_fun = sp.utilities.lambdify([x, y, z], fun)
lam_fun2 = sp.utilities.lambdify([x, y], fun2)

result = lam_fun(1, 2, 3)
print("lam_fun:", result)
result = lam_fun2(1, 2)
print("lam_fun2:", result)

sys.stdin.readline()

### compile sympy functions to C:

from sympy.utilities.codegen import codegen

out = sp.symbols('out', real=True)

eq = sp.Eq(out, fun)
eq2 = sp.Eq(out, fun2)
[(c_name, c_code), (h_name, h_code)] = codegen([("fun", eq), ("fun2", eq2)], language="C",
                                               project="sympy_compile_example", standard="C11", prefix="fun",
                                               argument_sequence=[out, x, y, z])

with open(c_name, "w") as c_file:
    c_file.write(c_code)

with open(h_name, "w") as h_file:
    h_file.write(h_code)

print(f"Generated files:\n- {c_name}\n- {h_name}")

sys.stdin.readline()

### create vector-valued functions:

fun3 = sp.Matrix([x ** 2, x + y, x + y + sp.cos(z)])
print("fun3:")
sp.pprint(fun3)

sys.stdin.readline()

### compute jacobian of vector-valued functions:

fun3_jacobian = fun3.jacobian([x, y, z])
print("fun3_jacobian:")
sp.pprint(fun3_jacobian)

sys.stdin.readline()

### compile vector-valued functions to C:

out = sp.MatrixSymbol('out', *fun3.shape)

eq = sp.Eq(out, fun3)
[(c_name, c_code), (h_name, h_code)] = codegen([("fun", eq)], language="C",
                                               project="sympy_compile_example", standard="C11", prefix="fun3",
                                               argument_sequence=[out, x, y, z])

with open(c_name, "w") as c_file:
    c_file.write(c_code)

with open(h_name, "w") as h_file:
    h_file.write(h_code)

print(f"Generated files:\n- {c_name}\n- {h_name}")

sys.stdin.readline()

### compile matrix-valued functions to C:

out = sp.MatrixSymbol('out', *fun3_jacobian.shape)

eq = sp.Eq(out, fun3_jacobian)
[(c_name, c_code), (h_name, h_code)] = codegen([("fun", eq)], language="C",
                                               project="sympy_compile_example", standard="C11", prefix="fun3",
                                               argument_sequence=[out, x, y, z])

with open(c_name, "w") as c_file:
    c_file.write(c_code)

with open(h_name, "w") as h_file:
    h_file.write(h_code)

print(f"Generated files:\n- {c_name}\n- {h_name}")

