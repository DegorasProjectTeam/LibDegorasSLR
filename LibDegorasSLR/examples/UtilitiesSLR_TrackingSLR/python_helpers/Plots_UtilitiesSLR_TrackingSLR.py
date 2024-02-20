import matplotlib.pyplot as plt
import numpy as np


# Function to read positions from file
def read_positions(filename):
    header_size = 19
    pass_positions = []
    track_positions = []
    sun_positions = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines[header_size:]:
            data = line.split(';')
            
            pass_az = float(data[2])
            pass_el = float(data[3])
            pass_positions.append((pass_az, pass_el))

            if(data[4] != '' and data[5] != ''):
                track_az = float(data[4])
                track_el = float(data[5])
                track_positions.append((track_az, track_el))

            sun_az = float(data[6])
            sun_el = float(data[7])
            sun_positions.append((sun_az, sun_el))
                        
    return [pass_positions, track_positions, sun_positions]


if __name__ == "__main__":

    # Output folder.
    output_dir = "../outputs"

    # Example files.
    files = ['Lares_SunBeg_track.csv', 'Jason3_SunMid_track.csv']

    # Selector.
    selector = 0
    plot_all = True

    # Read positions from files
    pass_positions, track_positions, sun_positions = read_positions(output_dir + '/' + files[selector])

    # Extract data
    pass_azimuths = [np.radians(pos[0]) for pos in pass_positions]
    pass_elevations = [pos[1] for pos in pass_positions]
    track_azimuths = [np.radians(pos[0]) for pos in track_positions]
    track_elevations = [pos[1] for pos in track_positions]
    sun_azimuths = [np.radians(pos[0]) for pos in sun_positions]
    sun_elevations = [pos[1] for pos in sun_positions]
    max_el_idx = np.argmax(track_elevations)

    # Plot polar graph
    plt.figure(figsize=(12, 12))
    ax = plt.subplot(111, polar=True)

    # Plot sun positions
    ax.scatter(sun_azimuths, sun_elevations, color='orange', label='Sun', s=10, zorder=3)

    # Plot tracking positions
    ax.plot(track_azimuths, track_elevations, color='blue', linewidth=2, alpha=1, label='Tracking', zorder=3)

    # Plot pass positions
    ax.plot(pass_azimuths, pass_elevations, color='black', linewidth=1.5, alpha=1, label='Pass', zorder=2)

    # Plot green point at the beginning of the track
    ax.scatter(track_azimuths[0], track_elevations[0], color='green', label='Start', s=50, zorder=5)

    # Plot red point at the end of the track
    ax.scatter(track_azimuths[-1], track_elevations[-1], color='red', label='End', s=50, zorder=5)

    # Plot blue point at the culmination of the track
    ax.scatter(track_azimuths[max_el_idx], track_elevations[max_el_idx], color='blue', label='Culmination', s=50, zorder=5)
    
    # If enable, print the ascending and descending parts of the tracks.
    if(plot_all):
        ax.plot(track_azimuths[:max_el_idx+1], track_elevations[:max_el_idx+1], color='green', linewidth=2, label='Track ascending', zorder=4)
        ax.plot(track_azimuths[max_el_idx:], track_elevations[max_el_idx:], color='red', linewidth=2, alpha=1, label='Track descending', zorder=4)

    # Configurations
    ax.set_theta_zero_location('N')
    ax.set_theta_direction(-1)
    ax.set_rlabel_position(-90)
    ax.set_ylim(90, 0)
    ax.set_yticks(range(0, 90, 10))
    ax.set_yticklabels([str(i) for i in range(0, 90, 10)])

    # Add legend
    plt.legend()

    # Save plot with high resolution
    #plt.savefig('polar_plot.png', dpi=800)

    # Show plot
    plt.show()