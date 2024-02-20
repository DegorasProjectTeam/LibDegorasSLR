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
 * @file tracking_slr.h
 * @author Degoras Project Team.
 * @brief This file contains the definition of the TrackingSLR class.
 * @copyright EUPL License
 * @version
***********************************************************************************************************************/

// =====================================================================================================================
#pragma once
// =====================================================================================================================

// LIBDEGORASSLR INCLUDES
// =====================================================================================================================
#include "LibDegorasSLR/libdegorasslr_global.h"
#include "LibDegorasSLR/UtilitiesSLR/predictor_slr/predictor_slr.h"
#include "LibDegorasSLR/Astronomical/sun_utils/predictor_sun.h"
#include "LibDegorasSLR/Timing/types/time_types.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace utils{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using dpslr::timing::types::HRTimePointStd;
using dpslr::timing::types::MJDate;
using dpslr::timing::types::SoD;
using astro::SunPosition;
using astro::PredictorSun;
// ---------------------------------------------------------------------------------------------------------------------

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
class LIBDPSLR_EXPORT PredictorMountSLR
{
public:

    /**
     * @brief Enumerates the possible status codes for a tracking position.
     *
     * This enumeration defines the status of a tracking position with respect to the Sun's position and the predictor.
     * It is used to quickly identify the tracking scenario and take appropriate action based on the status.
     */
    enum class PositionStatus
    {
        OUTSIDE_SUN,        ///< The final mount position is outside the sun.
        INSIDE_SUN,         ///< The final mount position is in the Sun and is configured for not avoiding.
        AVOIDING_SUN,       ///< The final mount position is avoiding sun security sector.
        CANT_AVOID_SUN,     ///< Final mount position can't be calculated, since it cannot avoid sun security sector.
        OUT_OF_TRACK,       ///< The time provided for prediction is outside of tracking.
        PREDICTION_ERROR    ///< The object position can't be calculated, there was a SLR prediction error.
    };

    /**
     * @brief The SunCollisionSector struct contains data of a sector where this space object tracking
     * passes through the sun security sector.
     */
    struct SunCollisionSector
    {
        long double az_entry;       ///< Azimuth of sun sector entry point.
        long double az_exit;        ///< Azimuth of sun sector exit point.
        long double el_entry;       ///< Elevation of sun sector entry point.
        long double el_exit;        ///< Elevation of sun sector exit point.
        MJDateTime mjdt_entry;      ///< MJ datetime of sun sector entry point.
        MJDateTime mjdt_exit;       ///< MJ datetime of sun sector exit point.
        bool cw;                    ///< Rotation direction of the avoidance manoeuvre (true = cw, false = ccw).
    };

    /// Alias for vector of SunCollisionSector.
    using SunCollisionSectors = std::vector<SunCollisionSector>;

    /**
     * @brief Represents the azimuth and elevation position that the tracking mount must use.
     *
     * This structure holds the calculated azimuth and elevation angles that the tracking mount must use for track
     * a space object. It also includes the differences between the real space object predicted position and the
     * track position. The necessity to deviate from the predicted path to avoid direct line-of-sight with the Sun or
     * other obstructions can result in these differences.
     */
    struct MountPosition
    {
        long double az;      ///< Azimuth for the tracking mount in degrees.
        long double el;      ///< Elevation for the tracking mount in degrees.
        long double diff_az; ///< Azimuth difference between space object prediction position and tracking position.
        long double diff_el; ///< Elevation difference between space object prediction position and tracking position.
    };

    /**
     * @brief Represents the result of a tracking prediction operation, including azimuth and elevation position that
     * the tracking mount must use for a given time to track an space object.
     * If status is either OUTSIDE_SUN, INSIDE_SUN OR AVOIDING_SUN, all result members are available.
     * This structure is designed to encapsulate the outcome of a tracking prediction operation (from the function
     * `predictTrackingPosition`. It includes timing information, tracking position, prediction results, and sun
     * position, depending on the position `status` information. See the **Note** section for more details.
     *
     * @note The presence of optional members is contingent upon the PositionStatus `status` member:
     * - If `status` is `OUT_OF_TRACK`, the optional members (`prediction_result`, `tracking_position`, `sun_pos`)
     *   are not populated.
     * - If `status` is `PREDICTION_ERROR` or `CANT_AVOID_SUN`, both `prediction_result` and `sun_pos` are provided to
     *   detail the prediction outcome and solar interference, respectively.
     */
    struct MountSLRPrediction
    {
        // Datetime members.
        MJDate mjd;              ///< Modified Julian Date in days.
        SoD sod;                 ///< Second of day in that Modified Julian Date.
        MJDateTime mjdt;         ///< Modified Julian DateTime (day & fraction).

