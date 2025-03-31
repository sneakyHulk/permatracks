from calibration import calibrate_using_coordinate_transform_ransac, calibrate_using_coordinate_transform, \
    compute_from_position_direction_values
from data_collection.scripts.collect_medability_sensor_array_data import collect, collect_position_direction_values, \
    get_sensor_position_values
import json
import numpy as np
import warnings


class NumpyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return super().default(obj)


def calibrate():
    filepaths = [
        "../../result/LIS3MDL_ARRAY_Besprechungsraum/mag_data_LIS3MDL_ARRAY_mean_2025Mar28_16h45min11s_calibration_pattern.txt"]
    measured_data = collect(filepaths)
    magnets_position_direction_values = collect_position_direction_values(filepaths)
    sensor_position_values = get_sensor_position_values()
    computed_data = compute_from_position_direction_values(sensor_position_values, magnets_position_direction_values[0])

    transforms = np.array([calibrate_using_coordinate_transform(src, dst) for src, dst in
                           zip(measured_data, computed_data)])

    transforms = np.where(np.isnan(transforms), None, transforms)

    dictionary = {key: {'transformation': transform, 'sensor_position': sensor_position_value} for
                  key, transform, sensor_position_value in
                  zip(["{:02}".format(n) for n in range(len(transforms))], transforms, sensor_position_values)}

    with open("../../result/current_calibration.json", "w") as outfile:
        json.dump(dictionary, outfile, cls=NumpyEncoder)


if __name__ == "__main__":
    calibrate()
