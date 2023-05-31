/*
#include <iostream>
#include <iomanip>
#include <string>
#include <ratio>

#include "includes/math.h"

namespace dpslr {
namespace units {

// ========== CONSTANTS ================================================================================================
// Conversion using multiplying factors.
constexpr long double kfLightSecondToMeter = math::c;
constexpr long double kfArcdegreeToRadian = math::pi / 180.0;
constexpr long double kfArcdegreeToGradian = 200.0 / 180.0L;
constexpr long double kfGradianToRadian = math::pi / 200.0;
constexpr long double kfGradianToArcdegree = 1.0 / kfArcdegreeToGradian;
constexpr long double kfRadianToGradian = 1.0 / kfGradianToRadian;
constexpr long double kfRadianToArcdegree = 1.0 / kfArcdegreeToRadian;
constexpr long double kfFootToMeter = 0.3048L;
constexpr long double kfMeterToFoot = 1.0 / kfFootToMeter;
// Conversion using direct sums.
constexpr double ksCelsiusToKelvin = 273.15;
constexpr double ksKelvinToCelsius = -ksCelsiusToKelvin;
// Conversion using lambda formulas.
auto klFahrenheitToCelsius = [] (long double f) -> long double {return (f - 32.0)*5.0/9.0;};
auto klCelsiusToFahrenheit = [] (long double c) -> long double {return (c * 9.0/5.0) + 32;};
auto klKelvinToFahrenheit  = [] (long double k) -> long double {return (klCelsiusToFahrenheit(k+ksKelvinToCelsius));};
auto klFahrenheitToKelvin  = [] (long double f) -> long double {return (klFahrenheitToCelsius(f)+ksCelsiusToKelvin);};
// Custom ratios.
typedef std::ratio<1, 3600> ra_arcsec;
typedef std::ratio<1, 60> ra_arcmin;
typedef std::ratio<60, 1> ra_minute;
typedef std::ratio<3600, 1> ra_hour;
typedef std::ratio<86400, 1> ra_day;
// =====================================================================================================================





template <typename Ratio = std::ratio<1>, typename T = double>
class ScalingUnit
{
public:

    explicit ScalingUnit(const T &u = T()) : unit_(u), runit_(u) {}

    inline long double ratio() const {return this->ratio_;};

    inline constexpr operator const T&() const {return this->unit_;};
    inline operator T&() {return this->unit_;};

    template <typename RatioOut = std::ratio<1>, class TOut = double>
    operator ScalingUnit<RatioOut, TOut>() const
    {
        long double ratio_out = static_cast<long double>(RatioOut::den)/RatioOut::num;
        return ScalingUnit<RatioOut, TOut>(static_cast<TOut>(this->runit_ * this->ratio_ * ratio_out));
    };

    template <typename RatioOut = std::ratio<1>, typename TOut = double>
    ScalingUnit<RatioOut, TOut> scale() const
    {
        long double ratio_out = static_cast<long double>(RatioOut::den)/RatioOut::num;
        return ScalingUnit<RatioOut, TOut>(static_cast<TOut>(this->runit_ * this->ratio_ * ratio_out));
    }

    static T scale(const ScalingUnit& su)
    {
        return static_cast<T>(su * su.ratio() * ratio_);
    };

    template <typename RatioOut = std::ratio<1>>
    static T scale(const T& val)
    {
        long double ratio_out = static_cast<long double>(RatioOut::den)/RatioOut::num;
        return static_cast<T>(val * ratio_ * ratio_out);
    };

private:

    static constexpr long double ratio_ = static_cast<long double>(Ratio::num)/Ratio::den;
    T unit_;
    T runit_;
};

namespace operators
{
template <typename Ratio, typename T>
std::ostream& operator<< (std::ostream& os, const ScalingUnit<Ratio,T>& su)
{
    os << static_cast<T>(su);
    return os;
}
}


template <typename OrigUnit, typename DestUnit, typename T>
struct transform
{
    ScalingUnit<std::ratio<1>, T> operator()(const ScalingUnit<std::ratio<1>, T>&) = delete;
};



template <typename T> static constexpr bool sum_explicit = false;




template <typename SmartUnit, typename T>
struct transform<SmartUnit, SmartUnit, T>
{
    ScalingUnit<std::ratio<1>,T> operator()(const ScalingUnit<std::ratio<1>, T>& su){return su;}
};

template <typename Unit, typename Ratio = std::ratio<1>, typename T = double>
class SmartUnit : public ScalingUnit<Ratio,T>
{
public:

    static constexpr bool enable_sum = true;

    explicit SmartUnit(const T &t = T()) : ScalingUnit<Ratio,T>(t) {}

    template<typename OtherRatio, typename OtherT>
    explicit constexpr SmartUnit(const SmartUnit<Unit, OtherRatio, OtherT>& other_m)  :
        ScalingUnit<Ratio, T>(ScalingUnit<>::scale(other_m))
    {}

    template<typename OtherUnit, typename OtherRatio, typename OtherT>
    explicit constexpr SmartUnit(const SmartUnit<OtherUnit, OtherRatio, OtherT>& other_m)  :
        ScalingUnit<Ratio, T> (transform<Unit, OtherUnit, T>(other_m))
    {}

    template <typename OtherRatio, typename OtherT>
    constexpr operator SmartUnit<Unit, OtherRatio, OtherT> () const
    {
        long double ratio_out = static_cast<long double>(OtherRatio::den)/OtherRatio::num;
        return SmartUnit<Unit, OtherRatio, OtherT>(static_cast<OtherT>(*this * ratio_out * this->ratio()));
    }

    template <typename OtherUnit, typename OtherRatio, typename OtherT>
    constexpr operator SmartUnit<OtherUnit, OtherRatio, OtherT> () const
    {
        return SmartUnit<OtherUnit, OtherRatio, OtherT>(transform<Unit, OtherUnit, T>()(*this));
    }


    template <typename Ratio2, typename T2>
    SmartUnit<Unit, std::ratio<1>, std::common_type_t<T,T2>> operator + (const SmartUnit<Unit, Ratio2, T2>& b)
    {
        using tcom = std::common_type_t<T,T2>;
        SmartUnit<Unit, std::ratio<1>, tcom> a_1 = *this;
        SmartUnit<Unit, std::ratio<1>, tcom> b_1 = b;
        return SmartUnit<Unit, std::ratio<1>, tcom>(static_cast<tcom>(a_1) + static_cast<tcom>(b_1));
    }

    template <typename Unit2, typename Ratio2, typename T2>
    SmartUnit<Unit, std::ratio<1>, std::common_type_t<T,T2>> operator + (const SmartUnit<Unit2, Ratio2, T2>& b)
    {
        static_assert (sum_explicit<Unit>, "Error");
        using tcom = std::common_type_t<T,T2>;
        SmartUnit<Unit, std::ratio<1>, tcom> a_1 = *this;
        SmartUnit<Unit2, std::ratio<1>, tcom> b_1 = b;
        SmartUnit<Unit, std::ratio<1>, tcom> aux(transform<Unit2, Unit, tcom>()(b_1));
        return SmartUnit<Unit, std::ratio<1>, tcom>(static_cast<tcom>(a_1) + static_cast<tcom>(aux));
    }

    template <typename Ratio2, typename T2>
    SmartUnit<Unit, std::ratio<1>, std::common_type_t<T,T2>> operator - (const SmartUnit<Unit, Ratio2, T2>& b)
    {
        using tcom = std::common_type_t<T,T2>;
        SmartUnit<Unit, std::ratio<1>, tcom> a_1 = *this;
        SmartUnit<Unit, std::ratio<1>, tcom> b_1 = b;
        return SmartUnit<Unit, std::ratio<1>, tcom>(static_cast<tcom>(a_1) - static_cast<tcom>(b_1));
    }

    template <typename Unit2, typename Ratio2, typename T2>
    SmartUnit<Unit, std::ratio<1>, std::common_type_t<T,T2>> operator - (const SmartUnit<Unit2, Ratio2, T2>& b)
    {
        static_assert (sum_explicit<Unit>, "Error");
        using tcom = std::common_type_t<T,T2>;
        SmartUnit<Unit, std::ratio<1>, tcom> a_1 = *this;
        SmartUnit<Unit2, std::ratio<1>, tcom> b_1 = b;
        SmartUnit<Unit, std::ratio<1>, tcom> aux(transform<Unit2, Unit, tcom>()(b_1));
        return SmartUnit<Unit, std::ratio<1>, tcom>(static_cast<tcom>(a_1) - static_cast<tcom>(aux));
    }


    template <typename Unit2, typename Ratio2, typename T2>
    T operator * (const SmartUnit<Unit2, Ratio2, T2>& b) = delete;

    template <typename Unit1, typename Unit2, typename Ratio1, typename Ratio2, typename T1, typename T2>
    SmartUnit<Unit1, Ratio1, T1> operator * (const SmartUnit<Unit2, Ratio2, T2>& b) = delete;

    template <typename Unit2, typename Ratio2, typename T2>
    T operator / (const SmartUnit<Unit2, Ratio2, T2>& b) = delete;

    template <typename Unit1, typename Unit2, typename Ratio1, typename Ratio2, typename T1, typename T2>
    SmartUnit<Unit1, Ratio1, T1> operator / (const SmartUnit<Unit2, Ratio2, T2>& b) = delete;



};

template <class Unit1, class Unit2, class Ratio1, class Ratio2, class T1, class T2>
SmartUnit<Unit1, std::ratio<1>, std::common_type_t<T1,T2>> operator + (
    const SmartUnit<Unit1, Ratio1, T1>& a, const SmartUnit<Unit2, Ratio2, T2>& b)
{
    using tcom = std::common_type_t<T1,T2>;
    SmartUnit<Unit1, std::ratio<1>, tcom> a_1 = a;
    SmartUnit<Unit2, std::ratio<1>, tcom> b_1 = b;
    //SmartUnit<Unit3, std::ratio<1>, tcom> a_t(transform<Unit1, Unit3, tcom>()(a_1));
    SmartUnit<Unit1, std::ratio<1>, tcom> b_t(transform<Unit2, Unit1, tcom>()(b_1));
    return SmartUnit<Unit1, std::ratio<1>, tcom>(static_cast<tcom>(a_1) + static_cast<tcom>(b_1));
}

//template <typename Unit1, typename Unit2, typename Ratio1, typename Ratio2, typename T1, typename T2, typename T3>
//T3 operator + (const SmartUnit<Unit1, Ratio1, T1>& a, const SmartUnit<Unit2, Ratio2, T2>& b) = delete;

//template <class U1, class U2, class U3, class R1, class R2, class R3, class T1, class T2, class T3>
//SmartUnit<U3, R3, T3> operator + (const SmartUnit<U1, R1, T1>& a, const SmartUnit<U2, R2, T2>& b) = delete;


}}

static bool USE_UNIT_STREAM_SYMBOLS = false;

#define DPSLR_USE_UNIT_STREAM_SYMBOLS(BOOL) \
USE_UNIT_STREAM_SYMBOLS = BOOL;

#define DPSLR_DECLARE_MAIN_UNIT(UNIT_ID, UNIT_CLASS) \
namespace dpslr { namespace units { \
    struct UNIT_ID {static constexpr bool enable_bool = true;}; \
    template <typename T = double> using UNIT_CLASS = SmartUnit<UNIT_ID, std::ratio<1>, T>;}}

#define DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, UNIT_CLASS, RATIO) \
namespace dpslr { namespace units { \
    template <typename T = double> using UNIT_CLASS = SmartUnit<UNIT_ID, RATIO, T>;}}

#define DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, SYMBOL, RATIO) \
namespace dpslr { namespace units { namespace operators { \
    template<typename T> std::ostream&  operator<< (std::ostream& os, const SmartUnit<UNIT_ID, RATIO, T>& su) \
{if(USE_UNIT_STREAM_SYMBOLS) os << static_cast<T>(su)<< "_" << #SYMBOL;  \
        else os << static_cast<T>(su); return os;}}}}

#define DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, SYMBOL, RATIO) \
namespace dpslr { namespace units { namespace operators { \
    SmartUnit<UNIT_ID, RATIO, long double> operator "" _##SYMBOL (long double a) \
{return SmartUnit<UNIT_ID, RATIO, long double>(a);} \
    SmartUnit<UNIT_ID, RATIO, unsigned long long int> operator "" _##SYMBOL(unsigned long long int a) \
{return SmartUnit<UNIT_ID, RATIO, unsigned long long int>(a);}}}}

#define DPSLR_DECLARE_UNIT_CONVERSION_FACTOR(UNIT_ID_IN, UNIT_ID_OUT, FACTOR) \
namespace dpslr { namespace units { \
    template <typename T> struct transform<UNIT_ID_IN, UNIT_ID_OUT, T> \
{ScalingUnit< std::ratio<1>, T> constexpr operator() (const ScalingUnit<std::ratio<1>, T>& mag) \
    {return SmartUnit<UNIT_ID_OUT, std::ratio<1>, T>(mag * FACTOR);}};}}

#define DPSLR_DECLARE_UNIT_CONVERSION_SUM(UNIT_ID_IN, UNIT_ID_OUT, SUM) \
namespace dpslr { namespace units { \
    template <typename T> struct transform<UNIT_ID_IN, UNIT_ID_OUT, T> \
{ScalingUnit< std::ratio<1>, T> constexpr operator() (const ScalingUnit<std::ratio<1>, T>& mag) \
    {return SmartUnit<UNIT_ID_OUT, std::ratio<1>, T>(mag + SUM);}};}}

#define DPSLR_DECLARE_UNIT_CONVERSION_LAMBDA(UNIT_ID_IN, UNIT_ID_OUT, FUNCTOR) \
namespace dpslr { namespace units { \
    template <typename T> struct transform<UNIT_ID_IN, UNIT_ID_OUT, T> \
{ScalingUnit< std::ratio<1>, T> constexpr operator() (const ScalingUnit<std::ratio<1>, T>& mag) \
    {return SmartUnit<UNIT_ID_OUT, std::ratio<1>, T>(FUNCTOR(mag));}};}}

#define DPSLR_DECLARE_UNIT_SUM_EXPLICIT(UNIT_ID) \
namespace dpslr { namespace units { template<> bool sum_explicit<UNIT_ID> = true; }}


#define DPSLR_DECLARE_MAIN_UNIT_LITERAL(UNIT_ID, SYMBOL) \
DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, SYMBOL, std::ratio<1>)

#define DPSLR_DECLARE_MAIN_UNIT_STREAM(UNIT_ID, SYMBOL) \
DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, SYMBOL, std::ratio<1>)

#define DPSLR_EXPAND_MAIN_UNIT_PREFIXES(UNIT_ID, BASE, ...) \
DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Peta##BASE,  std::peta);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Tera##BASE,  std::tera);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Giga##BASE,  std::giga);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Mega##BASE,  std::mega);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Kilo##BASE,  std::kilo);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Hecto##BASE, std::hecto); \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Deca##BASE,  std::deca);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Deci##BASE,  std::deci);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Centi##BASE, std::centi); \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Milli##BASE, std::milli); \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Micro##BASE, std::micro); \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Nano##BASE,  std::nano);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Pico##BASE,  std::pico);  \
    DPSLR_DECLARE_PREFIX_UNIT(UNIT_ID, __VA_ARGS__##Femto##BASE, std::femto);

#define DPSLR_EXPAND_MAIN_UNIT_LITERALS(UNIT_ID, SYMBOL, ...) \
DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##P##SYMBOL,  std::peta);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##T##SYMBOL,  std::tera);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##G##SYMBOL,  std::giga);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##M##SYMBOL,  std::mega);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##k##SYMBOL,  std::kilo);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##h##SYMBOL,  std::hecto); \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##da##SYMBOL, std::deca);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##d##SYMBOL,  std::deci);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##c##SYMBOL,  std::centi); \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##m##SYMBOL,  std::milli); \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##u##SYMBOL,  std::micro); \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##n##SYMBOL,  std::nano);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##p##SYMBOL,  std::pico);  \
    DPSLR_DECLARE_UNIT_LITERAL(UNIT_ID, __VA_ARGS__##f##SYMBOL,  std::femto);

#define DPSLR_EXPAND_MAIN_UNIT_STREAM(UNIT_ID, SYMBOL, ...) \
DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##P##SYMBOL,  std::peta);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##T##SYMBOL,  std::tera);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##G##SYMBOL,  std::giga);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##M##SYMBOL,  std::mega);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##k##SYMBOL,  std::kilo);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##h##SYMBOL,  std::hecto); \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##da##SYMBOL, std::deca);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##d##SYMBOL,  std::deci);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##c##SYMBOL,  std::centi); \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##m##SYMBOL,  std::milli); \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##u##SYMBOL,  std::micro); \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##n##SYMBOL,  std::nano);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##p##SYMBOL,  std::pico);  \
    DPSLR_DECLARE_UNIT_STREAM(UNIT_ID, __VA_ARGS__##f##SYMBOL,  std::femto);

#define DPSLR_DECLARE_FULL_UNIT(UNIT_ID, UNIT_CLASS, SYMBOL) \
DPSLR_DECLARE_MAIN_UNIT(UNIT_ID, UNIT_CLASS) \
    DPSLR_EXPAND_MAIN_UNIT_PREFIXES(UNIT_ID, UNIT_ID) \
    DPSLR_DECLARE_MAIN_UNIT_LITERAL(UNIT_ID, SYMBOL) \
    DPSLR_EXPAND_MAIN_UNIT_LITERALS(UNIT_ID, SYMBOL) \
    DPSLR_DECLARE_MAIN_UNIT_STREAM(UNIT_ID, SYMBOL) \
    DPSLR_EXPAND_MAIN_UNIT_STREAM(UNIT_ID, SYMBOL)

    // International System of Units.
    // Second.
    DPSLR_DECLARE_FULL_UNIT(second, Second, s);
DPSLR_DECLARE_PREFIX_UNIT(second, Minute, ra_minute);
DPSLR_DECLARE_PREFIX_UNIT(second, Hour, ra_hour);
DPSLR_DECLARE_PREFIX_UNIT(second, Day, ra_day);
DPSLR_DECLARE_UNIT_LITERAL(second, min, ra_minute);
DPSLR_DECLARE_UNIT_LITERAL(second, h, ra_hour);
DPSLR_DECLARE_UNIT_LITERAL(second, d, ra_day);
DPSLR_DECLARE_UNIT_STREAM(second, min, ra_minute);
DPSLR_DECLARE_UNIT_STREAM(second, h, ra_hour);
DPSLR_DECLARE_UNIT_STREAM(second, d, ra_day);


DPSLR_DECLARE_FULL_UNIT(meter, Meter, m);

DPSLR_DECLARE_FULL_UNIT(gram, Gram, g);
DPSLR_DECLARE_FULL_UNIT(ampere, Ampere, A);
DPSLR_DECLARE_FULL_UNIT(kelvin, Kelvin, K);
DPSLR_DECLARE_UNIT_SUM_EXPLICIT(kelvin);
DPSLR_DECLARE_FULL_UNIT(celsius, Celsius, gC);
DPSLR_DECLARE_UNIT_SUM_EXPLICIT(celsius);
DPSLR_DECLARE_FULL_UNIT(pascal, Pascal, Pa);
DPSLR_DECLARE_FULL_UNIT(radian, Radian, rad);
DPSLR_DECLARE_FULL_UNIT(hertz, Hertz, Hz);

// Light-second.
DPSLR_DECLARE_MAIN_UNIT(lightsecond, LightSecond);
DPSLR_DECLARE_MAIN_UNIT_LITERAL(lightsecond, ls);
DPSLR_DECLARE_MAIN_UNIT_STREAM(lightsecond, ls);
DPSLR_EXPAND_MAIN_UNIT_LITERALS(lightsecond, s, l);
DPSLR_EXPAND_MAIN_UNIT_PREFIXES(lightsecond, second, Light);

// Degrees Fahrenheit.
DPSLR_DECLARE_MAIN_UNIT(fahrenheit, Fahrenheit);
DPSLR_DECLARE_MAIN_UNIT_LITERAL(fahrenheit, F);
DPSLR_DECLARE_MAIN_UNIT_STREAM(fahrenheit, F);
DPSLR_DECLARE_UNIT_SUM_EXPLICIT(fahrenheit);

// Degree of arc.
DPSLR_DECLARE_MAIN_UNIT(arcdegree, Arcdegree);
DPSLR_DECLARE_PREFIX_UNIT(arcdegree, Arcmin, ra_arcmin);
DPSLR_DECLARE_PREFIX_UNIT(arcdegree, Arcsec, ra_arcsec);
DPSLR_DECLARE_MAIN_UNIT_LITERAL(arcdegree, deg);
DPSLR_DECLARE_UNIT_LITERAL(arcdegree, arcmin, ra_arcmin);
DPSLR_DECLARE_UNIT_LITERAL(arcdegree, arcsec, ra_arcsec);
DPSLR_DECLARE_MAIN_UNIT_STREAM(arcdegree, deg);
DPSLR_DECLARE_UNIT_STREAM(arcdegree, arcmin, ra_arcmin);
DPSLR_DECLARE_UNIT_STREAM(arcdegree, arcsec, ra_arcsec);

// Gradian.
DPSLR_DECLARE_FULL_UNIT(gradian, Gradian, gon);


// Foot.
DPSLR_DECLARE_MAIN_UNIT(foot, Foot);
DPSLR_DECLARE_MAIN_UNIT_LITERAL(foot, ft);
DPSLR_DECLARE_MAIN_UNIT_STREAM(foot, ft);

// Conversions.
DPSLR_DECLARE_UNIT_CONVERSION_FACTOR(arcdegree, radian, dpslr::units::kfArcdegreeToRadian);
DPSLR_DECLARE_UNIT_CONVERSION_FACTOR(radian, arcdegree, dpslr::units::kfRadianToArcdegree);





DPSLR_DECLARE_UNIT_CONVERSION_FACTOR(foot, meter, dpslr::units::kfFootToMeter);
DPSLR_DECLARE_UNIT_CONVERSION_FACTOR(meter, foot, dpslr::units::kfMeterToFoot);
DPSLR_DECLARE_UNIT_CONVERSION_SUM(celsius, kelvin, dpslr::units::ksCelsiusToKelvin);
DPSLR_DECLARE_UNIT_CONVERSION_SUM(kelvin, celsius, dpslr::units::ksKelvinToCelsius);
DPSLR_DECLARE_UNIT_CONVERSION_LAMBDA(fahrenheit, kelvin, dpslr::units::klFahrenheitToKelvin);
DPSLR_DECLARE_UNIT_CONVERSION_LAMBDA(kelvin, fahrenheit, dpslr::units::klKelvinToFahrenheit);
DPSLR_DECLARE_UNIT_CONVERSION_LAMBDA(fahrenheit, celsius, dpslr::units::klFahrenheitToCelsius);
DPSLR_DECLARE_UNIT_CONVERSION_LAMBDA(celsius, fahrenheit, dpslr::units::klCelsiusToFahrenheit);



//DPSLR_DECLARE_CONVERSION(radian, degree, dpslr::geo::kRadianToDegree)

//DPSLR_DECLARE_BASE_MAGNITUDE(Radian, radian);
//DPSLR_DECLARE_BASE_MAGNITUDE(Degree, degree);


int main()
{
    // DPSLR namespaces.
    using namespace dpslr::units;
    using namespace dpslr::units::operators;


    Kilometer<> km_test_1 {5.0};     // Prueba inicialización 1.
    Kilometer<> km_test_2 = 5.0_km;     // Prueba inicialización 2.
    Meter<> m_test_1 = km_test_1;    // Ratio conversion test 1.
    Meter<> m_test_2(km_test_1);     // Ratio conversion test 2.
    Millimeter<> mm_test_1(km_test_1.scale<std::milli>());     // Ratio conversion test 3.
    Centimeter<> cm_test_1 = 20_m;     // Ratio conversion test 4.
    SmartUnit<foot> ft_test_1 = 3258.96_km;

    Kelvin<> k_t = 15_gC + 15_gC;


    DPSLR_USE_UNIT_STREAM_SYMBOLS(true);
    std::cout << std::fixed;
    std::cout << std::setprecision(11);



    return 0;
}
*/
