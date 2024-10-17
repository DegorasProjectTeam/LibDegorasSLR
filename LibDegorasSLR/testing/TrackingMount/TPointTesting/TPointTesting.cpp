#include <LibDegorasSLR/TrackingMount/models/tpoint/tpoint_tools.h>

#include <LibDegorasBase/Helpers/filedir_helpers.h>

#include <omp.h>
#include <cmath>
#include <fstream>
#include <filesystem>

int main(int argc, char **argv)
{


    unsigned factor = 10;
    unsigned min_elev = 10, max_elev = 85;

    if (argc == 4)
    {
        try
        {
            min_elev = std::stoul(argv[1]);
            max_elev = std::stoul(argv[2]);
            factor = std::stoul(argv[3]);
        } catch (...) { std::cout << "Bad parameters, using default." << std::endl; }
    }


    if (!std::filesystem::exists("errors.csv"))
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
        unsigned col_size = (max_elev - min_elev) * factor;

        const std::vector<dpslr::astro::types::AltAzPos> zero_pos_vector(col_size, {0.L, 0.L});
        const std::vector<double> zero_vector(col_size, 0.);
        std::vector<std::vector<dpslr::astro::types::AltAzPos>> modified_positions(36000, zero_pos_vector);
        std::vector<std::vector<double>> error_az(36000, zero_vector);
        std::vector<std::vector<double>> error_el(36000, zero_vector);
        std::vector<std::vector<double>> error_rms(36000, zero_vector);

    omp_set_num_threads(32);

        #pragma omp parallel for
        for (unsigned i = 0; i < 360 * factor; i++)
        {
            for (unsigned j = min_elev * factor; j < max_elev * factor; j++)
            {
                dpslr::astro::types::AltAzPos p(i / (1.L * factor), j / (1.L * factor));
                unsigned col_idx = j - min_elev * factor;
                modified_positions[i][col_idx] = dpslr::mount::models::computeCorrectedByTPointPosition(coefs, p);
                error_az[i][col_idx] = p.az - modified_positions[i][col_idx].az;
                error_el[i][col_idx] = p.el - modified_positions[i][col_idx].el;
                error_rms[i][col_idx] = std::sqrt((error_az[i][col_idx]*error_az[i][col_idx] +
                                                   error_el[i][col_idx]*error_el[i][col_idx]) / 2.);
            }
        }


        std::ofstream out("errors.csv");

        out << "az,el,az_error,el_error,rms" << std::endl;

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

    }
    else
    {
        std::cout << "The file errors.csv already exists. Only plotting. If you want to recalculate, delete errors.csv"
                  << std::endl;
    }

    std::string current_dir = dpbase::helpers::files::getCurrentDir();

    // Configure the python script executable.
    std::string python_plot_script(current_dir+"/python_scripts/plot.py");
    std::string python_cmd = "python \"" + python_plot_script + "\" " +
                             std::to_string(factor) + " " + std::to_string(min_elev) + " " + std::to_string(max_elev);

    std::cout<<"Plotting data using Python helpers..."<<std::endl;
    if(system(std::string(python_cmd).c_str()))
        std::cout<<"Plotting failed!!"<<std::endl;

    return 0;
}
