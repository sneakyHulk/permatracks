import numpy as np
import matplotlib.pyplot as plt
import csv


def plot_optimization_error():
    with open('../../result/last_optimization_error.csv') as csvfile:
        csvreader = csv.DictReader(csvfile, delimiter=',')
        absolute_errors = np.array([
            np.array([float(row["mag_x0"]) - float(row["computed_mag_x0"]),
                      float(row["mag_y0"]) - float(row["computed_mag_y0"]),
                      float(row["mag_z0"]) - float(row["computed_mag_z0"]),
                      float(row["mag_theta0"]) - float(row["computed_mag_theta0"]),
                      float(row["mag_phi0"]) - float(row["computed_mag_phi0"])]) for row in csvreader])

    fig, ax = plt.subplots()
    ax.plot(absolute_errors[42:, 0:3])
    ax.legend(["x", "y", "z"])
    ax.set_title("Optimization error")
    ax.set_xlabel("Iteration")
    ax.set_ylabel("Position error [m]")

    plt.show()


def plot_optimization_time():
    with open('../../result/LevenbergMarquardtOptimizer.csv') as csvfile:
        csvreader = csv.DictReader(csvfile, delimiter=',')
        time_taken = np.array([float(row["t"]) for row in csvreader])

        fig, ax = plt.subplots()
        ax.plot(time_taken[42:])
        ax.legend(["time"])
        ax.set_title("Time taken for solving the optimization problem")
        ax.set_xlabel("Iteration")
        ax.set_ylabel("time taken [s]")

        plt.show()


if __name__ == '__main__':
    plot_optimization_error()
    plot_optimization_time()
