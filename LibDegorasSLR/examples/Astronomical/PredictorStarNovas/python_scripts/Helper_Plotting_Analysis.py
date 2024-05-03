import matplotlib.pyplot as plt
import numpy as np
import sys
import math
import os
from matplotlib.animation import FuncAnimation
from matplotlib.patches import Circle, Wedge


def plot_ascending_descending_segments(ax, track_azimuths, track_elevations):
    """
    Plot ascending and descending segments of the track based on elevation changes.
    """
    elevation_diff = [track_elevations[i] - track_elevations[i-1] for i in range(1, len(track_elevations))]

    ascending_segments = []
    descending_segments = []

    segment_start = 0
    for i, diff in enumerate(elevation_diff):
        if diff > 0:
            if i == 0 or elevation_diff[i-1] <= 0:
                segment_start = i
        elif diff < 0:
            if i == 0 or elevation_diff[i-1] >= 0:
                if i - segment_start > 1:  # To avoid single point segments
                    ascending_segments.append((segment_start, i))
                segment_start = i

    if elevation_diff[-1] > 0:
        if len(elevation_diff) - segment_start > 1:  # To avoid single point segments
            ascending_segments.append((segment_start, len(elevation_diff)))
    elif elevation_diff[-1] < 0:
        if len(elevation_diff) - segment_start > 1:  # To avoid single point segments
            descending_segments.append((segment_start, len(elevation_diff)))

    for start, end in ascending_segments:
        ax.plot(track_azimuths[start:end], track_elevations[start:end], color='green', linewidth=2, label='Track ascending')

    for start, end in descending_segments:
        ax.plot(track_azimuths[start:end], track_elevations[start:end], color='red', linewidth=2, label='Track descending')


def plot_full_coverage_circle(ax, radius_deg, points, color):

    edge_angle = np.linspace(0, 2*np.pi, points)
    edge_radius = np.full(edge_angle.shape, 90 - radius_deg)
    ax.plot(edge_angle, edge_radius, color, linestyle='--')


def gen_projected_circle(ax, x_deg, y_deg, r_deg, plot_circle = True, \
                         points = 1000, color='yellow', zorder = 2, alph = 0.5) :

    print(y_deg)

    # Check if the security sector exceeds the zenith
    if math.ceil(y_deg) + r_deg >= 90:
        plot_full_coverage_circle(ax, r_deg, points, color)
        return

    # Project the Sun avoid zone.
    angle = np.linspace(0, 2*np.pi, points)
    xvec = np.cos(angle) * r_deg + x_deg
    yvec = np.sin(angle) * r_deg + y_deg

    # Adjust for points beyond the zenith
    for i in range(len(yvec)):
        if yvec[i] > 90:
            # Reflect the elevation
            yvec[i] = 180 - yvec[i]
            # Reflect the azimuth, ensuring it remains in the range [0, 360]
            xvec[i] = (xvec[i] + 180) % 360

    # Plot on the provided axis.
    if(plot_circle):
        ax.plot(np.radians(xvec), yvec, color, zorder=zorder)

    return xvec, yvec


def plot_track_culmination(ax, azs, els, color='blue', s=40, zorder=4, label='Track Culmination'):

    # Find the maximum value
    max_el_idx = np.argmax(els)
    max_el = els[max_el_idx]
    max_idxs = [index for index, value in enumerate(els) if value == max_el]

    # Check if the maximum elevation occurs at more than one point
    if len(max_idxs) > 1:
        # Plot a line connecting all points with the maximum elevation
        for idx in max_idxs:
            # Ensure continuity in the plot by connecting adjacent points
            if idx < len(azs) - 1 and (idx + 1) in max_idxs:
                ax.plot(azs[idx:idx+2], els[idx:idx+2], color=color, label=label, zorder=zorder)
                label = "_"  # Avoid duplicate labels in legend
    else:
        # Plot a single point if there's only one maximum
        ax.scatter(azs[max_idxs[0]], els[max_idxs[0]], color=color, s=s, zorder=zorder, label=label)


# Function to read positions from file
def read_positions(filename):
    header_size = 23
    track_positions = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines[header_size:]:
            data = line.split(';')

            track_az = float(data[1])
            track_el = float(data[2])
            track_positions.append((track_az, track_el))

                        
    return track_positions
    


if __name__ == "__main__":

    # Configuration.
    num_points = 100
    selector = 1
    plot_all = True

    # Manual example files.
    files = ['Vega_track_realtime.csv']

    # Get external file by arguments. If no external file, use the manually selected.
    filename = sys.argv[1] if len(sys.argv) > 1 else output_dir + '/' + files[selector]
    output_dir = "outputs" if len(sys.argv) > 1 else "../outputs"

    plot_file_name = os.path.splitext(os.path.basename(filename))[0]

    # Read positions from files
    track_positions = read_positions(filename)

    # Extract data
    # ---------------------------------------------------------
    track_azimuths = [np.radians(pos[0]) for pos in track_positions]
    track_elevations = [pos[1] for pos in track_positions]
    max_track_el_idx = np.argmax(track_elevations)
    # ---------------------------------------------------------

    # Polar plot configuration
    # ---------------------------------------------------------
    plt.figure(figsize=(12, 12))    
    ax = plt.subplot(111, polar=True)
    ax.set_theta_zero_location('N')
    ax.set_theta_direction(-1)
    ax.set_rlabel_position(-90)
    ax.set_ylim(90, 0)
    ax.set_yticks(range(0, 90, 10))
    ax.set_yticklabels([str(i) for i in range(0, 90, 10)])
    # ---------------------------------------------------------

    # Plot track data.
    ax.plot(track_azimuths, track_elevations, color='brown', linewidth=1.5, alpha=1, label='Star Pass', zorder=3)
    ax.scatter(track_azimuths[0], track_elevations[0], color='green', label='Track Start', s=50, zorder=4)
    ax.scatter(track_azimuths[-1], track_elevations[-1], color='red', label='Track End', s=50, zorder=4)
    #plot_track_culmination(ax, track_azimuths, track_elevations, color='blue', s=50, zorder=5)

    #for azimuth, elevation in zip(sun_azimuths, sun_elevations):
    #    plot_projected_circle(ax, np.degrees(azimuth), elevation, 15, num_points, 'yellow', 2)





    # Plot the culmination of the track
    
    # Plot blue point at the culmination of the track


    # If enable, print the ascending and descending parts of the tracks and Sun sectors.
    #plot_ascending_descending_segments(ax, track_azimuths, track_elevations)



    # Add legend
    plt.legend()

    # Save plot with high resolution
    plt.savefig(output_dir + '/' + plot_file_name + '_plot_polar.png', dpi=500)

    # Show plot
    plt.show()
   
