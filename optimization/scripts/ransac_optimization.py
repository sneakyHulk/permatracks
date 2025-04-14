import sympy as sp
import math
import numpy as np
import itertools
import scipy.optimize
import scipy
import pyswarms as ps


def ransac_optimization(F, symbols, B, init=[210e-3 / 2, 210e-3 / 2, 15e-2 / 2, 0, 0, 0, 0, 0]):
    H = 4e-3
    R = 3e-3
    m = 1.35 / 4 * H * R ** 2

    F = sp.Matrix(F).subs(symbols['m1'], m)

    lam_F = sp.utilities.lambdify(
        [symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'], symbols['Gy'],
         symbols['Gz']], F.T.tolist()[0], 'numpy')
    lam_J = sp.utilities.lambdify(
        [symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'], symbols['Gy'],
         symbols['Gz']],
        F.jacobian([symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1'], symbols['Gx'],
                    symbols['Gy'], symbols['Gz']]).T.tolist(), 'numpy')

    def objective_function(X, B, mask):
        return (np.array(lam_F(*X)) - B)[np.repeat(mask, 3)]

    def objective_function_jacobian(X, B, mask):
        return np.transpose(lam_J(*X))[np.repeat(mask, 3)]

    res = scipy.optimize.least_squares(objective_function,
                                       init,
                                       method='lm', jac=objective_function_jacobian, args=(B,np.array([False, False, False, False, False, False, False, False, False, False, False, True, True, True, True, True])))

    return res.x

from calibration.scripts.calibration import compute_from_position_direction_values, calibrate
from optimization.scripts.models import dipol_model
from data_collection.scripts.collect_medability_sensor_array_data import collect, collect_position_direction_values, \
    get_sensor_position_values
import matplotlib.pyplot as plt
from tqdm import tqdm

def optimize(filepaths_calibration, filepaths_ground_truth):
    sensor_position_values = get_sensor_position_values()
    model, symbols = dipol_model(sensor_position_values)

    transforms = calibrate(filepaths_calibration)

    measurements_for_each_sensor = collect(filepaths_ground_truth)

    transformed_measurements_for_each_sensor = np.array(
        [[np.dot(transform, np.append(measurement, 1))[0:3] for measurement in measurements_for_one_sensor] for
         transform, measurements_for_one_sensor in zip(transforms, measurements_for_each_sensor)])
    transformed_sensor_data_for_each_measurement = np.swapaxes(transformed_measurements_for_each_sensor, 0, 1)[42:100]

    optimized_positions_direction_values = np.array(
        [ransac_optimization(model, symbols,
                                 transformed_sensor_data_for_one_measurement.flatten())
         for transformed_sensor_data_for_one_measurement in
         tqdm(transformed_sensor_data_for_each_measurement)])

    ground_truth_positions_direction_values = collect_position_direction_values(filepaths_ground_truth)[0][42:100]

    print(optimized_positions_direction_values[:, 3:8])

    fig, ax = plt.subplots()
    ax.plot(optimized_positions_direction_values[:, 0:3] - ground_truth_positions_direction_values[:, 0:3])
    ax.legend(["x", "y", "z"])
    ax.set_title("Optimization error")
    ax.set_xlabel("Iteration")
    ax.set_ylim(-0.05, 0.05)
    ax.set_ylabel("Position error [m]")
    plt.show()


if __name__ == '__main__':
    filepaths = ["../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h26min51s_calibration_x.txt",
                 "../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h39min41s_calibration_y.txt",
                 "../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h45min44s_calibration_z.txt"]
    ground_truth = ["../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h32min12s_(1-10_3).txt"]

    filepaths = [
        "../../result/LIS3MDL_ARRAY_Besprechungsraum/2/mag_data_LIS3MDL_ARRAY_mean_2025Apr01_14h38min28s_calibration_pattern.txt"]
    ground_truth = [
        "../../result/LIS3MDL_ARRAY_Besprechungsraum/2/mag_data_LIS3MDL_ARRAY_2025Apr01_14h42min53s_ground_truth_data.txt"]

    filepaths = [
        "../../result/LIS3MDL_ARRAY_Besprechungsraum/1/mag_data_LIS3MDL_ARRAY_mean_2025Mar28_16h45min11s_calibration_pattern.txt"]
    ground_truth = [
        "../../result/LIS3MDL_ARRAY_Besprechungsraum/1/mag_data_LIS3MDL_ARRAY_2025Mar28_17h22min51s_ground_truth_data.txt"]

    optimize(filepaths, ground_truth)
