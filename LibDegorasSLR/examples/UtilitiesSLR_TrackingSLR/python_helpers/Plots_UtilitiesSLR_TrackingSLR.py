import matplotlib.pyplot as plt
import numpy as np


# Function to read positions from file
def read_positions(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
        positions = [(float(line.split(',')[0]), float(line.split(',')[1])) for line in lines]
    return positions


if __name__ == "__main__":

    # Output folder.
    output_dir = "../outputs"

    # Read positions from files
    sun_positions = read_positions(output_dir + '/' + 'Lares_Sun_Beg_sun.dat')
    tracking_positions = read_positions(output_dir + '/' + 'Lares_Sun_Beg_tracking.dat')

    # Convert azimuth and elevation to radians
    sun_azimuths = [np.radians(pos[0]) for pos in sun_positions]
    sun_elevations = [pos[1] for pos in sun_positions]
    tracking_azimuths = [np.radians(pos[0]) for pos in tracking_positions]
    tracking_elevations = [pos[1] for pos in tracking_positions]

    # Plot polar graph
    plt.figure(figsize=(12, 12))
    ax = plt.subplot(111, polar=True)

    # Plot sun positions
    #ax.plot(sun_azimuths, sun_elevations, color='orange', label='Sun', linewidth=3, zorder=0)
    ax.scatter(sun_azimuths, sun_elevations, color='orange', s=10, zorder=1)

    # Plot tracking positions
    ax.plot(tracking_azimuths, tracking_elevations, color='blue', label='Tracking', zorder=0)

    # Plot green point at the beginning of the track
    ax.scatter(tracking_azimuths[0], tracking_elevations[0], color='green', label='Start', s=50, zorder=1)

    # Plot red point at the end of the track
    ax.scatter(tracking_azimuths[-1], tracking_elevations[-1], color='red', label='End', s=50, zorder=1)

    # Configs
    ax.set_theta_zero_location('N')
    ax.set_theta_direction(-1)
    ax.set_rlabel_position(-90)
    ax.set_ylim(90, 0)
    ax.set_yticks(range(0, 90, 10))
    ax.set_yticklabels([str(i) for i in range(0, 90, 10)])

    # Add legend
    plt.legend()

    # Save plot with high resolution
    plt.savefig('polar_plot.png', dpi=800)

    # Show plot
    plt.show()