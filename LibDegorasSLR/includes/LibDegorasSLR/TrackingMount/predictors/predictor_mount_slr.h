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
#include "LibDegorasSLR/UtilitiesSLR/predictors/predictor_cpf.h"
#include "LibDegorasSLR/Astronomical/predictors/predictor_sun.h"
#include "LibDegorasSLR/Timing/types/base_time_types.h"
#include "LibDegorasSLR/Mathematics/units/strong_units.h"
#include "LibDegorasSLR/Astronomical/types/astro_types.h"
#include "LibDegorasSLR/FormatsILRS/cpf/cpf.h"
// =====================================================================================================================

// LIBDPSLR NAMESPACES
// =====================================================================================================================
namespace dpslr{
namespace mount{
// =====================================================================================================================

// ---------------------------------------------------------------------------------------------------------------------
using timing::types::HRTimePointStd;
using timing::types::MJDateTime;
using timing::types::MJDate;
using timing::types::SoD;
using astro::PredictorSun;
using astro::types::AltAzPos;
using astro::types::AltAzPosV;
using math::units::DegreesU;
using math::units::Degrees;
using math::units::MillisecondsU;
using math::units::Meters;
using utils::PredictorSLR;
using utils::SLRPrediction;
using utils::SLRPredictions;
using ilrs::cpf::CPF;
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

    enum class RotationDirection
    {
        CLOCKWISE,
        COUNTERCLOCKWISE
    };

    /**
     * @brief The SunCollisionSector struct contains data of a sector where the space object real pass
     * crosses a Sun security sector.
     */
    struct SunCollisionSector
    {
        // Meter las coordenadas del sol
        // Actualizar rotation al limitar la elevacion.
        // Actualizar todo cuando se limita la elevacion.

        AltAzPosV altaz_sun_coords;  ///< Altazimuth coordinates of the Sun during the collision time in degrees.
        AltAzPos altaz_entry;        ///< Sun sector altazimuth entry point coordinate in degrees.
        AltAzPos altaz_exit;         ///< Sun sector altazimuth exit point coordinate in degrees.
        MJDateTime mjdt_entry;            ///< MJ datetime of sun sector entry point.
        MJDateTime mjdt_exit;             ///< MJ datetime of sun sector exit point.
        RotationDirection cw;             ///< Rotation direction of the avoidance manoeuvre.
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
        MountPosition(const AltAzPos& pos) :
            altaz_coord(pos),
            diff_az(0.0L),
            diff_el(0.0L)
        {}

        MountPosition() :
            altaz_coord(AltAzPos()),
            diff_az(0.0L),
            diff_el(0.0L)
        {}

        AltAzPos altaz_coord;  ///< Altazimuth coordinate for the tracking mount in degrees.
        Degrees diff_az; ///< Azimuth difference between space object prediction position and tracking position.
        Degrees diff_el; ///< Elevation difference between space object prediction position and tracking position.
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
        MountSLRPrediction() = default;

        // utilizar el nuevo datetime.

        // Datetime members.
        MJDateTime mjdt;         ///< Modified Julian DateTime.

        // Result members.
        Optional<SLRPrediction> slr_pred;  ///< Optional SLR prediction with the object pass position.
        Optional<PredictorSun::SunPrediction> sun_pred;  ///< Optional Sun position container.
        Optional<MountPosition> mount_pos;               ///< Optional tracking moun position container.

        // Status.
        PositionStatus status;  ///< The current postion status.
    };

    /// Alias for Tracking results vector.
    using MountSLRPredictions = std::vector<MountSLRPrediction>;

    /// TODO add max el and az speeds.
    // ESTA ES LA CONFIGURACION QUE LE HAS PASADO AL SISTEMA
    struct PredictorMountSLRConfig
    {
        // DIVIDIR EN DOS
        //MountTrackAnalyzerConfig??? -> time_delta, angle, elevations, sun_avoid, speeds.
        // Realmente el analizador solo necesita lo anterior + vector de tiempos + vector de posiciones.
        // Que estaria bien que pusieran un MountTrackGenerator que generele los anteriores vectores.

        // En caso de que en desplazamientos absolutos o relativos, si la montura empieza en un sector de Sol, esta
        // ya sería una posicion con estado de CANT_AVOID_SUN (la montura no puede teletransportarse). Concretamente,
        // en la montura AMELAS de SFEL por ejemplo, esto produciria un estado de error que el observador debe
        // solucionar manualmente, pues es una situación que nunca debería haber ocurrido.

        //Esta tendria la MountTrackAnalyzerConfig
        // Datos de tiempo

        // Basic data.
        MJDateTime mjdt_start;
        MJDateTime mjdt_end;
        MillisecondsU time_delta;  ///< Time delta fo calculations in milliseconds.
        DegreesU sun_avoid_angle;  ///< Avoid angle for Sun collisions in degrees.
        DegreesU min_elev;         ///< Configured minimum elevation (degrees).
        DegreesU max_elev;         ///< Configured maximum elevation (degrees).
        bool sun_avoid;            ///< Flag indicating if the track is configured for avoid the Sun.
    };

    // ESTA ES LA INFORMACION BASICA DEL TRACK YA ANALIZADO
    struct TrackInfo
    {
        TrackInfo() :
            valid_pass(false),
            sun_deviation(false),
            sun_collision(false),
            sun_collision_at_middle(false),
            sun_collision_at_start(false),
            sun_collision_at_end(false),
            trim_at_start(false),
            trim_at_end(false),
            el_deviation(false)
        {}

        // Time data.
        MJDateTime mjdt_start;
        MJDateTime mjdt_end;

