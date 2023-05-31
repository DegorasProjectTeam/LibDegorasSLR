/***********************************************************************************************************************
 * Copyright 2023 Degoras Project Team
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they will be approved by the
 * European Commission - subsequent versions of the EUPL (the "Licence");
 *
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the Licence is distributed on
 * an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the Licence for the
 * specific language governing permissions and limitations under the Licence.
 **********************************************************************************************************************/

#include "LibDPSLR/geo.h"

#include <cmath>

namespace dpslr{
namespace geo{

namespace meteo {

double waterVaporPressure(double rh, double temp, double pres, WtrVapPressModel mode)
{
    // Water vapor pressure variable.
    double e0 = 0;

    // Calculate the water vapor pressute using the original Marini and Murray formula.
    if(mode == WtrVapPressModel::ORIGINAL_MM)
        e0 = rh * 6.11e-2 * std::pow(10.0, ((7.5 * (temp - 273.15)) / (237.3 + (temp - 273.15))));

    // Calculate the water vapor pressure using Giacomo and Davis formula.
    if(mode == WtrVapPressModel::GIACOMO_DAVIS)
    {
        // Calculate the saturation vapor pressure in mbar (Giacomo 1982 and Davis 1992).
        double es = 0.01 * std::exp(
                    1.2378847e-5*std::pow(temp,2) - 1.9121316e-2*temp + 33.93711047 - 6.3431645e3*std::pow(temp,-1));
        // Calculate the enhancement factor (Giacomo 1982)
        double fw = 1.00062 + 3.14e-6 * pres + 5.6e-7 * std::pow(temp - 273.15, 2);
        // Finally, calculate the water vapor pressure from a relative humidity measurement (%).
        e0 = rh * 0.01 * fw * es;
    }

    // Return the water vapor pressure.
    return e0;
}

} // END NAMESPACE METEO.

namespace tropo{

double pathDelayMariniMurray(double pres, double temp, double rh, double el, double wl, double phi,
                             double ht, meteo::WtrVapPressModel wvpm)
{
    // Calculate the water vapor pressure.
    double e0 = meteo::waterVaporPressure(rh, temp, pres, wvpm);
    // Calculate A, B and K.
    double a = 0.2357e-2 * pres + 0.141e-3 * e0;
    double k = 1.163 - 0.968e-2 * std::cos(2.0 * phi) - 0.104e-2 * temp + 0.1435e-4 * pres;
    double b = 1.084e-8 * pres * temp * k + 4.734e-8 * (2.0 * std::pow(pres,2))/(temp * (3.0 - 1.0/k));
    // Calculate the laser frequency parameter and the laser site function value.
    double flam = 0.9650 + 0.0164 * std::pow(wl, -2) + 0.228e-3 * std::pow(wl, -4);
    double fphih = 1.0 - 0.26e-2 * std::cos(2.0 * phi) - 0.31e-6 * ht;
    // Calculate the range correction.
    double sine = std::sin(el);
    double ab = a + b;
    double ar = (flam / fphih) * (ab / (sine + (b / ab) / (sine + 0.01)));
    // Return one way the tropospheric path delay (meters).
    return ar;
}

} // END NAMESPACE TROPO.
}} // END NAMESPACE GEO AND DPSLR.

// =====================================================================================================================
