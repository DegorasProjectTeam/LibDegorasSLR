# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from scipy.interpolate import griddata
from matplotlib.colors import Normalize

df = pd.read_csv('errors.csv', header=None)
df.columns = ['az', 'el', 'az_error', 'el_error', 'rms']

az = np.array(df['az'])
el = np.array(df['el'])
az_error = np.array(df['az_error'])
el_error = np.array(df['el_error'])
rms = np.array(df['rms'])

factor = 1
min_elev = 10
max_elev = 85
rows = 360 * factor
cols = (max_elev - min_elev) * factor

az = az.reshape(rows, cols)
el = el.reshape(rows, cols)
az_error = az_error.reshape(rows, cols)
el_error = el_error.reshape(rows, cols)
rms = rms.reshape(rows, cols)

fig1, ax1 = plt.subplots(subplot_kw={'projection': 'polar'})

ax1.set_theta_zero_location('N')
ax1.set_theta_direction(-1)
ax1.set_yticks(np.arange(0, 91, 20))
ax1.set_yticklabels(np.arange(0, 91, 20))
ax1.set_title('Distribución de Errores del eje de Azimuth')

vmin1 = -0.045
vmax1 = 0.05
norm1 = Normalize(vmin=vmin1, vmax=vmax1)

c = ax1.pcolormesh(az, el, az_error, norm=norm1)
# set the limits of the plot to the limits of the data
fig1.colorbar(c, ax=ax1)
ax1.grid()


fig2, ax2 = plt.subplots(subplot_kw={'projection': 'polar'})

ax2.set_theta_direction(-1)
ax2.set_yticks(np.arange(0, 91, 20))
ax2.set_theta_zero_location('N')
ax2.set_yticklabels(np.arange(0, 91, 20))
ax2.set_title('Distribución de Errores del eje de Elevación')

el_min, el_max = 0.001, 0.1

vmin2 = -0.045
vmax2 = 0.05
norm2 = Normalize(vmin=vmin2, vmax=vmax2)

c = ax2.pcolormesh(az, el, el_error, norm=norm2)
# set the limits of the plot to the limits of the data
fig2.colorbar(c, ax=ax2)
ax2.grid()


fig3, ax3 = plt.subplots(subplot_kw={'projection': 'polar'})

ax3.set_theta_direction(-1)
ax3.set_yticks(np.arange(0, 91, 20))
ax3.set_theta_zero_location('N')
ax3.set_yticklabels(np.arange(0, 91, 20))
ax3.set_title('Distribución de RMS')

vmin3 = 0.
vmax3 = 0.01
norm3 = Normalize(vmin=vmin3, vmax=vmax3)

c = ax3.pcolormesh(az, el, rms, norm=norm3)
# set the limits of the plot to the limits of the data
fig3.colorbar(c, ax=ax3)
ax3.grid()


plt.show()