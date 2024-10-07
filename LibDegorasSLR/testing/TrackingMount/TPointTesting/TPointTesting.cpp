#include <LibDegorasSLR/Helpers/filedir_helpers.h>
#include <LibDegorasSLR/TrackingMount/models/tpoint/tpoint_tools.h>

#include <omp.h>
#include <cmath>
#include <fstream>

int main()
{

    std::string path("inputs/model.dat");

    dpslr::mount::models::TPointCoefficients coefs;
    auto error = dpslr::mount::models::parseTPointModelFile(path, coefs);

    if (error != dpslr::mount::models::TPointParseError::NOT_ERROR || coefs.empty())
    {
        std::cerr << "Cannot load TPoint terms from file in path " << path << std::endl;
        return -1;
    }

    std::cout << "Loaded model is: " << std::endl;
    for (const auto& parallel_group : coefs)
    {
        std::cout << "(";
        for (auto coef_it = parallel_group.begin(); coef_it != parallel_group.end(); coef_it++)
        {
            std::cout << coef_it->name << " = " << coef_it->value;
            if (coef_it + 1 != parallel_group.end())
                std::cout << ", ";
        }
        std::cout << ") ";
    }

    std::cout << std::endl;

    unsigned factor = 1;
    unsigned min_elev = 10, max_elev = 85;
    unsigned col_size = (max_elev - min_elev) * factor;

    const std::vector<dpslr::astro::types::AltAzPos> zero_pos_vector(col_size, {0., 0.});
    const std::vector<double> zero_vector(col_size, 0.);
    std::vector<std::vector<dpslr::astro::types::AltAzPos>> modified_positions(36000, zero_pos_vector);
    std::vector<std::vector<double>> error_az(36000, zero_vector);
    std::vector<std::vector<double>> error_el(36000, zero_vector);
    std::vector<std::vector<double>> error_rms(36000, zero_vector);

    omp_set_num_threads(8);

    #pragma omp parallel for
    for (unsigned i = 0; i < 360 * factor; i++)
    {
        for (unsigned j = min_elev * factor; j < max_elev * factor; j++)
        {
            dpslr::astro::types::AltAzPos p(i / (1. * factor), j / (1. * factor));
            unsigned col_idx = j - min_elev * factor;
            modified_positions[i][col_idx] = dpslr::mount::models::computeCorrectedByTPointPosition(coefs, p);
            error_az[i][col_idx] = p.az - modified_positions[i][col_idx].az;
            error_el[i][col_idx] = p.el - modified_positions[i][col_idx].el;
            error_rms[i][col_idx] = std::sqrt((error_az[i][col_idx]*error_az[i][col_idx] +
                                               error_el[i][col_idx]*error_el[i][col_idx]) / 2.);
        }
    }

    std::ofstream out("errors.csv");

    out << std::setprecision(8);

    for (unsigned i = 0; i < 360 * factor; i++)
    {
        for (unsigned j = min_elev * factor; j < max_elev * factor; j++)
        {

            unsigned col_idx = j - min_elev * factor;
            out << i / static_cast<double>(factor) << "," << j / static_cast<double>(factor) << ","
                << error_az[i][col_idx] << "," << error_el[i][col_idx] << "," <<  error_rms[i][col_idx];
            out << std::endl;
        }
    }


    std::string current_dir = dpslr::helpers::files::getCurrentDir();

    // Configure the python script executable.
    std::string python_plot_script(current_dir+"/python_scripts/plot.py");
    std::string python_cmd = "python \"" + python_plot_script + "\" ";

    std::cout<<"Plotting data using Python helpers..."<<std::endl;
    if(system(std::string(python_cmd).c_str()))
        std::cout<<"Plotting failed!!"<<std::endl;

    return 0;
}
