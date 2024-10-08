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

    # Get external file by arguments. If no external file, use the manually selected.
    try:
        factor = int(sys.argv[1])
    except (ValueError, IndexError):
        factor = 1

    try:
        min_elev = int(sys.argv[2])
    except (ValueError, IndexError):
        min_elev = 0

    try:
        max_elev = int(sys.argv[3])
    except (ValueError, IndexError):
        max_elev = 90

    df = pd.read_csv('errors.csv', header=0)
    df.columns = ['az', 'el', 'az_error', 'el_error', 'rms']

    az = np.array(df['az'])
    el = np.array(df['el'])
    az_error = np.array(df['az_error'])
    el_error = np.array(df['el_error'])
    rms = np.array(df['rms'])

    rows = 360 * factor
    cols = (max_elev - min_elev) * factor

    az = az.reshape(rows, cols)
    el = el.reshape(rows, cols)
    az_error = az_error.reshape(rows, cols)
    el_error = el_error.reshape(rows, cols)
    rms = rms.reshape(rows, cols)

    fig1, ax1 = plt.subplots(subplot_kw={'projection': '3d'})

    ax1.set_title('Distribución de Errores del eje de Azimuth')

    c = ax1.plot_surface(az, el, az_error, cmap=cm.coolwarm, linewidth=0, antialiased=False)
    # set the limits of the plot to the limits of the data
    fig1.colorbar(c, ax=ax1)
    ax1.grid()


    fig2, ax2 = plt.subplots(subplot_kw={'projection': '3d'})

    ax2.set_title('Distribución de Errores del eje de Elevación')

    c = ax2.plot_surface(az, el, el_error, cmap=cm.coolwarm, linewidth=0, antialiased=False)
    # set the limits of the plot to the limits of the data
    fig2.colorbar(c, ax=ax2)
    ax2.grid()


    fig3, ax3 = plt.subplots(subplot_kw={'projection': '3d'})

    ax3.set_title('Distribución de RMS')

    c = ax3.plot_surface(az, el, rms, cmap=cm.coolwarm, linewidth=0, antialiased=False)
    # set the limits of the plot to the limits of the data
    fig3.colorbar(c, ax=ax3)
    ax3.grid()


    plt.show()