        // Result members.
        Optional<PredictorSLR::SLRPrediction> prediction_result;  ///< SLR prediction result.
        Optional<MountPosition> tracking_position;             ///< Tracking position.
        Optional<SunPosition> sun_pos;       ///< Sun position.

        // Status.
        PositionStatus status;  ///< The current postion status.
    };

    /// Alias for Tracking results vector.
    using MountSLRPredictions = std::vector<MountSLRPrediction>;

    struct MountTrackSLR
    {
        // Constructor.
        MountTrackSLR(const CPF& cpf, const PredictorSLR& predictor_slr, const PredictorSun& predictor_sun);

        // TODO: velocities

        // Date and times.

        // CREAR ESTRUCTURA INTERMEDIA PARA ALMACENAR INFO DEL TRACK Y DEL PASE
        // REPETIR ESTO PARA TRACK VS PASS
        // TODO Check times.
        MJDate mjd_start;
        SoD sod_start;
        MJDate mjd_end;
        SoD sod_end;
        MJDate mjd_max_elev;
        SoD sod_max_elev;

        // Elevations.
        // TODO Diferenciar entre pase y start.
        // PONER LAS DEL TRACK NO LAS DEL PASE.
        long double start_elev;     ///< Track start elevation (degrees).
        long double end_elev;       ///< Track end elevation.
        long double max_elev;       ///< TODO

        // ======================================================================

        // AÑADIR LAS DEL PASE? Y como para que quede bien.

        // Flags.
        bool valid_pass;              ///< Flag indicating if the pass is valid.
        bool sun_collision;           ///< Flag indicating if the pass has a collision with the Sun.
        bool sun_collision_at_start;  ///< Flag indicating if the pass has a collision at start with the Sun.
        bool sun_collision_at_end;    ///< Flag indicating if the pass has a collision at end with the Sun.

        // Configurations.
        bool cfg_sun_avoid;            ///< Flag indicating if the track is configured for avoid the Sun.
        unsigned cfg_time_delta;       ///< Time delta fo calculations in milliseconds.
        unsigned cfg_sun_avoid_angle;  ///< Avoid angle for Sun collisions in degrees.
        unsigned cfg_min_elev;         ///< Configured minimum elevation (degrees).

        // Result containers.
        SunCollisionSectors sun_sectors;    ///< Sun sectors in the track for the required time interval.
        MountSLRPredictions predictions;    ///< Predicted data for the required time interval.

        // CPF and predictors.
        const CPF& cpf;
        const PredictorSLR& predictor_slr;
        const PredictorSun& predictor_sun;
    };


    PredictorMountSLR(PredictorSLR&& predictor, MJDate mjd_start, SoD sod_start, MJDate mjd_end, SoD sod_end,
                unsigned min_elev_deg = 10, unsigned time_delta_ms = 1000, bool sun_avoid = true,
                unsigned sun_avoid_angle = 15);

    PredictorMountSLR(PredictorSLR&& predictor, const HRTimePointStd& tp_start, const HRTimePointStd& tp_end,
                unsigned min_elev_deg = 10, unsigned time_delta_ms = 1000, bool sun_avoid = true,
                unsigned sun_avoid_angle = 15);

    /**
     * @brief This function checks if there is a valid SLR tracking. You should check this, before requesting positions.
     * @return true if there is a valid tracking, false otherwise.
     */
    bool isValid() const;

    /**
     * @brief This function returns the tracking info available.
     * @return the tracking info.
     */
    const MountTrackSLR& getMountTrack() const;

    /**
     * @brief This function returns the minimum elevation of this tracking in degrees.
     * @return the minimum elevation of the tracking in degrees.
     */
    unsigned getMinElev() const;

