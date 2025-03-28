from calibration import calibrate_using_coordinate_transform, compute_from_position_direction_values
from data_collection.scripts.collect_medability_sensor_array_data import collect, collect_position_direction_values, \
    get_sensor_position_values
import json
import numpy as np


class NumpyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return super().default(obj)


def calibrate():
    filepaths = ["../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h26min51s_calibration_x.txt",
                 "../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h39min41s_calibration_y.txt",
                 "../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h45min44s_calibration_z.txt"]

    measured_data = collect(filepaths)
    magnets_position_direction_values = collect_position_direction_values(filepaths)
    sensor_position_values = get_sensor_position_values()

    computed_data = compute_from_position_direction_values(sensor_position_values, magnets_position_direction_values[0])

    transforms = [calibrate_using_coordinate_transform(src, dst) for src, dst in
                  zip(measured_data, computed_data)]

    dictionary = dict(zip(range(len(transforms)), transforms))

    with open("../../result/current_calibration.json", "w") as outfile:
        json.dump(dictionary, outfile, cls=NumpyEncoder)


if __name__ == "__main__":
    calibrate()
