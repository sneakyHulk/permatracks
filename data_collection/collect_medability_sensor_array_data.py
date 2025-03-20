import csv
import itertools

import numpy as np
import math


def collect(filepaths):
    def collect_single(filepath):
        with open(filepath) as file:
            reader = csv.DictReader(file)

            def relevant_fieldnames(fieldnames):
                for i in itertools.count(0):
                    if 'x' + str(i) in fieldnames and 'y' + str(i) in fieldnames and 'z' + str(i) in fieldnames:
                        yield ['x' + str(i), 'y' + str(i), 'z' + str(i)]
                    else:
                        break

            data = np.array([[[float(row[y]), float(row[x]), float(row[z])] for x, y, z in
                              relevant_fieldnames(reader.fieldnames)] for row in
                             itertools.islice(reader, 1, None)])  # x and y changed in medability array code
            data = np.swapaxes(data, 0, 1)

            return data * 1e-6

    data = np.concat([collect_single(filepath) for filepath in filepaths], axis=1)
    return data


def collect_position_direction_values(filepaths):
    def collect_single(filepath):
        with open(filepath) as file:
            reader = csv.DictReader(file)

            def relevant_fieldnames(fieldnames):
                for i in itertools.count(0):
                    if 'mag_x' + str(i) in fieldnames and 'mag_y' + str(i) in fieldnames and 'mag_z' + str(
                            i) in fieldnames and 'mag_theta' + str(i) in fieldnames and 'mag_phi' + str(
                        i) in fieldnames:
                        yield ['mag_x' + str(i), 'mag_y' + str(i), 'mag_z' + str(i), 'mag_theta' + str(i),
                               'mag_phi' + str(i)]
                    else:
                        break

            data = np.array([[[float(row[mag_x]), float(row[mag_y]), float(row[mag_z]), float(row[mag_theta]),
                               float(row[mag_phi])] for mag_x, mag_y, mag_z, mag_theta, mag_phi in
                              relevant_fieldnames(reader.fieldnames)] for row in
                             itertools.islice(reader, 1, None)])  # x and y changed in medability array code
            data = np.swapaxes(data, 0, 1)

            return data

    data = np.concat([collect_single(filepath) for filepath in filepaths], axis=1)
    return data


def get_holes_position_direction_values(holes_descriptions):
    with open("../result/test.csv", "w") as file:
        writer = csv.DictWriter(file,
                                fieldnames=["mag_x0", "mag_y0", "mag_z0", "mag_theta0", "mag_phi0"])

        writer.writeheader()
        writer.writerow({'mag_x0': None, 'mag_y0': None, 'mag_z0': None, 'mag_theta0': None, 'mag_phi0': None})
        for holes_description in holes_descriptions:
            if holes_description['holes_facing'] == 'x':
                writer.writerow({'mag_x0': 37.5e-3 + (holes_description['x_pos'] - 1) * 15e-3,
                                 "mag_y0": 30e-3 + (holes_description['y_pos'] - 1) * 15e-3, 'mag_z0': 55e-3,
                                 'mag_theta0': 90 * math.pi / 180 if holes_description[
                                     'in_positive_direction'] else -90 * math.pi / 180,
                                 'mag_phi0': 0})
            elif holes_description['holes_facing'] == 'y':
                writer.writerow({'mag_x0': 30e-3 + (holes_description['x_pos'] - 1) * 15e-3,
                                 "mag_y0": 37.5e-3 + (holes_description['y_pos'] - 1) * 15e-3, 'mag_z0': 55e-3,
                                 'mag_theta0': 90 * math.pi / 180,
                                 'mag_phi0': 90 * math.pi / 180 if holes_description[
                                     'in_positive_direction'] else -90 * math.pi / 180})
            elif holes_description['holes_facing'] == 'z':
                writer.writerow({'mag_x0': 30e-3 + (holes_description['x_pos'] - 1) * 15e-3,
                                 "mag_y0": 30e-3 + (holes_description['y_pos'] - 1) * 15e-3, 'mag_z0': 55e-3,
                                 'mag_theta0': 0 if holes_description['in_positive_direction'] else 180 * math.pi / 180,
                                 'mag_phi0': 0})
            else:
                raise ValueError("Invalid holes_facing")


