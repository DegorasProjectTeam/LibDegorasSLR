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

    const std::vector<dpslr::astro::types::AltAzPos> zero_pos_vector(9000, {0., 0.});
    const std::vector<double> zero_vector(9000, 0.);
    std::vector<std::vector<dpslr::astro::types::AltAzPos>> modified_positions(36000, zero_pos_vector);
    std::vector<std::vector<double>> error_az(36000, zero_vector);
    std::vector<std::vector<double>> error_el(36000, zero_vector);
    std::vector<std::vector<double>> error_rms(36000, zero_vector);

    omp_set_num_threads(8);

    #pragma omp parallel for
    for (unsigned i = 0; i < 36000; i++)
    {
        for (unsigned j = 0; j < 9000; j++)
        {
            dpslr::astro::types::AltAzPos p(i / 100., j / 100.);
            modified_positions[i][j] = dpslr::mount::models::computeCorrectedByTPointPosition(coefs, p);
            error_az[i][j] = p.az - modified_positions[i][j].az;
            error_el[i][j] = p.el - modified_positions[i][j].el;
            error_rms[i][j] = std::sqrt((error_az[i][j]*error_az[i][j] + error_el[i][j]*error_el[i][j]) / 2.);
        }
    }

    std::ofstream out_az ("error_az.csv");
    std::ofstream out_el ("error_el.csv");
    std::ofstream out_rms ("error_rms.csv");

    out_az << std::setprecision(8);
    out_el << std::setprecision(8);
    out_rms << std::setprecision(8);

    for (unsigned i = 0; i < 36000; i++)
    {
        for (unsigned j = 0; j < 9000; j++)
        {
            out_az << error_az[i][j];
            out_el << error_el[i][j];
            out_rms << error_rms[i][j];
            if (j < 9000 - 1)
            {
                out_az << ",";
                out_el << ",";
                out_rms << ",";
            }
        }
        out_az << std::endl;
        out_el << std::endl;
        out_rms << std::endl;
    }
}
