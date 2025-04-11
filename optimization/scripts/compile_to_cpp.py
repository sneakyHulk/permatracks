import math

import sympy as sp
from sympy.codegen.rewriting import create_expand_pow_optimization
from models import dipol_model, test_model
from data_collection.scripts.collect_medability_sensor_array_data import get_sensor_position_values
from sympy.utilities.codegen import codegen
from sympy.printing.cxx import CXX17CodePrinter
from sympy.codegen.rewriting import create_expand_pow_optimization

from sympy.utilities.codegen import codegen, Routine
import itertools


def compile_dipol_model():
    sensor_position_values = get_sensor_position_values()
    model, symbols = dipol_model(sensor_position_values)

    F = sp.Matrix(model)
    J = F.jacobian([symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'],
                    symbols['Gy'], symbols['Gz']])

    # B = sp.MatrixSymbol('B', *F.shape)
    # BB = sp.MatrixSymbol('BB', *J.shape)
    #
    # eq = sp.Eq(B, F)
    # eq2 = sp.Eq(BB, J)
    #
    # [(c_name, c_code), (h_name, h_code)] = codegen([("dipol_model", eq), ("dipol_model_jacobian", eq2)], language="C",
    #                                               project="SensorArrayDipolModel",
    #                                               standard="C11", prefix="sensor_array_dipol_model",
    #                                               argument_sequence=[B, BB, symbols['m1'], symbols['x1'],
    #                                                                  symbols['y1'],
    #                                                                  symbols['z1'], symbols['theta1'],
    #                                                                  symbols['phi1'], symbols['Gx'], symbols['Gy'],
    #                                                                  symbols['Gz']])

    def compile(name, symbols, F: sp.Matrix, J: sp.Matrix):
        tmp_gen = sp.numbered_symbols(prefix='tmp')
        cxx_printer = CXX17CodePrinter()
        expand_opt = create_expand_pow_optimization(3)

        h_name = name + ".h"
        c_name = name + ".cpp"

        h_code = f"#pragma once\n"
        c_code = f"#include \"{name}.h\"\n"
        c_code += "#include <math.h>\n"

        # Dipol model:

        input_values = [f"double {sym}" for sym in symbols.values() if sym in F.free_symbols]
        output_values = [f"double *out"]

        h_code += f"void {name}({", ".join(output_values + input_values)});\n"
        c_code += f"void {name}({", const ".join(output_values + input_values)}) {{\n"

        replacements, reduced_expr = sp.cse(F, symbols=tmp_gen)

        c_code += "".join(
            [f"\tdouble const {sym} = {cxx_printer.doprint(expand_opt(expr))};\n" for sym, expr in replacements])
        c_code += "".join(
            [f"\tout[{i}] = {cxx_printer.doprint(expand_opt(reduced_expr[0][i]))};\n" for i in
             range(math.prod(F.shape))])
        c_code += "}\n"

        # Jacobian of dipol model:

        input_values = [f"double {sym}" for sym in symbols.values() if sym in J.free_symbols]
        output_values = [f"double *out"]

        h_code += f"void {name}_jacobian({", ".join(output_values + input_values)});\n"
        c_code += f"void {name}_jacobian({", const ".join(output_values + input_values)}) {{\n"

        replacements, reduced_expr = sp.cse(J, symbols=tmp_gen)

        c_code += "".join(
            [f"\tdouble const {sym} = {cxx_printer.doprint(expand_opt(expr))};\n" for sym, expr in replacements])
        c_code += "".join(
            [f"\tout[{i}] = {cxx_printer.doprint(expand_opt(reduced_expr[0][i]))};\n" for i in
             range(math.prod(J.shape))])
        c_code += "}\n"

        return [(c_name, c_code), (h_name, h_code)]

    [(c_name, c_code), (h_name, h_code)] = compile("dipol_model", symbols=symbols, F=F, J=J)

    with open("../src/" + c_name, "w") as c_file:
        c_file.write(c_code)

    with open("../include/" + h_name, "w") as h_file:
        h_file.write(h_code)

    # Output file names
    print(f"Generated files:\n- {c_name}\n- {h_name}")


def compile_test_model():
    model, symbols = test_model()

    F = sp.Matrix(model)
    J = F.jacobian([symbols['x'], symbols['y'], symbols['z']])

    B = sp.MatrixSymbol('B', *F.shape)
    BB = sp.MatrixSymbol('BB', *J.shape)

    eq = sp.Eq(B, F)
    eq2 = sp.Eq(BB, J)

    [(c_name, c_code), (h_name, h_code)] = codegen([("test_model", eq), ("test_model_jacobian", eq2)], language="C",
                                                   project="TestModel", standard="C11", prefix="test_model",
                                                   argument_sequence=[B, BB, symbols['x'], symbols['y'], symbols['z']])

    with open("../src/" + c_name, "w") as c_file:
        c_file.write(c_code)

    with open("../include/" + h_name, "w") as h_file:
        h_file.write(h_code)

    # Output file names
    print(f"Generated files:\n- {c_name}\n- {h_name}")


if __name__ == "__main__":
    compile_dipol_model()
    compile_test_model()
