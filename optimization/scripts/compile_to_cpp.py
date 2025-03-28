import sympy as sp
from sympy.codegen.rewriting import create_expand_pow_optimization
from models import dipol_model, test_model
from data_collection.collect_medability_sensor_array_data import get_sensor_position_values
from sympy.utilities.codegen import codegen


def compile_dipol_model():
    expand_opt = create_expand_pow_optimization(2)

    sensor_position_values = get_sensor_position_values()
    model, symbols = dipol_model(sensor_position_values)

    F = sp.Matrix(model)
    J = F.jacobian([symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'],
                    symbols['Gy'], symbols['Gz']])

    B = sp.MatrixSymbol('B', *F.shape)
    BB = sp.MatrixSymbol('BB', *J.shape)

    eq = sp.Eq(B, F)
    eq2 = sp.Eq(BB, J)

    [(c_name, c_code), (h_name, h_code)] = codegen([("dipol_model", eq), ("dipol_model_jacobian", eq2)], language="C",
                                                   project="SensorArrayDipolModel",
                                                   standard="C11", prefix="sensor_array_dipol_model",
                                                   argument_sequence=[B, BB, symbols['m1'], symbols['x1'],
                                                                      symbols['y1'],
                                                                      symbols['z1'], symbols['theta1'],
                                                                      symbols['phi1'], symbols['Gx'], symbols['Gy'],
                                                                      symbols['Gz']])

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
    compile_test_model()
