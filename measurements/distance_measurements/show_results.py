import csv
import os
import matplotlib.pyplot as plt
import math
import re
import numpy as np


def show_results(sensor):
    magnetic_flux_density = []
    distances = []

    distance_pattern = r"mag_data_([0-9a-zA-Z]+)_[0-9a-zA-Z]+_[0-9a-zA-Z]+_([0-9]+)mm.txt"
    for filename in os.listdir(os.path.join(os.getcwd(), "results", sensor)):
        match = re.search(distance_pattern, filename)

        assert match is not None
        assert str(match.group(1)) == str(sensor)

        with open(os.path.join(os.getcwd(), "results", sensor, filename)) as csvfile:
            reader = csv.reader(csvfile, delimiter=',')
            data = [math.sqrt(float(x[1]) ** 2 + float(x[2]) ** 2) for x in reader]

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
    show_results("MMC5983MA")