def get_sensor_position_values(rows=4, columns=4, stride=30e-3, start=(40e-3, 45e-3)):
    return np.array([[start[0] + row * stride, start[1] + column * stride, 0] for row, column in
                     itertools.product(range(rows), range(columns))])


if __name__ == '__main__':
    filepaths = ["../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h26min51s_calibration_x.txt",
                 "../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h39min41s_calibration_y.txt",
                 "../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h45min44s_calibration_z.txt"]

    print(collect(filepaths))
    print(collect_position_direction_values(filepaths))

    get_sensor_position_values(4, 4, 30e-3, (40e-3, 45e-3))

    holes_descriptions_x = [{'holes_facing': 'x', 'x_pos': 1, 'y_pos': 1, 'in_positive_direction': True},
                            {'holes_facing': 'x', 'x_pos': 1, 'y_pos': 1, 'in_positive_direction': False},
                            {'holes_facing': 'x', 'x_pos': 10, 'y_pos': 1, 'in_positive_direction': True},
                            {'holes_facing': 'x', 'x_pos': 10, 'y_pos': 1, 'in_positive_direction': False},
                            {'holes_facing': 'x', 'x_pos': 10, 'y_pos': 11, 'in_positive_direction': True},
                            {'holes_facing': 'x', 'x_pos': 10, 'y_pos': 11, 'in_positive_direction': False},
                            {'holes_facing': 'x', 'x_pos': 1, 'y_pos': 11, 'in_positive_direction': True},
                            {'holes_facing': 'x', 'x_pos': 1, 'y_pos': 11, 'in_positive_direction': False},
                            {'holes_facing': 'x', 'x_pos': 5, 'y_pos': 6, 'in_positive_direction': True},
                            {'holes_facing': 'x', 'x_pos': 5, 'y_pos': 6, 'in_positive_direction': False}]

    holes_descriptions_y = [{'holes_facing': 'y', 'x_pos': 1, 'y_pos': 1, 'in_positive_direction': True},
                            {'holes_facing': 'y', 'x_pos': 1, 'y_pos': 1, 'in_positive_direction': False},
                            {'holes_facing': 'y', 'x_pos': 11, 'y_pos': 1, 'in_positive_direction': True},
                            {'holes_facing': 'y', 'x_pos': 11, 'y_pos': 1, 'in_positive_direction': False},
                            {'holes_facing': 'y', 'x_pos': 11, 'y_pos': 10, 'in_positive_direction': True},
                            {'holes_facing': 'y', 'x_pos': 11, 'y_pos': 10, 'in_positive_direction': False},
                            {'holes_facing': 'y', 'x_pos': 1, 'y_pos': 10, 'in_positive_direction': True},
                            {'holes_facing': 'y', 'x_pos': 1, 'y_pos': 10, 'in_positive_direction': False},
                            {'holes_facing': 'y', 'x_pos': 5, 'y_pos': 6, 'in_positive_direction': True},
                            {'holes_facing': 'y', 'x_pos': 5, 'y_pos': 6, 'in_positive_direction': False}]

    holes_descriptions_z = [{'holes_facing': 'z', 'x_pos': 1, 'y_pos': 1, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 1, 'y_pos': 1, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 11, 'y_pos': 1, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 11, 'y_pos': 1, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 11, 'y_pos': 11, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 11, 'y_pos': 11, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 1, 'y_pos': 11, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 1, 'y_pos': 11, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 6, 'y_pos': 6, 'in_positive_direction': True},
                            {'holes_facing': 'z', 'x_pos': 6, 'y_pos': 6, 'in_positive_direction': True}]

    holes_descriptions_test = [{'holes_facing': 'x', 'x_pos': 1, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 2, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 3, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 4, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 5, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 6, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 7, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 8, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 9, 'y_pos': 3, 'in_positive_direction': True},
                               {'holes_facing': 'x', 'x_pos': 10, 'y_pos': 3, 'in_positive_direction': True}]

    get_holes_position_direction_values(holes_descriptions_x)
    get_holes_position_direction_values(holes_descriptions_y)
    get_holes_position_direction_values(holes_descriptions_z)
    get_holes_position_direction_values(holes_descriptions_test)