    /**
     * @brief If this traking is valid, you can get the tracking start with this function. This start time
     * could be different from the start time of the space object pass.
     * @param mjd, the MJ date in days for the tracking start.
     * @param sod, the second of day for the tracking start.
     */
    void getTrackingStart(MJDate &mjd, SoD& sod) const;

    /**
     * @brief If this tracking is valid, you can get the tracking end with this function. This end time
     * could be different from the end time of the space object pass.
     * @param mjd, the MJ date in days for the tracking end.
     * @param sod, the second of day for the tracking end.
     */
    void getTrackingEnd(MJDate &mjd, SoD& sod) const;

    /**
     * @brief This function returns an interator to the first valid position in tracking.
     * @return an interator to the first valid position in tracking, if tracking is valid. Otherwise end iterator.
     */
    MountSLRPredictions::const_iterator getTrackingBegin() const;

    /**
     * @brief This function returns an interator to the last valid position in tracking.
     * @return an interator to the last valid position in tracking, if tracking is valid. Otherwise end iterator.
     */
    MountSLRPredictions::const_iterator getTrackingEnd() const;

    /**
     * @brief This function returns if sun avoidance is applied.
     * @return true if sun avoidance is applied, false otherwise.
     */
    bool getSunAvoidApplied() const;

    /**
     * @brief This function returns if there is sun overlapping in this tracking.
     * @return true if sun avoid is applied and there is an overlapping with the sun, false otherwise.
     */
    bool isSunOverlapping() const;

    /**
     * @brief This function returns if the start time of the tracking was modified due to a sun collision
     * at the begining.
     * @return if sun avoid is applied, it returns true if tracking start time was modified due to sun, false otherwise.
     */
    bool isSunAtStart() const;

    /**
     * @brief This function returns if the end time of the tracking was modified due to a sun collision at the end.
     * @return if sun avoid is applied, it returns true if tracking end time was modified due to sun, false otherwise.
     */
    bool isSunAtEnd() const;

    /**
     * @brief This function returns the radius of the sun security sector applied to sun avoidance manouvre.
     *        This function should not be called if sun avoidance is not applied.
     * @return the radius of the sun security sector
     */
    unsigned getSunAvoidAngle() const;

    /**
     * @brief This function returns the object's position at a given time.
     * @param tp_time The time point datetime.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     */
    PositionStatus predict(const timing::HRTimePointStd& tp_time, MountSLRPrediction &tracking_result);

    /**
     * @brief This function returns the object's position at a given time.
     * @param mjd, the MJD in days.
     * @param sod, the second of day in seconds.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     */
    PositionStatus predict(MJDate mjd, SoD sod, MountSLRPrediction &tracking_result);

private:

    // Private functions.

    /// Helper to analyze the track.
    void analyzeTracking();

    /// Helper to check the tracking start.
    bool checkTrackingStart();

    /// Helper to check the tracking end.
    bool checkTrackingEnd();

    /// Helper to check the tracking.
    bool checkTracking();

    /// Helper to check if the predicted position is inside a sun sector.
    bool insideSunSector(const InstantData& pos, const SunPosition &sun_pos) const;

    /// Helper to set the rotation direction of a sun sector.
    bool setSunSectorRotationDirection(
        SunCollisionSector &sector, MountSLRPredictions::const_iterator sun_start, MountSLRPredictions::const_iterator sun_end);

    /// Helper to check positions whithin a sun sector to see if it is possible to avoid sun
    void checkSunSectorPositions(
        const SunCollisionSector &sector, MountSLRPredictions::iterator sun_start, MountSLRPredictions::iterator sun_end);

    long double calcSunAvoidTrajectory(MJDateTime mjdt, const SunCollisionSector &sector, SunPosition &sun_pos);

    // Private members.
    PredictorSLR predictor_;               ///< SLR predictor.
    astro::PredictorSun sun_predictor_;    ///< Sun predictor.
    MountTrackSLR mount_track_;            ///< Mount track analyzed data.

    MountSLRPredictions::iterator tracking_begin_;
    MountSLRPredictions::iterator tracking_end_;
};

}} // END NAMESPACES
// =====================================================================================================================
