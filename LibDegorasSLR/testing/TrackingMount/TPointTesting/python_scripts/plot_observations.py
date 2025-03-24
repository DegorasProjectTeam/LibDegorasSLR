# -*- coding: utf-8 -*-

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from scipy.interpolate import griddata
from matplotlib.colors import Normalize
from matplotlib import cm
import sys


if __name__ == "__main__":

    df = pd.read_csv('errors_stars.csv', header=0)
    df.columns = ['az_calc', 'el_calc', 'az_obs', 'el_obs', 'az_corrected', 'el_corrected']

    az_calc = np.array(df['az_calc'])
    el_calc = np.array(df['el_calc'])
    az_obs = np.array(df['az_obs'])
    el_obs = np.array(df['el_obs'])
    az_corrected = np.array(df['az_corrected'])
    el_corrected = np.array(df['el_corrected'])

    az_error = (az_obs - az_corrected) * 3600
    el_error = (el_obs - el_corrected) * 3600


    fig1, ax1 = plt.subplots()
    ax1.plot(az_error, 'bo')
    ax1.set(title="Azimuth correction error")

    fig2, ax2 = plt.subplots()
    ax2.plot(el_error,'-go')
    ax2.set(title="Elevation correction error")

    stdev_az = np.std(az_error)
    stdev_el = np.std(el_error)

    print("az std: ", stdev_az, "el std: ", stdev_el)

    excluded = []
    for i, error in enumerate(az_error):
        if abs(error) > 2 * stdev_az:
            excluded.append(i)

    for i, error in enumerate(el_error):
        if abs(error) > 2 * stdev_el:
            excluded.append(i)

    excluded = list(set(excluded))
    excluded.sort()

    az_error = np.delete(az_error, excluded)
    el_error = np.delete(el_error, excluded)

    fig3, ax3 = plt.subplots()
    ax3.plot(az_error, 'bo')
    ax3.set(title="Azimuth correction error (no outliers)")

    fig4, ax4 = plt.subplots()
    ax4.plot(el_error,'-go')
    ax4.set(title="Elevation correction error (no outliers)")

    plt.show()
