import csv
import os
import matplotlib.pyplot as plt
import math
import re
import numpy as np


def show_results(folder, B, A_1):
    magnetic_flux_density = []
    distances = []

    distance_pattern = r"mag_data_([0-9a-zA-Z]+)_[0-9a-zA-Z]+_[0-9a-zA-Z]+_([0-9]+)mm.txt"
    for filename in os.listdir(os.path.join(os.getcwd(), "results", folder)):
        match = re.search(distance_pattern, filename)

        assert match is not None

        with open(os.path.join(os.getcwd(), "results", folder, filename)) as csvfile:
            reader = csv.reader(csvfile, delimiter=',')

            data = []
            for x in reader:
                h = np.array([float(i) for i in x[1:]])
                h = np.dot(A_1, np.add(h, -B))

                data.append(math.sqrt(h[0] ** 2 + h[1] ** 2 + h[2] ** 2))

            distances.append(float(match.group(2)))
            magnetic_flux_density.append(data)

    indices = np.argsort(distances)

    fig = plt.figure()
    ax = fig.add_subplot(111)

    ax.boxplot([magnetic_flux_density[i] for i in indices])
    ticks = [str(x) + "mm" for x in [distances[i] for i in indices]]
    ax.set_xticklabels(ticks)
    # Customize the plot
    ax.set_xlabel("Distance to the Sensor")
    ax.set_ylabel("Magnetic flux density in µTesla")

    fig2 = plt.figure()
    ax2 = fig2.add_subplot(111)

    ax2.boxplot([magnetic_flux_density[i] for i in indices][4:])
    ticks = [str(x) + "mm" for x in [distances[i] for i in indices]][4:]
    ax2.set_xticklabels(ticks)

    # Show the plot
    plt.show()


if __name__ == '__main__':
    # show_results("MMC5983MA")

    B_lis3mdl = np.array([-10.1861, -9.79706, -10.2995])
    A_1_lis3mdl = np.array([[0.85813, 0.0366316, -0.00226046],
                            [0.0366316, 0.824624, -0.0210964],
                            [-0.00226046, -0.0210964, 0.873556]])
    show_results("LIS3MDL", B_lis3mdl, A_1_lis3mdl)
