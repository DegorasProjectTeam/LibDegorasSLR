#include <LibDegorasSLR/TrackingMount/models/tpoint/tpoint_tools.h>

#include <LibDegorasBase/Helpers/filedir_helpers.h>
#include <LibDegorasBase/Helpers/string_helpers.h>

#include <omp.h>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <thread>

struct Observation
{
    long double az_calc;
    long double el_calc;
    long double az_obs;
    long double el_obs;
    long double az_corrected;
    long double el_corrected;

};

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

        if (!std::filesystem::exists("errors_stars.csv"))
        {
            std::string path("inputs/observation.dat");

            std::ifstream observation_file(path);
            if (observation_file.is_open())
            {
                std::string line;
                std::vector<std::string> tokens;
                Observation obs;
                std::vector<Observation> observations;

                // Read all terms
                while(std::getline(observation_file, line))
                {
                    dpbase::helpers::strings::split(tokens, line, " ", false);

                    try
                    {
                        obs.az_calc = std::stold(tokens.at(0));
                        obs.el_calc = std::stold(tokens.at(1));
                        obs.az_obs = std::stold(tokens.at(2));
                        obs.el_obs = std::stold(tokens.at(3));

                        observations.push_back(obs);
                    }
                    catch(...) {}
                }

                for (auto&& o : observations)
                {
                    dpslr::astro::types::AltAzPos pos(o.az_calc, o.el_calc);
                    auto corrected_pos = dpslr::mount::models::computeCorrectedByTPointPosition(coefs, pos);

                    o.az_corrected = corrected_pos.az;
                    o.el_corrected = corrected_pos.el;
                }

                std::ofstream out("errors_stars.csv");

                out << "az_calc,el_calc,az_obs,el_obs,az_corrected,el_corrected" << std::endl;

                out << std::setprecision(8);

                for (const auto &o : observations)
                {
                    out << o.az_calc << "," << o.el_calc << "," << o.az_obs << "," << o.el_obs << ","
                        << o.az_corrected << "," << o.el_corrected << std::endl;
                }
            }

            else
            {
                std::cout << "Cannot open observations file. No star position correction calculated." << std::endl;
            }

        }
        else
        {
            std::cout << "The file errors_stars.csv already exists. Only plotting. "
                      << "If you want to recalculate, delete errors_stars.csv"
                      << std::endl;
        }

    }
    else
    {
        std::cout << "The file errors.csv already exists. Only plotting. If you want to recalculate, delete errors.csv"
                  << std::endl;
    }



    std::string current_dir = dpbase::helpers::files::getCurrentDir();

    // Configure the python script executable.
    std::string python_plot_errors_script(current_dir+"/python_scripts/plot.py");
    std::string python_cmd = "python \"" + python_plot_errors_script + "\" " +
                             std::to_string(factor) + " " + std::to_string(min_elev) + " " + std::to_string(max_elev);

    std::string python_plot_error_stars_script(current_dir+"/python_scripts/plot_observations.py");
    std::string python_cmd_stars = "python \"" + python_plot_error_stars_script;

    std::cout<<"Plotting data using Python helpers..."<<std::endl;

    auto t1 = std::thread([&python_cmd]{std::system(std::string(python_cmd).c_str());});
    auto t2 = std::thread([&python_cmd_stars]{std::system(std::string(python_cmd_stars).c_str());});

    t1.join();
    t2.join();


    return 0;
}
