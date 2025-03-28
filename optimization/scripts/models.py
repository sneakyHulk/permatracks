import sympy as sp
import math
import numpy as np
import itertools
import matplotlib.pyplot as plt
import scipy.optimize
import scipy


def dipol_model(sensor_position_direction_values):
    Gx, Gy, Gz = sp.symbols('Gx, Gy, Gz', real=True)
    x1, y1, z1 = sp.symbols('x1, y1, z1', real=True)
    m1 = sp.symbols('m1', real=True)
    theta1 = sp.symbols('theta1', real=True)
    phi1 = sp.symbols('phi1', real=True)

    ri1x = [(x1 - sx) for sx in sensor_position_direction_values[:, 0]]
    ri1y = [(y1 - sy) for sy in sensor_position_direction_values[:, 1]]
    ri1z = [(z1 - sz) for sz in sensor_position_direction_values[:, 2]]

    Bi1x = [Gx + m1 * (3 * r1x * (
            sp.sin(theta1) * sp.cos(phi1) * r1x
            + sp.sin(theta1) * sp.sin(phi1) * r1y
            + sp.cos(theta1) * r1z)
                       / (sp.sqrt(r1x ** 2 + r1y ** 2 + r1z ** 2) ** 5)
                       - sp.sin(theta1) * sp.cos(phi1)
                       / (sp.sqrt(r1x ** 2 + r1y ** 2 + r1z ** 2) ** 3))
            for r1x, r1y, r1z in zip(ri1x, ri1y, ri1z)]

    Bi1y = [Gy + m1 * (3 * r1y * (
            sp.sin(theta1) * sp.cos(phi1) * r1x
            + sp.sin(theta1) * sp.sin(phi1) * r1y
            + sp.cos(theta1) * r1z)
                       / sp.sqrt(r1x ** 2 + r1y ** 2 + r1z ** 2) ** 5
                       - sp.sin(theta1) * sp.sin(phi1) /
                       sp.sqrt(r1x ** 2 + r1y ** 2 + r1z ** 2) ** 3)
            for r1x, r1y, r1z in zip(ri1x, ri1y, ri1z)]

    Bi1z = [Gz + m1 * (3 * r1z * (
            sp.sin(theta1) * sp.cos(phi1) * r1x
            + sp.sin(theta1) * sp.sin(phi1) * r1y
            + sp.cos(theta1) * r1z)
                       / sp.sqrt(r1x ** 2 + r1y ** 2 + r1z ** 2) ** 5
                       - sp.cos(theta1) /
                       sp.sqrt(r1x ** 2 + r1y ** 2 + r1z ** 2) ** 3)
            for r1x, r1y, r1z in zip(ri1x, ri1y, ri1z)]

    model = []
    for Bx, By, Bz in zip(Bi1x, Bi1y, Bi1z):
        model = model + [Bx, By, Bz]

    # model = Bi1x + Bi1y + Bi1z
    symbols = {'x1': x1, 'y1': y1, 'z1': z1, 'm1': m1, 'theta1': theta1, 'phi1': phi1, 'Gx': Gx, 'Gy': Gy, 'Gz': Gz}

    return model, symbols


def test_model():
    x, y, z = sp.symbols('x, y, z', real=True)

    model = [x ** 2 - 1, y ** 2 - 1, z ** 2 - 1]

    symbols = {'x': x, 'y': y, 'z': z}
    return model, symbols
