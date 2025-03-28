import csv
import numpy as np
import matplotlib.pyplot as plt
from skimage.measure import ransac
from skimage.transform import AffineTransform


def calibrate_using_coordinate_transform(src, dst):
    model = AffineTransform(dimensionality=3)
    model.estimate(src, dst)

    print("Affine transform:")
    print(model.params)

    return model.params


def calibrate_using_coordinate_transform_ransac(src, dst):
    class AffineTransform3D(AffineTransform):
        def __init__(self):
            super().__init__(dimensionality=3)

    model, inliers = ransac((src, dst), AffineTransform3D, min_samples=4, residual_threshold=1e-6, max_trials=1000)
    outliers = inliers == False

    print("RANSAC Affine transform:")
    print(model.params)
    print("Outliers: ", outliers)

    return model.params


from optimization.scripts.models import dipol_model
import sympy as sp
import itertools


def compute_from_position_direction_values(sensor_position_values, magnet_position_direction_values):
    model, symbols = dipol_model(sensor_position_values)

    H = 4e-3
    R = 3e-3
    m = 1.35 / 4 * H * R ** 2

    F = sp.Matrix(model).subs({symbols['m1']: m, symbols['Gx']: 0, symbols['Gy']: 0, symbols['Gz']: 0})

    lam_F = sp.utilities.lambdify([symbols['x1'], symbols['y1'], symbols['z1'], symbols['theta1'], symbols['phi1']],
                                  F.T.tolist()[0], 'numpy')

    computed_data = np.array(
        [[[x, y, z] for x, y, z in itertools.batched(lam_F(*magnet_position_direction_value), 3)] for
         magnet_position_direction_value in magnet_position_direction_values])
    computed_data = np.swapaxes(computed_data, 0, 1)

    return computed_data


from data_collection.scripts.collect_medability_sensor_array_data import collect, collect_position_direction_values, \
    get_sensor_position_values

def test_calibration():
    filepaths = ["../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h26min51s_calibration_x.txt",
                 "../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h39min41s_calibration_y.txt",
                 "../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h45min44s_calibration_z.txt"]

    measured_data = collect(filepaths)
    magnets_position_direction_values = collect_position_direction_values(filepaths)
    sensor_position_values = get_sensor_position_values()

    computed_data = compute_from_position_direction_values(sensor_position_values, magnets_position_direction_values[0])

    for src, dst in zip(measured_data, computed_data):
        transform = calibrate_using_coordinate_transform(src, dst)

        for measurement in src:
            print(np.dot(transform, np.append(measurement, 1)))

    for src, dst in zip(measured_data, computed_data):
        transform = calibrate_using_coordinate_transform_ransac(src, dst)

        for measurement in src:
            print(np.dot(transform, np.append(measurement, 1)))

    transforms = [calibrate_using_coordinate_transform(src, dst) for src, dst in
                  zip(measured_data, computed_data)]

    return transforms

if __name__ == "__main__":
    test_calibration()


