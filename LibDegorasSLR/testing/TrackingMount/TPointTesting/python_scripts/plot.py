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
    az_error = np.array(df['az_error']) * 3600
    el_error = np.array(df['el_error']) * 3600
    rms = np.array(df['rms']) * 3600

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
    fig1.savefig('az_corr_3d.png')


    fig2, ax2 = plt.subplots(subplot_kw={'projection': '3d'})

    ax2.set_title('Distribución de Errores del eje de Elevación')

    c = ax2.plot_surface(az, el, el_error, cmap=cm.coolwarm, linewidth=0, antialiased=False)
    # set the limits of the plot to the limits of the data
    fig2.colorbar(c, ax=ax2)
    ax2.grid()
    fig2.savefig('el_corr_3d.png')


    # Every 3rd point in each direction.
    skip = (slice(None, None, 50), slice(None, None, 50))
    dx, dy = np.gradient(az_error)
    fig4, ax4 = plt.subplots()
    cf = ax4.contourf(az, el, az_error)
    ax4.quiver(az[skip], el[skip], dx[skip], dy[skip], color='Black')
    ax4.set(title="Azimuth errors quiver plot")
    fig4.colorbar(cf, ax=ax4)
    fig4.savefig('az_corr_quiver.png')

    skip = (slice(None, None, 50), slice(None, None, 50))
    dx, dy = np.gradient(el_error)
    fig5, ax5 = plt.subplots()
    cf = ax5.contourf(az, el, el_error)
    ax5.quiver(az[skip], el[skip], dx[skip], dy[skip], color='Black')
    ax5.set(title="Elevation errors quiver plot")
    fig5.colorbar(cf, ax=ax5)
    fig5.savefig('el_corr_quiver.png')

    fig6, ax6 = plt.subplots(subplot_kw={'projection': 'polar'})
    # Set the range for elevation (0 to 90 degrees)
    ax6.set_ylim(0, 90)
    ax6.set_yticks([0, 30, 60, 90])  # Customize the elevation rings
    ax6.set_yticklabels(['90°', '60°', '30°', '0°'])
    c = ax6.pcolormesh(az * (np.pi / 180), 90 - el, az_error, shading="nearest")
    ax6.set(aspect=1, title="Azimuth errors polar heatmap")
    fig6.colorbar(c, ax=ax6)
    ax6.set_theta_zero_location('N')
    ax6.set_theta_direction(-1)
    fig6.savefig('az_corr_polar.png')

    fig7, ax7 = plt.subplots(subplot_kw={'projection': 'polar'})
    # Set the range for elevation (0 to 90 degrees)
    ax7.set_ylim(0, 90)
    ax7.set_yticks([0, 30, 60, 90])  # Customize the elevation rings
    ax7.set_yticklabels(['90°', '60°', '30°', '0°'])
    c = ax7.pcolormesh(az * (np.pi / 180), 90 - el, el_error, shading="nearest")
    ax7.set(aspect=1, title="Elevation errors polar heatmap")
    fig7.colorbar(c, ax=ax7)
    ax7.set_theta_zero_location('N')
    ax7.set_theta_direction(-1)
    fig7.savefig('el_corr_polar.png')


