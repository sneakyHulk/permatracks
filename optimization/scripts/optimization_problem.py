import sympy as sp
import math
import numpy as np
import itertools
import scipy.optimize
import scipy
import pyswarms as ps


def optimize_particle_swarm(F, symbols, B):
    H = 4e-3
    R = 3e-3
    m = 1.35 / 4 * H * R ** 2

    F = sp.Matrix(F).subs(symbols['m1'], m)

    lam_F = sp.utilities.lambdify(
        [symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'], symbols['Gy'],
         symbols['Gz']], F.T.tolist()[0], 'numpy')

    swarm_size = 100
    options = {'c1': 1.5, 'c2': 1.5, 'w': 0.5}

    constraints = (np.array([0, 0, 0, -180 * math.pi / 180, -180 * math.pi / 180, -50e-6, -50e-6, -50e-6]),
                   np.array([210e-3, 210e-3, 210e-3, 180 * math.pi / 180, 180 * math.pi / 180, 50e-6, 50e-6, 50e-6]))

    def objective_function(X):
        return np.linalg.vector_norm(np.array([lam_F(*x) for x in X]) - B, axis=1)

    optimizer = ps.single.GlobalBestPSO(n_particles=swarm_size,
                                        dimensions=8,
                                        options=options,
                                        bounds=constraints)

    cost, x = optimizer.optimize(objective_function, iters=2000)

    return x


def optimize_least_squares_levenberg_marquardt(F, symbols, B):
    H = 4e-3
    R = 3e-3
    m = 1.35 / 4 * H * R ** 2

    print(sp.ccode(F))
    F = sp.Matrix(F).subs(symbols['m1'], m)

    lam_F = sp.utilities.lambdify(
        [symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'], symbols['Gy'],
         symbols['Gz']], F.T.tolist()[0], 'numpy')
    lam_J = sp.utilities.lambdify(
        [symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'], symbols['Gy'],
         symbols['Gz']],
        F.jacobian([symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'],
                    symbols['Gy'], symbols['Gz']]).T.tolist(), 'numpy')

    # print(np.array(lam_F(37.5e-3, 30e-3, 55e-3, 90 * math.pi / 180, 0 * math.pi / 180, 0, 0, 0)))
    # print(np.array(lam_J(37.5e-3, 30e-3, 55e-3, 90 * math.pi / 180, 0 * math.pi / 180, 0, 0, 0)))

    def objective_function(X, B):
        return np.array(lam_F(*X)) - B

    def objective_function_jacobian(X, B):
        return np.transpose(lam_J(*X))

    res = scipy.optimize.least_squares(objective_function,
                                       [210e-3 / 2, 210e-3 / 2, 55e-3, 0, 0, 0, 0, 0],
                                       method='lm', jac=objective_function_jacobian, args=(B,))

    return res.x


from calibration.scripts.calibration import compute_from_position_direction_values, \
    calibrate_using_coordinate_transform_ransac, \
    calibrate_using_coordinate_transform, test_calibration
from optimization.scripts.models import dipol_model
from data_collection.scripts.collect_medability_sensor_array_data import collect, collect_position_direction_values, \
    get_sensor_position_values
import matplotlib.pyplot as plt


def test_optimization1():
    sensor_position_values = get_sensor_position_values()
    model, symbols = dipol_model(sensor_position_values)

    B = compute_from_position_direction_values(sensor_position_values,
                                               [[30e-3, 30e-3, 55e-3, 90 * math.pi / 180,
                                                 0 * math.pi / 180]])

    optimize_least_squares_levenberg_marquardt(model, symbols, B.flatten())


def test_optimization2():
    sensor_position_values = get_sensor_position_values()
    model, symbols = dipol_model(sensor_position_values)

    transforms = test_calibration()

    measurements_for_each_sensor = collect(
        ["../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h32min12s_(1-10_3).txt"])

    transformed_measurements_for_each_sensor = np.array(
        [[np.dot(transform, np.append(measurement, 1))[0:3] for measurement in measurements_for_one_sensor] for
         transform, measurements_for_one_sensor in zip(transforms, measurements_for_each_sensor)])
    transformed_sensor_data_for_each_measurement = np.swapaxes(transformed_measurements_for_each_sensor, 0, 1)

    optimized_positions_direction_values = np.array(
        [optimize_least_squares_levenberg_marquardt(model, symbols,
                                                    transformed_sensor_data_for_one_measurement.flatten())
         for transformed_sensor_data_for_one_measurement in
         transformed_sensor_data_for_each_measurement])

    ground_truth_positions_direction_values = collect_position_direction_values(
        ["../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h32min12s_(1-10_3).txt"])[0]

    print(optimized_positions_direction_values[:, 3:8])

    err = np.abs(optimized_positions_direction_values[:, 0:3] - ground_truth_positions_direction_values[:, 0:3])

    fig, ax = plt.subplots()
    ax.plot(np.linspace(1, 10, 10), err[:, 0] * 1e3)
    ax.plot(np.linspace(1, 10, 10), err[:, 1] * 1e3)
    ax.plot(np.linspace(1, 10, 10), err[:, 2] * 1e3)
    ax.legend(["x", "y", "z"])
    ax.set_title("Position error in x, y and z direction")
    ax.set_xlabel("Measurements")
    ax.set_ylabel("Measurement error [mm]")
    plt.show()


if __name__ == '__main__':
    test_optimization1()
    test_optimization2()