        // Position data.
        AltAzPos start_coord;    ///< Track start altazimuth coordinates.
        AltAzPos end_coord;      ///< Track end altazimuth  coordinates.
        Degrees max_el;               ///< Track maximum elevation in degrees.

        // Flags.
        bool valid_pass;              ///< Flag indicating if the pass is valid.

        // PA LA OTRA CLASE
        bool sun_deviation;           ///< Flag indicating if the track was deviated from pass due to Sun.
        bool sun_collision;           ///< Flag indicating if the pass has a collision with the Sun.
        bool sun_collision_at_middle; ///< Flag indicating if the pass has a collision at middle with the Sun.
        bool sun_collision_at_start;  ///< Flag indicating if the pass has a collision at start with the Sun.
        bool sun_collision_at_end;    ///< Flag indicating if the pass has a collision at end with the Sun.
        bool trim_at_start;           ///< Flag indicating if the pass was trimmed due to elevation or Sun at start.
        bool trim_at_end;             ///< Flag indicating if the pass was trimmed due to elevation or Sun at end.
        bool el_deviation;            ///< Flag indicating if the track was deviated from pass due to max elevation.
    };

    // TODO MOVE TO PASS GENERATOR
    struct PassInfo
    {
        // Eleve max
        // CW
        // Azi start
        //
    };

    struct MountTrackSLR
    {
        // Constructor.
        MountTrackSLR(const PredictorSLR& predictor_slr, const PredictorSun& predictor_sun);

        // Containers
        PredictorMountSLRConfig config;    ///< Contains the PredictorMountSLR user configuration.
        TrackInfo track_info;              ///< Contains the analyzed tracking information.
        //TODO PassInfo

        SunCollisionSectors sun_sectors;    ///< Sun sectors in the track for the required time interval.
        MountSLRPredictions predictions;    ///< Predicted data for the required time interval.

        // Predictors.
        const PredictorSLR& predictor_slr;    ///< Internal PredictorSLR predictor.
        const PredictorSun& predictor_sun;    ///< Internal Sun predictor.
    };

    // TODO Use the maximum elevations.

    // Predictor tendria que recibir PredictorSLR, PredictorSun (virtual), TrackAnalizerConfig, MJDatetime start y end.

    PredictorMountSLR(const PredictorSLR &predictor, MJDateTime mjdt_start, MJDateTime mjdt_end,
                      MillisecondsU time_delta = 1000, DegreesU min_elev = 10, DegreesU max_elev = 85,
                      DegreesU sun_avoid_angle = 15, bool sun_avoid = true);

    PredictorMountSLR(const PredictorSLR& predictor, const HRTimePointStd& tp_start, const HRTimePointStd& tp_end,
                      MillisecondsU time_delta = 1000, DegreesU min_elev = 10, DegreesU max_elev = 85,
                      DegreesU sun_avoid_angle = 15, bool sun_avoid = true);

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
     * @brief If this traking is valid, you can get the tracking start with this function. This start time
     * could be different from the start time of the space object pass.
     * @param mjd, the MJ datetime in days for the tracking start.
     */
    void getTrackingStart(MJDateTime &mjdt) const;

    /**
     * @brief If this tracking is valid, you can get the tracking end with this function. This end time
     * could be different from the end time of the space object pass.
     * @param mjd, the MJ datetime in days for the tracking end.
     * @param sod, the second of day for the tracking end.
     */
    void getTrackingEnd(MJDateTime &mjdt) const;

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
     * @brief This function returns the object's position at a given time.
     * @param tp_time The time point datetime.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     *
     * @warning Nanoseconds resolution for the prediction.
     */
    PositionStatus predict(const timing::HRTimePointStd& tp_time, MountSLRPrediction &tracking_result);

    /**
     * @brief This function returns the object's position at a given time.
     * @param mjd, the MJD in days.
     * @param tracking_result, the returned TrackingResult struct.
     * @return the result of the operation. Must be checked to ensure the position is valid.
     */
    PositionStatus predict(const MJDateTime &mjd, MountSLRPrediction &tracking_result);

private:

    // Private functions.

    /// Helper to analyze the track.
    void analyzeTracking();

    /// Helper to check the tracking start.
    bool analyzeTrackingStart();

    /// Helper to check the tracking end.
    bool analyzeTrackingEnd();

    /// Helper to check the tracking.
    bool analyzeTrackingMiddle();

    /// Helper to check if the predicted position is inside a sun sector.
    bool insideSunSector(const AltAzPos& pass_pos, const AltAzPos& sun_pos) const;

    /// Helper to set the rotation direction of a sun sector.
    bool setSunSectorRotationDirection(
        SunCollisionSector &sector, MountSLRPredictions::const_iterator sun_start, MountSLRPredictions::const_iterator sun_end);

    /// Helper to check positions whithin a sun sector to see if it is possible to avoid sun
    void checkSunSectorPositions(
        const SunCollisionSector &sector, MountSLRPredictions::iterator sun_start, MountSLRPredictions::iterator sun_end);

    long double calcSunAvoidTrajectory(const MJDateTime& mjdt, const SunCollisionSector &sector,
                                       const AltAzPos &sun_pos);

    // Private members.
    const PredictorSLR& predictor_;              ///< SLR predictor.
    astro::PredictorSun sun_predictor_;    ///< Sun predictor.
    MountTrackSLR mount_track_;            ///< Mount track analyzed data.

    MountSLRPredictions::iterator tracking_begin_;
    MountSLRPredictions::iterator tracking_end_;
};

}} // END NAMESPACES
// =====================================================================================================================
