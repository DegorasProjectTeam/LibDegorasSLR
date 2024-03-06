/***********************************************************************************************************************
 *   LibDegorasSLR (Degoras Project SLR Library).                                                                      *
 *                                                                                                                     *
 *   A modern and efficient C++ base library for Satellite Laser Ranging (SLR) software and real-time hardware         *
 *   related developments. Developed as a free software under the context of Degoras Project for the Spanish Navy      *
 *   Observatory SLR station (SFEL) in San Fernando and, of course, for any other station that wants to use it!        *
 *                                                                                                                     *
 *   Copyright (C) 2024 Degoras Project Team                                                                           *
 *                      < Ángel Vera Herrera, avera@roa.es - angeldelaveracruz@gmail.com >                             *
 *                      < Jesús Relinque Madroñal >                                                                    *
 *                                                                                                                     *
 *   This file is part of LibDegorasSLR.                                                                               *
 *                                                                                                                     *
 *   Licensed under the European Union Public License (EUPL), Version 1.2 or subsequent versions of the EUPL license   *
 *   as soon they will be approved by the European Commission (IDABC).                                                 *
 *                                                                                                                     *
 *   This project is free software: you can redistribute it and/or modify it under the terms of the EUPL license as    *
 *   published by the IDABC, either Version 1.2 or, at your option, any later version.                                 *
 *                                                                                                                     *
 *   This project is distributed in the hope that it will be useful. Unless required by applicable law or agreed to in *
 *   writing, it is distributed on an "AS IS" basis, WITHOUT ANY WARRANTY OR CONDITIONS OF ANY KIND; without even the  *
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the EUPL license to check specific   *
 *   language governing permissions and limitations and more details.                                                  *
 *                                                                                                                     *
 *   You should use this project in compliance with the EUPL license. You should have received a copy of the license   *
 *   along with this project. If not, see the license at < https://eupl.eu/ >.                                         *
 **********************************************************************************************************************/

/** ********************************************************************************************************************
 * @file tracking_analyzer.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the TrackingAnalyzer class.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/Timing/types/base_time_types.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
#include "LibDegorasSLR/TrackingMount/types/tracking_types.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using timing::types::MJDateTime;
using timing::types::MJDate;
using timing::types::SoD;
using astro::PredictorSunBase;
using astro::types::AltAzPos;
using astro::types::AltAzPosV;
using math::units::DegreesU;
using math::units::Degrees;
using math::units::MillisecondsU;
using math::units::Meters;
using utils::PredictorSlrBase;
using utils::SLRPrediction;
using utils::SLRPredictionV;
using astro::SunPrediction;
using astro::SunPredictionV;
// ---------------------------------------------------------------------------------------------------------------------

// TODO LIST:
// If a not tracking movement (absolute, relative, continuous) starts at sun, the mount must go to error state.
// If it ends at sun, the movment is not valid.

// No se procesa la elevacion maxima porque es trivial, a diferencia de un cambio de trayectoria completo como
// puede ser el resultado de una posible interferencia del Sol. La elevación minima simplemente se utiliza para
// comprobar la existencia o no de un pase en el intervalo seleccionado. Por defecto, para satélites que no sean
// altos, 10 grados es suficiente. Esta elevación debe de coincidir con la que se use para generar las predicciones
// para evitar incongruencias.

// Este sistema no analiza los limites físicos de la montura de seguimiento. En SFEl, la montura AMELAS tiene
// capacidad independiente de cálculo, y es la encargada de realizar internamente una modificación de la trayectoria
// si el pase es demasiado alto o demasiado rápido. En otros sistemas, este cálculo específico debe de ser realizado
// independientemente con otro subsistema dedicado una vez realizada la predicción con esta clase.

// TRACKING SLR VS PassCalculatorSLR
// TrackingSLR está pensado para un unico track. La idea es que pass calculator tiene capacidad de calcular todos
// los pases (o los que se requieran) a partir de un CPF y sacar las características del mismo. En este caso, los
// datos serían los reales respecto al pase. TrackingSLR lo que permite es modificar estos datos y generar una
// nueva clase TrackSLR (diferente de PassSLR pero que contiene tambien el original) en la que el pase pueda verse
// modificado por distintas cuestiones, siempre desde el punto de vista de la seguridad o mécanico de la montura.
// Inicialmente, solo tendremos en cuenta el Sol y limites de altura, pero en el futuro puede añadirse nuevos
// parámetros como la máxima velocidad del pase.

// inicio > final
// El intervalo se encuentra dentro del CPF (predictor). Validar predictor respecto a intervalos.
// Comprobar que existe un pase (la elevacion es en todo momento > minimo).
//
// TODO Se podrian añadir en un futuro limites mecánicos para que el sistema ajuste automáticamente velocidades,
// alturas, etc.

/**
 * @brief The TrackingSLR class implements an abstraction for a SLR tracking.
 *
 * This class uses a PredictorSLR to look for a SLR tracking within the given time window, i.e., the object is
 * always above the minimum elevation.
 *
 * This class also offers a sun avoidance algorithm. This algorithm changes the tracking trajectory whenever it passes
 * through a circular sector around the sun (the sun security sector). There are other checkings performed to the
 * tracking by this class. Before using the tracking you should check whether the tracking is valid or not.
 *
 */
class LIBDPSLR_EXPORT TrackingAnalyzer
{
public:

    TrackingAnalyzer(const TrackingConfig &config);

    /**
     * @brief This function checks if there is a valid tracking. You should check this, before requesting positions.
     * @return true if there is a valid tracking, false otherwise.
     */
    bool isValid() const;

    const TrackingPredictionV &getPredictions() const;

    const TrackingInfo& getTrackingInfo() const;

    TrackingPredictionV::const_iterator trackingBegin() const;

    TrackingPredictionV::const_iterator trackingEnd() const;

    void analyzePrediction(TrackingPrediction& pred) const;

    void analyzeTracking(const TrackingPredictionV &predictions);



private:

    // Private functions.


    /// Helper to check the tracking start.
    bool analyzeTrackingStart();

    /// Helper to check the tracking end.
    bool analyzeTrackingEnd();

    /// Helper to check the tracking.
    bool analyzeTrackingMiddle();

    /// Helper to check if the predicted position is inside a sun sector.
    bool insideSunSector(const AltAzPos& pass_pos, const AltAzPos& sun_pos) const;

    /// Helper to set the rotation direction of a sun sector.
    bool setSunSectorRotationDirection(SunCollisionSector &sector,
                                       TrackingPredictionV::const_iterator sun_start,
                                       TrackingPredictionV::const_iterator sun_end);

    /// Helper to check positions whithin a sun sector to see if it is possible to avoid sun
    void checkSunSectorPositions(const SunCollisionSector &sector,
                                 TrackingPredictionV::iterator sun_start,
                                 TrackingPredictionV::iterator sun_end);

    long double calcSunAvoidTrajectory(const MJDateTime& mjdt,
                                       const SunCollisionSector &sector,
                                       const AltAzPos &sun_pos) const;

    void calcSunAvoidPos(TrackingPrediction& pred,
                         const SunCollisionSector &sector) const;

    // Private members.
    TrackingConfig config_;                         ///< Contains the tracking user configuration.
    TrackingInfo track_info_;                       ///< Contains the analyzed tracking information.
    TrackingPredictionV predictions_;
    TrackingPredictionV::iterator begin_;
    TrackingPredictionV::iterator end_;
    SunCollisionSectorV sun_sectors_;
};

}} // END NAMESPACES
// =====================================================================================================================
