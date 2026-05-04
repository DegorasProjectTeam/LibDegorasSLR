// Includes

#include <cassert>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <format>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <numeric>

#include <LibDegorasBase/Mathematics/types/matrix.h>
#include <LibDegorasBase/Statistics/fitting.h>
#include <LibDegorasBase/Statistics/histogram.h>
#include <LibDegorasBase/Statistics/measures.h>
#include <LibDegorasBase/Helpers/string_helpers.h>
#include <LibDegorasBase/Helpers/filedir_helpers.h>
#include <LibDegorasBase/Mathematics/operators/operators.h>
#include <LibDegorasBase/Mathematics/math_constants.h>
#include <LibDegorasBase/Mathematics/utils/math_utils.h>
#include <LibDegorasSLR/Geophysics/utils/meteo_utils.h>
#include <LibDegorasSLR/ILRS/algorithms/statistics.h>

using dpbase::math::types::Matrix;
using dpbase::math::kPi;
namespace fs = std::filesystem;
using dpbase::math::operators::operator +;
using dpbase::math::operators::operator -;
using dpbase::math::operators::operator *;
using dpbase::math::operators::operator /;
using dpbase::math::operators::operator ==;
using dpbase::math::operators::operator !=;
using dpbase::math::operators::operator >=;
using dpbase::math::operators::operator <=;
using dpbase::math::operators::operator >;
using dpbase::math::operators::operator <;
using dpbase::math::operators::operator &&;
using dpbase::math::operators::operator ||;
using dpbase::math::where;
using dpbase::math::sortValArg;
using dpbase::math::abs;
using dpbase::math::sqrt;
using dpbase::math::sin;
using dpbase::math::cos;
using dpbase::math::tan;
using dpbase::math::asin;
using dpbase::math::acos;
using dpbase::math::atan;

namespace fs = std::filesystem;

// This version of the program is only the algorithm, only the base of program
// not the final version, is only a guide of the algorithm.
// TODO Structuring the program
// Initializations & Parameters
void main()
{
    std::size_t nu = 13;  // lsq size
    long double sol = 299792458.0;  // Speed of Light m/s
    long double swi = 1.5e-8;  // weighting applied to residual fitting
    long double dae = 6.378137000;  // PARAMETERS OF SPHEROID
    long double df = 298.2570; // Inverse flattening
    std::string CsysID = "--";  // CRD system configuration ID
    std::string FRdata = "";  // ILRS full-rate date file in CRD format
    std::string CPFin = "";  // ILRS XYZ orbit prediction in CPF format
    long double INmjd = 0.0;  // Input the MJD for raw data sets
    std::string STAT_id = "";  // File containing the local meteorological data
    std::string STAT_name = "";  // Station name
    std::string STAT_abv = "";  // Satellite abbreviation
    std::string STAT_coords = "";  // Station coordinates string
    std::string SATtarget_name = "";  // Satellite name
    std::string savepassname = "";  // Save name
    long double frate = 0.0;  // Laser fire rate
    bool SNXref = true;  // Flag to acquire station coordinates fron SINEX file
    bool METap = false;  // Flag to show met data applied
    long double PWidth = 0.0;  // Laser pulse width
    bool PWadjust = false;  // Adjust Gaussian fit by considering laser pulse width
    long double Wavel = 532.0;  // Laser pulse width
    bool plotRES = false;  // Flag to plot and save the results, no display
    bool clipGauss = false;  // GAUSS
    bool clipsigma = false;  // Set residual clipping  wrt to calculated RMS
    bool clipLEHM = false;  // Set residual clipping in front and behind the LEHM
    bool clipWIDE = false;  // Clip residuals at wider limits for inclusion in full-rate output
    bool NPout = false;  // Output normal points to file
    bool FRin = false;  // Input epoch ranges in full-rate CRD
    bool Unfilter = false;  // Flag to include the data in the CRD full-rate data that is flagged as 'noise'
    std::vector<std::string> setupWarningList = {};  // setup warnings output in summary
    std::vector<std::string> runWarningList = {};  // run warnings output in summary
    int Dchannel = 0;  // Detector Channel

    long double NPbin_length = -1.0;  // Set length of normal point in seconds
    int minNPn = -1;  // Set the minimum number of range observations to form a Normal point

    bool loop = true;  // loop in a FRD file
    bool FRDloop = false;  // set FRD file loop

    bool read1st = true; // Flag to the first read

    long double ep1 = 0.0;
    long double ep1m = 0.0;

    bool mjd_daychange = false;
    std::vector<MJDateTime> Mmep = {};


    std::vector<long double> pressure = {};
    std::vector<long double> TK = {};
    std::vector<long double> humid = {};


    std::vector<long double> Depc = {};
    Iso8601Str Ddatet;
    std::vector<MJDateTime> Dmep = {};
    std::vector<long double> Drng = {};
    std::vector<MJDateTime> Cmep = {};
    std::vector<long double> Crng = {};
    std::vector<std::string> Cnum = {};
    std::vector<std::string> Crms = {};
    std::vector<std::string> Cskw = {};
    std::vector<std::string> Ckurt = {};
    std::vector<std::string> Cpm = {};
    long double surveyd = 0.0;
    bool STsel = false;
    bool SDapplied = true;

    std::vector<std::size_t> h2i = {};
    std::vector<std::string> h2l = {};
    std::vector<std::string> h4l = {};
    std::size_t c10 = 0;
    std::vector<std::size_t> Fcount = {};

    std::string line = "";
    std::size_t l = 0;

    std::vector<std::string> words = {};

    std::size_t lnpass = 0;
    int numpass = 0;

    std::vector<std::size_t> selpass = {};

    int ipass = 0;

    std::string rawi = "";

    std::vector<std::string> lineS = {};

    std::string h1l = "";

    std::vector<std::string> a = {};

    std::string CRDversion = "";

    std::string Hsat = "";

    std::string Hyr = "";
    std::string Hmon = "";
    std::string Hd = "";

    long double cep = 0.0;

    Iso8601Str c;
    Iso8601Str datT;

    long double mean_Dmep;

    std::vector<std::string> a;
    std::string delimiter = " ";
    split(a,STAT_coords,delimiter,true);

    long double STAT_LAT = 0.0;
    long double STAT_LONG = 0.0;
    long double STAT_HEI = 0.0;

    long double STAT_LONGrad = 0.0;
    long double STAT_LATrad = 0.0;
    long double STAT_HEI_Mm = 0.0;

    std::vector<long double> PRESSURE = {};
    std::vector<long double> TEMP = {};
    std::vector<long double> HUM = {};

    Mm = {};
    MM = {};

    long double MDmep = 0.0;
    long double mDmep = 0.0;

    long double cpf0 = cpfEP[0];
    std::vector<long double> cpf_ply_X;
    std::vector<long double> cpf_ply_Y;
    std::vector<long double> cpf_ply_Z;

    std::vector<long double> cpfEP = {};
    std::vector<long double> cpfX = {};
    std::vector<long double> cpfY = {};
    std::vector<long double> cpfZ = {};
    long double mep = 0.0;
    long double mep2 = 0.0;
    long double stp = 0.0;

    long double mX = 0.0;
    long double mY = 0.0;
    long double mZ = 0.0;
    long double geoR = 0.0;

    std::size_t neps = 0;
    std::size_t nmet = 0;

    std::vector<long double> gvs(3);
    Matrix<long double> cv;
    std::vector<long double> rhs(nu);
    Matrix<long double> rd(nu,nu,0);
    std::vector<long double> rf(nu);
    std::vector<long double> s(nu);
    std::size_t itr = 0;
    std::size_t itrm = 30;
    long double aln = 0.0;
    long double acr = 0.0;
    long double rdl = 0.0;
    long double acrd = 0.0;
    long double alnc = 0.0;
    long double acrc = 0.0;
    long double radc = 0.0;
    long double acrdd = 0.0;
    long double alndc = 0.0;
    long double acrdc = 0.0;
    long double raddc = 0.0;
    long double alnddc = 0.0;
    long double acrddc = 0.0;
    long double radddc = 0.0;
    long double alnd = 0.0;
    long double rdld = 0.0;
    long double alndd = 0.0;
    long double rdldd = 0.0;
    long double saln = 0.0;
    long double sacr = 0.0;
    long double srdl = 0.0;
    long double salnd = 0.0;
    long double sacrd = 0.0;
    long double srdld = 0.0;
    long double salndd = 0.0;
    long double sacrdd = 0.0;
    long double srdldd = 0.0;
    std::size_t ierr = 0;
    long double sigt = 0.0;
    long double sigr = 0.0;
    long double sigtt = 0.0;
    long double sigrr = 0.0;
    long double oldrms = 0.0;

    long double dn = 0.0;
    long double dnh = 0.0;
    long double dnab = 0.0;
    long double dnabh = 0.0;
    long double STAT_X = 0.0;
    long double STAT_Y = 0.0;
    long double STAT_Z = 0.0;

    std::vector<long double> ql = {};
    std::vector<long double> qm = {};
    std::vector<long double> qn = {};
    std::vector<long double> ddr = {};
    std::vector<long double> dxi = {};
    std::vector<long double> dyi = {};
    std::vector<long double> dzi = {};
    std::vector<long double> dvx = {};
    std::vector<long double> dvy = {};
    std::vector<long double> dvz = {};

    std::vector<long double> rX = {};
    std::vector<long double> rY = {};
    std::vector<long double> rZ = {};
    std::vector<long double> cpfR = {};

    long double mCrng = 0.0;

    std::vector<long double> dvel = {};
    std::vector<long double> zdum = {};
    std::vector<long double> tp() = {};
    long double rej2 = 0.0;
    long double rej3 = 0.0;
    long double rmsa = 0.0;
    bool itr_fin = false;

    long double sw = 0.0;

    long double ssr = 0.0;
    std::size_t nr = 0;

    std::vector<std::size_t> sel = {};

    long double dstn = 0.0;

    std::vector<long double> al = {};
    std::vector<long double> ac = {};
    std::vector<long double> ra = {};
    std::vector<long double> dx = {};
    std::vector<long double> dy = {};
    std::vector<long double> dz = {};
    std::vector<long double> dxt = {};
    std::vector<long double> dyt = {};
    std::vector<long double> zt = {};
    std::vector<long double> dr = {};
    std::vector<long double> drc = {};
    std::vector<long double> czd = {};
    std::vector<long double> altc = {};

    std::vector<long double> drdal = {};
    std::vector<long double> drdac = {};
    std::vector<long double> drdrd = {};

    std::vector<long double> refr = {};
    std::vector<long double> delr = {};

    std::vector<long double> tresid = {};
    std::vector<long double> dresid = {};
    std::vector<long double> dresidAux = {};
    std::vector<long double> aresid = {};

    std::vector<std::size_t> Ssel = {};
    std::vector<std::size_t> Rsel = {};

    long double rmsb;
    long double rms3;

    std::vector<long double> pltresx1 = {};
    std::vector<long double> pltresy1 = {};

    std::size_t np = 0;

    std::vector<long double> pltres = {};

    long double sum = 0.0;

    std::vector<std::size_t> nus = {};

    long double rra = 0.0;
    std::size_t ins = 0;
    long double seuw = 0.0;

    std::vector<long double> presid = {};
    long double aRMS = 0.0;

    int psecbin = 0;

    long double pmin = 0.0;
    long double pmax = 0.0;
    int nbins = 0;
    std::size_t i_aux = 0;
    std::vector<long double> apresid = {};

    std::vector<long double> hbins = {};
    long double hstep = 0.0;

    long double PEAK = 0.0;

    std::vector<std::size_t> Osel = {};
    std::vector<std::size_t> OA = {};
    std::vector<std::size_t> OB = {};

    std::vector<long double> OUTresid = {};
    std::vector<long double> OUTrng = {};
    std::vector<long double> OUTep = {};
    std::vector<Iso8601Str> OUTdt = {};
    std::vector<MJDateTime> OUTmjd = {};
    std::vector<long double> NORMresid = {};
    std::vector<long double> NORMrng = {};
    std::vector<long double> NORMep = {};
    std::vector<Iso8601Str>NORMdt = {};
    std::vector<MJDateTime>NORMmjd = {};
    long double tRMS = 0.0;
    long double fbin = 0.0;
    Iso8601Str fdtbin;
    long double lbin;

    std::vector<long double> b1 = {};
    std::vectro<long double> b2 = {};

    std::vector<Iso8601Str> NPdtbins = {};

    std::vector<long double> Nav = {};
    std::vector<long double> Nstd = {};
    std::vector<long double> Nskw = {};
    std::vector<long double> Nkrt = {};
    std::vector<long double> Npk = {};
    std::vector<long double> Nep = {};
    std::vector<MJDateTime> Nmjd = {};
    std::vector<Iso8601Str> Ndatet = {};
    std::vector<long double> NRng = {};
    std::vector<long double> Nnpts = {};
    std::vector<long double> Ndur = {};

    std::vector<long double> NORMresidSel = {};
    long double m1 = 0.0;
    std::size_t Nc = 0.0;
    std::vector<long double> NORMepSel = {};
    long double mNORMep = 0.0;
    long double maxNep = 0.0;
    long double minNep = 0.0;
    std::vector<long double> Nf = {};

    std::size_t nN = 0;
    std::vector<long double> rR = {};

    int h = 0;
    std::vector<std::string> wList = {};

    unsigned int kd = std::min<unsigned int>(cpfEP.size()-1,16);
    std::vector<long double> cpfEP0;

    long long day_part;
    long double fract_part;

    int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int minute;
    unsigned int second;

    long double jd_total;
    long long jd_day;
    long double jd_fract;

    char buffer[20];

    // Clipper options parameters

    HistCountRes<long double> histr;
    HistCountRes<long double> amp = 0.0;
    std::vector<long double> pbins = {};
    std::size_t winlen = 0;
    std::vector<long double> smth = {};
    std::vector<long double> hprofil = {};
    std::size_t maxi = 0;
    long double amax = 0.0;
    long double bmax = 0.0;
    std::vector<long double> p0(3);
    std::vector<long double> coeffG = {};
    long double gPEAK = 0.0;
    long double gRMS = 0.0;
    long double l1 = 0.0;
    long double l2 = 0.0;
    std::vector<long double> clpp(4);

    long double iRMS = 0.0;
    long double prevRMS = 0.0;
    long double imean = 0.0;
    std::size_t citr = 0;

    std::vector<long double> presid_aux = {};

    std::size_t am = 0;
    std::vector<long double> ami = {};
    std::size_t d = 0;

    int am = 0;
    std::size_t af = 0;
    int ar = 0;

    long double mhprofil = 0;
    std::size_t pp = 0;
    std::size_t atmpt = 0;
    std::vector<long double> coeff = {};
    std::vector<long double> hbinsx = {};
    std::vector<long double> ampy = {};
    long double LEHMlow;
    long double LEHMupp;

    std::size_t PEAKi = 0;
    long double PEAKm = 0.0;
    std::vector<long double> gauss_hist_fit = {};
    std::vector<long double> gauss_hist_fit_aux = {};
    std::vector<std::size_t> l = {};
    long double FEHM = 0.0;
    std::vector<long double> hprofil_aux = {};

    std::vector<long double> ghfl = {};
    std::vector<long double> hbinsl = {};
    std::vector<long double> hpl = {};
    std::vector<long double> pbinsl = {};
    std::vector<long double> Clip(3);

    // Setup

    FRdata = "7824_jason3_crd_20251010_2013_00.fr2";
    FRin = true;
    CPFin = "jason3_cpf_251009_28201.hts";
    NPbin_length = 30;
    PWidth = 30;
    PWadjust = true;
    frate = 10;
    STAT_coords = "36.465258 353.794695 98.2684";
    SNXref = false;
    minNPn = 3;
    plotRES = true;
    STAT_id = 7824;
    NPout = true;
    savepassname = "testwop";


    // Algorithm

    while(loop)
    {
        if(FRDloop == false)
        {
            loop = false;
        }

        ep1 = -1.0;
        ep1m = -1.0;

        mjd_daychange = false;
        Mmep = {};

        pressure = {};
        TK = {};
        humid = {};

        Depc = {};
        Ddatet = "";
        Dmep = {};
        Drng = {};
        Cmep = {};
        Crng = {};
        Cnum = {};
        Crms = {};
        Cskw = {};
        Ckurt = {};
        Cpm = {};
        surveyd = 0.0;
        STsel = false;
        SDapplied = true;

        runWarningList = {};
        Dchannel = -1;

        std::cout<<"\n -- Read FRD file for epochs, ranges and meteorological data... "<<std::endl;
        std::ifstream fid(FRdata);

        if(!fid.is_open())
        {
            std::cerr << "\t Failed to open full-rate data file. "<< std::strerror(errno) << " : " << FRdata << std::endl;

            std::exit();
        }

        if(read1st)
        {
            h2i = {};
            h2l = {};
            h4l = {};
            c10 = 0;
            Fcount = {};

            line = "";
            l = 0;

            while(std::getline(fid,line))
            {
                if (line.find("h2") != std::string::npos || line.find("H2") != std::string::npos)
                {
                    if(c10 > 0)
                    {
                        Fcount.push_back(c10);
                    }
                    c10 = 0;
                    h2i.push_back(l);

                    split(words,line,delimiter,true);
                    h2l.push_back(words[2]);
                }
                if(line.find("h4") != std::string::npos || line.find("H4") != std::string::npos)
                {
                    h4l.push_back(strip(line));
                }
                if(line.length() >= 2 && line.substr(0, 2) == "10")
                {
                    c10++;
                }

                l++;
            }

            Fcount.push_back(c10);
            read1st = false;
        }

        lnpass = 1;
        numpass = 0;
        for(std::size_t i = 0; i < h2l.size(); i++)
        {
            if(h2l[i] == STAT_id)
            {
                numpass++;
            }
        }

        selpass = where(h2l == STAT_id);

        if(numpass == 0)
        {
            std::cerr<<" EXIT: No data for station "<<STAT_id<<" in FRD file"<<std::endl;

            std::exit();
        }

        else if(numpass == 1)
        {
            std::cout<<"\n FRD file contains only one pass for station "<<STAT_id<<std::endl;
            lnpass = h2i[selpass[0]];
            loop = false;
        }

        else
        {
            ipass = -1;

            std::cout << "\t"
                      << std::left << std::setw(10) << "Index"
                      << std::left << std::setw(20) << "Station Name"
                      << std::left << std::setw(15) << "Num Records"
                      << "H4 Start/End Entry" << std::endl;

            for(std::size_t i = 0; i < selpass.size(); i++)
            {
                std::cout << "\t  "
                          << i << " \t  "
                          << std::left  << std::setw(11) << STAT_name << "   "
                          << std::right << std::setw(8)  << Fcount[s] << "           "
                          << h4l[s] << std::endl;
            }

            std::cout << "\n FRD file contains " << numpass
                      << " passes for station " << STAT_id
                      << "\t\t\t(q to quit)" << std::endl;

            while ((ipass < 0) || (ipass >= numpass))
            {
                rawi = "";
                std::cout << "Enter pass number: ";
                std::cin >> rawi;

                try
                {
                    ipass = std::stoi(rawi);
                }
                catch(...)
                {
                    if(rawi == "q")
                    {
                        std::exit();
                    }
                    ipass = -1;
                }
            }
            lnpass = h2i[selpass[ipass]];
        }

        fid.seekg(0, std::ios::beg);

        std::getline(fid, line);

        ss.str(line);
        ss.clear();
        split(lineS,line,delimiter,true);

        if(lineS[0] != "H1" && lineS[0] != "h1")
        {
            std::cerr<<" ERROR: FRD input file read error"<<std::endl;
            std::exit(EXIT_FAILURE);
        }

        fid.seekg(0, std::ios::beg);
        h1l = {};

        for(std::size_t i = 0; i < line.size(); i++)
        {
            split(a,line,delimiter,true);

            if(a[0] == "H1" || a[0] == "h1")
            {
                CRDversion = a[2];
                h1l = line;
            }

            if(a[0]=="H2" || a[0] == "h2")
            {
                if ((STAT_id == a[1]) || (STAT_id == a[2]) && i == lnpass)
                {
                    STsel = true;
                    STAT_abv = a[0];
                }
            }

            if(STsel)
            {
                if(a[0] == "H3" || a[0] == "h3")
                {
                    Hsat = a[1];
                    SATtarget_name = Hsat
                }

                else if(a[0] == "H4" || a[0] == "h4")
                {
                    Hyr = a[2];
                    value = std::stoi(a[3]);
                    std::stringstream ss;
                    ss << std::setw(2) << std::setfill('0') << value;
                    Hmon = ss.str();

                    value = std::stoi(a[4]);
                    ss.clear();
                    ss << std::setw(2) << std::setfill('0') << value;
                    Hd = ss.str();

                    grtojd(std::stoi(a[2]), std::stold(a[3]), std::stold(a[4]), std::stold(a[5]), std::stold(a[6]), std::stod(a[7]), day_part, fract_part);

                    mjd1 = static_cast<long double> day_part + fract_part - 2400000.5;

                    grtojd(std::stoi(a[8]), std::stold(a[9]), std::stold(a[10]), std::stold(a[11]), std::stold(a[12]), std::stod(a[13]), day_part, fract_part);

                    mjd1 = static_cast<long double> day_part + fract_part - 2400000.5;

                    grtojd(std::stoi(a[2]), std::stoi(a[3]), std::stoi(a[4]), 0, 0, 0, day_part, fract_part);

                    INmjd = (static_cast<long double> day_part + fract_part) - 2400000.5;

                    mjdm = INmjd;
                    mjdc = INmjd;

                    jd_total = INmjd + 2400000.5;

                    jd_day = static_cast<long long>(jd_total);
                    jd_fract = jd_total - static_cast<long double>(jd_day);

                    jdtogr(jd_day, jd_fract, year, month, day, hour, minute, second);

                    snprintf(buffer, sizeof(buffer), "%04d-%02u-%02uT%02u:%02u:%02u", year, month, day, hour, minute, second);

                    c = std::string(buffer);

                    if(a[0] == "0")
                    {
                        SDapplied = false;
                        std::cout<<std::endl<<"-- System Delay Calibration not applied.  Will be applied";
                        runWarningList.push_back("System Delay Calibration was not applied to ranges")
                    }
                    else
                    {
                        std::cout<<std::endl<<"-- System Delay Calibration already applied";
                    }
                }

                else if(a[0] == "C0" || a[0] == "c0")
                {
                    if (CsysID == '--')
                    {
                        CsysID = a[3];
                    }
                }

                else if (a[0] == "C1" || a[0] == 'c1')
                {
                    frate = std::stold(a[5]);
                }

                else if(a[0] == "10")
                {
                    if(Unfilter || a[5] != "1")
                    {
                        ep = std::stold(a[1]) / 86400.0;

                        if(ep1 == -1)
                        {
                            ep1 = ep;
                        }

                        if ((ep + 300.0 / 86400.0 < mjd1 - INmjd) || (ep < ep1) && mjd_daychange == False)
                        {
                            std::cout<<std::endl<<"-- Day change detected during pass";
                            INmjd = INmjd + 1.0;
                            mjd_daychange = true;
                        }

                        Dmep.push_back(INmjd + ep);
                        Depc.push_back(std::stold(a[1]));
                        Drng.push_back(std::stold(a[2]));
                        cep = std::stold(a[1]);

                        jd_day = static_cast<long long>(INmjd);
                        jd_fract = INmjd - static_cast<long double>(jd_day);

                        jdtogr(jd_day, jd_fract, year, month, day, hour, minute, second);

                        snprintf(buffer, sizeof(buffer), "%04d-%02u-%02uT%02u:%02u:%02u", year, month, day, hour, minute, second);

                        c = std::string(buffer);

                        datT = c.add(cep);

                        Ddatet.push_back(datT);

                        if(Dchannel == -1)
                        {
                            Dchannel = std::stoi(a[6]);
                        }
                    }
                }

                else if(a[0] == "20")
                {
                    epm = std::stold(a[1])/86400.0;

                    if(ep1m == -1.0 && epm - (mjd1 - std::floor(mjd1)) > 0.5)
                    {
                        std::cout<<"\n -- Met dataset begins on previous day"<<std::endl;
                        runWarningList.push_back("Met dataset begins on previous day");
                        mjdm--;
                    }

                    if(epm < ep1m)
                    {
                        mjdm++;
                    }

                    ep1m = epm;
                    Mmep.push_back(mjdm + epm);
                    pressure.push_back(std::stold(a[2]));
                    TK.push_back(std::stold(a[3]));
                    humid.push_back(std::stold(a[4]));
                }

                else if(a[0] == "40")
                {
                    epc = std::stold(a[1]) / 86400.0;
                    Cmep.push_back(INmjd + epc);
                    Crng.push_back(std::stold[a[7]]);
                    Cnum.push_back(a[4]);
                    Crms.push_back(a[9]);
                    Cskw.push_back(a[10]);
                    Ckurt.push_back(a[11]);
                    Cpm.push_back(a[12]);
                    surveyd = std::stold(a[6]);
                }

                else if(a[0] == "H8" || a[0] == "h8")
                {
                    break;
                }
            }

            fid.close();
        }

        mean_Dmep = mean(Dmep);

        if(mjd2 < mjd1)
        {
            mjd2 = mjd1 + (argmax(cep) - argmin(cep)) / 86400.0
        }

        if(!SDapplied)
        {
            if(Cmep.size() > 1)
            {
                Drng = Drng - linearInterpolation(Cmep, Crng,Dmep,{Crng[0],Crng[Crng.size()-1]});
            }
            else
            {

                Drng = Drng - Crng[0];
            }
        }

        if(Dmep.size() == 0)
        {
            std::cerr << " No Epoch-Range data loaded, quitting... " << STsel << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if(Dchannel == -1)
        {
            Dchannel = 0;
        }

        split(a,STAT_coords,delimiter,true);

        STAT_LAT = std::stold(a[0]);
        STAT_LONG = std::stold(a[1]);
        STAT_HEI = std::stold(a[2]);

        if(STAT_id != "")
        {
            std::cout << "\n\t+ SLR Station is " << STAT_id << " " << STAT_name << std::endl;
        }

        std::cout << "\t+ Station Latitude, Longitude and Height: " << std::fixed
                  << std::setprecision(2) << STAT_LAT << " " << STAT_LONG << " "
                  << std::setprecision(1) << STAT_HEI << "\n";

        STAT_LONGrad = STAT_LONG * 2 * kPi / 360.0;
        STAT_LATrad = STAT_LAT * 2 * kPi / 360.0;
        STAT_HEI_Mm = STAT_HEI * 1e-6;

        std::cout << "\n -- Interpolate meteorological records ... " << std::endl;

        PRESSURE = {};
        TEMP = {};
        HUM = {};

        if(Mmep.size() == 0)
        {
            METap = false;
        }

        else if(Mmep.size() > 1)
        {
            PRESSURE = linearInterpolation(Mmep, pressure, Dmep, {pressure[0], pressure[pressure.size()-1]});
            TEMP = linearInterpolation(Mmep, TK, Dmep, {TK[0], TK[TK.size()-1]});
            HUM = linearInterpolation(Mmep, humid, Dmep, {humid[0],humid[humid.size()-1]});

            Mm.reserve(Mmep.size());
            MM.reserve(Mmep.size());

            MDmep = *max_element(Dmep.begin(),Dmep.end());
            mDmep = *min_element(Dmep.begin(),Dmep.end());


            MM = abs(Mmep - MDmep);
            Mm = abs(Mmep - mDmep);

            iMax = argmin(MM);
            iMin = argmin(Mm);


            if (std::abs(MDmep - Mmep[iMax]) < 0.5 / 24.0 && std::abs(mDmep - Mmep[iMin]) < 1.5 / 24.0)
            {
                METap = true;
            }
        }

        else
        {
            PRESSURE.push_back(pressure[0]);
            TEMP.push_back(TK[0]);
            HUM.push_back(humid[0]);
            METap = true;
        }

        try
        {
            fs::remove("cpf.in");
        }
        catch(const fs::filesystem_error& e)
        {
            std::cerr << "System error: " << e.what() << "\n";
        }

        cpf0 = cpfEP[0];

        if(CPFin == "")
        {
            std::cerr << "ERROR: No CPF file" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        else
        {
            std::ofstream f("cpf.in");
            if (f.is_open())
            {
                f << CPFin;
                f.close();
            }

            std::cout<<"\n -- Read CPF prediction file: "<<CPFin<<std::endl;
            std::ifstream cpf_fid(CPFin);
            cpfEP = {};
            cpfX = {};
            cpfY = {};
            cpfZ = {};
            mep = 0.0;
            mep2 = 0.0;
            stp = 0.0;

            while(std::getline(cpf_fid,line))
            {
                split(a,line,delimiter,true);

                if(a[0] == "10")
                {
                    mep = std::stold(a[2]) + std::stold(a[3])/86400.0;

                    if(stp == 0.0 && mep2 != 0.0)
                    {
                        stp = mep - mep2;
                    }

                    mep2 = mep;

                    if(mep >= (mjd1 - 0.5 / 24.0) - 2.0 * stp && mep <= (mjd2 + 0.5 / 24.0) + 3*stp)
                    {
                        cpfEP.push_back(mep);
                        cpfX.push_back(std::stold(a[5]));
                        cpfY.push_back(std::stold(a[6]));
                        cpfZ.push_back(std::stold(a[7]));
                    }
                }
            }

            cpf_fid.close();

            if(cpfEP.empty())
            {
                std::cerr<<"\n -- Selected CPF file"<<CPFin<<"does not cover the required orbit time period. Quit"<<std::endl;
                std::exit(EXIT_FAILURE);
            }

            kd = std::min<unsigned int>(cpfEP.size()-1,16);

            cpfEP0 = cpfEP - cpf0;

            try
            {
                cpf_ply_X = polynomialFit(cpfEP0, cpfX, kd);
                cpf_ply_Y = polynomialFit(cpfEP0, cpfY, kd);
                cpf_ply_Z = polynomialFit(cpfEP0, cpfZ, kd);
            }
            catch(...)
            {	kd = static_cast<unsigned int>(0.5 * kd) + 1;
                cpf_ply_X = polynomialFit(cpfEP0, cpfX, kd);
                cpf_ply_Y = polynomialFit(cpfEP0, cpfY, kd);
                cpf_ply_Z = polynomialFit(cpfEP0, cpfZ, kd);
            }
        }

        mX = applyPolynomial(cpf_ply_X, mean_Dmep - cpf0);
        mY = applyPolynomial(cpf_ply_Y, mean_Dmep - cpf0);
        mZ = applyPolynomial(cpf_ply_Z, mean_Dmep - cpf0);
        geoR = std::sqrt(mX*mX + mY*mY + mZ*mZ);
        std::cout<<"\n -- Begin orbit adjustment to fit range data"<<std::endl;

        neps = Depc.size();
        nmet = Mmep.size();

        dtobc = 0.0;

        if(Depc[Depc.size()-1] > Depc[0])
        {
            dtobc = (Depc[-1] + Depc[0]) / 2.0
        }

        std::fill(gvs.begin(), gvs.end(), 0);
        std::fill(cv.begin(), cv.end(), 0);
        std::fill(rhs.begin(), rhs.end(), 0);
        rd.fill(0);
        std::fill(rf.begin(), rf.end(), 0);
        std::fill(s.begin(), s.end(), 0);
        itr = 0;
        itrm = 30;
        alnc = 0.0;
        acrc = 0.0;
        radc = 0.0;
        alndc = 0.0;
        acrdc = 0.0;
        raddc = 0.0;
        alnddc = 0.0;
        acrddc = 0.0;
        radddc = 0.0;
        alnd = 0.0;
        rdld = 0.0;
        alndd = 0.0;
        rdldd = 0.0;
        saln = 0.0;
        sacr = 0.0;
        srdl = 0.0;
        salnd = 0.0;
        sacrd = 0.0;
        srdld = 0.0;
        salndd = 0.0;
        sacrdd = 0.0;
        srdldd = 0.0;
        ierr = 0;
        sigt = 0.1 / 8.64e7;
        sigr = 0.01 / 1.0e6;
        sigtt = 0.1 / 8.64e7;
        sigrr = 0.01 / 1.0e6;
        oldrms = 1000.0;

        dn = dae / (std::sqrt(1.0 + ((1.0 - 2.0 * df) / (df * df)) * std::sin(STAT_LATrad) * std::sin(STAT_LATrad)));
        dnh = dn + STAT_HEI_Mm;
        dnab = dn * (1.0 - 1.0 / df) * (1.0 - 1.0 / df);
        dnabh = dnab + STAT_HEI_Mm;
        STAT_X = dnh * std::cos(STAT_LATrad) * std::cos(STAT_LONGrad);
        STAT_Y = dnh * std::cos(STAT_LATrad) * std::sin(STAT_LONGrad);
        STAT_Z = dnabh * std::sin(STAT_LATrad);

        rX = applyPolynomial(cpf_ply_X, Dmep - cpf0);
        rY = applyPolynomial(cpf_ply_Y, Dmep - cpf0);
        rZ = applyPolynomial(cpf_ply_Z, Dmep - cpf0);
        cpfR = std::sqrt((rX - STAT_X * 1e6)*(rX - STAT_X * 1e6) + (rY - STAT_Y * 1e6)*(rY - STAT_Y * 1e6) + (rZ - STAT_Z * 1e6)*(rZ - STAT_Z * 1e6));

        if (!Crng.empty())
        {
            mCrng = mean(Crng);
            cpfR = cpfR + (0.5 * mCrng * 1e-12 * sol);

        }


        dvel = {};
        zdum.resize(neps);
        tp.resize(Depc.size());
        rej2 = 1.e10;
        rej3 = 1.e10;
        rmsa = 0.0;
        itr_fin = false;

        dkt = Dmep + ((cpfR / sol) / 86400.0);
        dkt1 = dkt - (0.5 / 86400.0);
        dkt2 = dkt + (0.5 / 86400.0);
        dxi = applyPolynomial(cpf_ply_X, dkt - cpf0) * 1e-6;
        dyi = applyPolynomial(cpf_ply_Y, dkt - cpf0) * 1e-6;
        dzi = applyPolynomial(cpf_ply_Z, dkt - cpf0) * 1e-6;
        dvx = (applyPolynomial(cpf_ply_X, dkt2 - cpf0) - applyPolynomial(cpf_ply_X,dkt1 - cpf0)) * (1e-6 * 86400.0);
        dvy = (applyPolynomial(cpf_ply_Y, dkt2 - cpf0) - applyPolynomial(cpf_ply_Y,dkt1 - cpf0)) * (1e-6 * 86400.0);
        dvz = (applyPolynomial(cpf_ply_Z, dkt2 - cpf0) - applyPolynomial(cpf_ply_Z,dkt1 - cpf0)) * (1e-6 * 86400.0);
        ddr = sqrt(dxi*dxi + dyi*dyi + dzi*dzi);
        dvel = sqrt(dvx*dvx + dvy*dvy + dvz*dvz);
        rv = ddr * dvel;
        ql = (dyi * dvz - dzi * dvy) / rv;
        qm = (dzi * dvx - dxi * dvz) / rv;
        qn = (dxi * dvy - dyi * dvx) / rv;

        while(itr < itrm)
        {
            itr++;
            sw = swi;

            if(itr <= 4)
            {
                sw = 2.0 * swi;
            }

            ssr = 0.0;
            nr = 0;
            oldrms = rmsa;
            std::fill(rhs.begin(), rhs.end(), 0.0);
            std::fill(cv.begin(), cv.end(), 0.0);
            rd.fill(0);

            tp = (Depc - dtobc) / 60.0;

            if(Depc[Depc.size()-1] < Depc[0])
            {
                sel = where(Depc < Depc[Depc.size()-1]);
                for(std::size_t i = 0; i < sel.size(); i++)
                {
                    tp[sel[i]] = (Depc[sel[i]] - 86400.0) / 60;
                }
            }
            gvs[0] = std::cos(STAT_LATrad) * std::cos(STAT_LONGrad);
            gvs[1] = std::cos(STAT_LATrad) * std::sin(STAT_LONGrad);
            gvs[2] = std::sin(STAT_LATrad);
            dstn = std::sqrt(gvs[0] * gvs[0] + gvs[1] * gvs[1] + gvs[2] * gvs[2]);

            al = alnc + alndc * tp + alnddc * tp * tp;
            ac = acrc + acrdc * tp + acrddc * tp * tp;
            ra = radc + raddc * tp + radddc * tp * tp;
            dx = dxi + dvx * al + ql * ac + (dxi * ra / ddr);
            dy = dyi + dvy * al + qm * ac + (dyi * ra / ddr);
            dz = dzi + dvz * al + qn * ac + (dzi * ra / ddr);
            dxt = dx - STAT_X;
            dyt = dy - STAT_Y;
            dzt = dz - STAT_Z;
            dr = sqrt(dxt * dxt + dyt * dyt + dzt * dzt);
            drc = dr * 2.0;
            czd = (dxt * gvs[0] + dyt * gvs[1] + dzt * gvs[2]) / (dr * dstn);
            altc = asin(czd) * 360.0 / (2.0 * kPi);

            if(itr < itrm)
            {
                drdal = (dvx * dxt + dvy * dyt + dvz * dzt) / dr;
                drdac = (ql * dxt + qm * dyt + qn * dzt) / dr;
                drdrd = (dx * dxt + dy * dyt + dz * dzt) / (dr * ddr);
            }

            cv.fill(tp.size(), nu, 0);

            for(std::size_t i = 0; i < tp.size(); i++)
            {
                cv[i] = {drdal[i], drdac[i], drdrd[i], drdal[i] * tp[i], drdac[i] * tp[i], drdrd[i] * tp[i], drdal[i] * tp[i] * tp[i][i], drdac[i] * tp[i] * tp[i],drdrd[i] * tp[i] * tp[i], zdum[i], zdum[i], zdum[i], zdum[i]};
            }

            cv = cv.transpose();

            for(std::size_t i = 0; i < nu; i++)
            {
                if(i < 9)
                {
                    for(auto& x:cv[i])
                    {
                        x /= sw;
                    }
                }
            }

            if(nmet > 0)
            {
                refr = refMM(PRESSURE, TEMP, HUM, altc, Wavel * 1e-3, STAT_LATrad, STAT_HEI_Mm);

                delr = refr * 1.0e-6;
                drc = drc + delr;
            }

            drc = (1e6 * drc / sol) * 1.0e9;
            tresid = (Drng * 1.0e-3 - drc) / 2.0;
            dresid = (tresid * sol * 1e-6 * 1.0e-9) / sw;
            aresid = abs(dresid);

            Ssel = where(aresid < rej2);
            for(std::size_t i = 0; i < Ssel.size(); i++)
            {
                dresidAux.push_back(dresid[Ssel[i]]);
            }

            rmsb = stddev(dresidAux);

            dresidAux = {};
            Rsel = where(aresid < rej2);
            for(std::size_t i = 0; i < Ssel.size(); i++)
            {
                dresidAux.push_back(dresid[Rsel[i]]);
            }

            rms3 = stddev(dresidAux);

            if(itrm - itr < 2)
            {
                Ssel = Rsel;
            }

            if(itr == 1)
            {
                std::cout<<"\n\t  #      pts         rms2          rms3          rmsa        TBias      Radial"<<std::endl;
                pltresx1 = Depc;
                pltresy1 = tresid;
            }

            rej3 = 3.0 * rms3;
            rej2 = 2.0 * rms3;

            ssr = 0.0;

            dresidAux = {};
            Rsel = where(aresid < rej2);
            for(std::size_t i = 0; i < Ssel.size(); i++)
            {
                dresidAux.push_back(dresid[Ssel[i]]);
            }

            for(std::size_t i = 0; i < dresidAux.size();i++)
            {
                ssr += dresidAux[i];
            }

            np = Ssel.size();

            pltres = tresid * 1.0e3;

            for(std::size_t j = 0; j < nu; j++)
            {
                sum = 0.0;
                for(std::size_t i = 0; i < Ssel.size(); i++)
                {
                    sum += cv[j][Ssel[i]] * dresid[Ssel[i]];
                }

                rhs[j] = sum;

                for(std::size_t k = 0; k < nu; k++)
                {
                    sum = 0.0;
                    for(std::size_t i = 0; i < Ssel.size(); i++)
                    {
                        sum += cv[j][Ssel[i]] + cv[k][Ssel[i]];
                    }

                    rd[k,j] = sum;
                }
            }

            if(itr < itrm)
            {
                rd[3, 3] = rd[3, 3] + (1.0 / sigt) * (1.0 / sigt);
                rd[5, 5] = rd[5, 5] + (1.0 / sigr) * (1.0 / sigr);
                rd[6, 6] = rd[6, 6] + (1.0 / sigtt) * (1.0 / sigtt);
                rd[8, 8] = rd[8, 8] + (1.0 / sigrr) * (1.0 / sigrr);
                rhs[3] = rhs[3] + (1.0 / sigt) * alnd;
                rhs[5] = rhs[5] + (1.0 / sigr) * rdld;
                rhs[6] = rhs[6] + (1.0 / sigtt) * alndd;
                rhs[8] = rhs[8] + (1.0 / sigrr) * rdldd;

                nus = {1, 4, 7, 9, 10, 11, 12};
                for(std::size_t i = 0; i < nus.size(); i++)
                {
                    for(std::size_t k = 0; k < rd.columnsSize(); k++)
                    {
                        rd[nus[i],k] = 0.0;
                    }
                    for(std::size_t k = 0; k < rd.rowSize(); k++)
                    {
                        rd[k,nus[i]] = 0.0;
                    }
                    rd[nus[i], nus[i]] = 1.0;
                    rhs[nus[i]] = 0.0;
                }

                rd = rd.inverse();

                if(rd.isEmpty())
                {
                    std::cerr << "FAILED to invert normal matrix - quit" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                for(std::size_t i = 0; i < nu; i++)
                {
                    rf[i] = 0.0;
                }

                for(std::size_t i = 0; i < nu; i++)
                {
                    for(std::size_t j = 0; j < nu; j++)
                    {
                        rf[i] += rd[i, j] * rhs[j];
                    }
                }

                sum = 0.0;
                for(std::size_t i = 0; i < nu; i++)
                {
                    sum += rf[i]*rhs[i];
                }

                rra = std::max(ssr - sum,0);
                ins = 3;

                rmsa = std::sqrt(rra/nr)*1.0e6;
                seuw = 0.0;

                if(nr + ins > nu)
                {
                    seuw = rra/(1.0 * (nr - nu + ins));
                }

                for(std::size_t i = 0; i < nu; i++)
                {
                    s[i] = 0.0;

                    if(rsh[i] != 0.0 && seuw > 0.0)
                    {
                        s[i] = std::sqrt(rd[i,i]*seuw);
                    }
                }

                if(itr < itrm)
                {
                    aln = rf[0];
                    saln = s[0] * 8.64e7;
                    acr = rf[1];
                    sacr = s[1] * 1.0e6;
                    rdl = rf[2];
                    srdl = s[2] * 1.0e6;
                    alnd = rf[3];
                    acrd = rf[4];
                    rdld = rf[5];
                    salnd = s[3] * 8.64e7;
                    sacrd = s[4] * 1.0e6;
                    srdld = s[5] * 1.0e6;
                    alndd = rf[6];
                    acrdd = rf[7];
                    rdldd = rf[8];
                    salndd = s[6] * 8.64e7;
                    sacrdd = s[7] * 1.0e6;
                    srdldd = s[8] * 1.0e6;
                    alnc = alnc + aln;
                    acrc = acrc + acr;
                    radc = radc + rdl;
                    alndc = alndc + alnd;
                    acrdc = acrdc + acrd;
                    raddc = raddc + rdld;
                    alnddc = alnddc + alndd;
                    acrddc = acrddc + acrdd;
                    radddc = radddc + rdldd;
                }
            }

            std::cout << "\t" << std::setw(3) << itr << " " << std::setw(8)
                      << static_cast<int>(Ssel.size()) << "   " << std::fixed << std::setprecision(3)
                      << std::setw(11) << (1e9 * rmsb * sw) << "   " << std::setw(11)
                      << (1e9 * rms3 * sw) << "   " << std::setw(11) << (1000.0 * rmsa * sw) << "    "
                      << std::setprecision(4) << std::setw(9) << (alnc * 8.64e7) << "  "
                      << std::setw(9) << (radc * 1.0e6) << std::endl;

            if((std::abs(oldrms - rmsa)*sw < 0.00001) && itr > 10)
            {
                if(!itr_fin)
                {
                    itrm = itr + 2;
                    itr_fin = true;
                }
            }
        }

        auto print_row = [](const std::string& label, double v1, double v2) {
            std::cout << "\t" << std::left << std::setw(40) << label
                      << std::right << std::fixed << std::setprecision(4)
                      << std::setw(10) << v1 << "\t"
                      << std::setw(8) << v2 << "\n";};

        std::cout << "\n";
        print_row("Satellite orbital time bias (ms)", alnc * 8.64e7, saln);
        print_row("Satellite radial error (m)", radc * 1.0e6, srdl);
        print_row("Rate of time bias (ms/minute)", alndc * 8.64e7, salnd);
        print_row("Rate of radial error (m/minute)", raddc * 1.0e6, srdld);
        print_row("Acceleration of time bias", alnddc * 8.64e7, salndd);
        print_row("Acceleration of radial error", radddc * 1.0e6, srdldd);

        if(std::abs(alnc * 8.64e7) > 100.0)
        {
            std::ostringstream oss;
            oss << "Large Time Bias required " << std::fixed << std::setprecision(3)
                << std::setw(9) << (alnc * 8.64e7) << " ms";

            std::string warning = oss.str();

            runWarningList.push_back(warning);
            std::cout << "\n" << warning << std::endl;
        }

        else if(std::abs(alnc * 8.64e7) > 10.0)
        {
            std::ostringstream oss;
            oss << "Time Bias required " << std::fixed << std::setprecision(3)
                << std::setw(9) << (alnc * 8.64e7) << " ms";
            runWarningList.push_back(oss.str());
            std::cout << "\n" << oss.str() << std::endl;
        }

        if(std::abs(radc * 1.0e6) > 100.0)
        {
            std::ostringstream oss;
            oss << "Large Radial Offset required " << std::fixed << std::setprecision(3)
                << std::setw(9) << (radc * 1.0e6) << " ms";
            runWarningList.push_back(oss.str());
            std::cout << "\n" << oss.str() << std::endl;
        }

        else if(std::abs(radc * 1.0e6) > 10.0)
        {
            std::ostringstream oss;
            oss << "Radial Offset required " << std::fixed << std::setprecision(3)
                << std::setw(9) << (radc * 1.0e6) << " ms";
            std::string msj = oss.str();
            runWarningList.push_back(msj);
            std::cout << "\n" << msj << std::endl;
        }

        presid = tresid * 1e3;
        aresid = abs(presid);

        aRMS = stddev(presid);

        std::cout << "\n\tFlattened range residuals RMS " << std::fixed << std::setprecision(2)
                  << aRMS << " ps" << std::endl;

        if(presid.size() < 200)
        {
            psecbin = 8;
        }

        else if(200 <= presid.size() <= 1500)
        {
            psecbin = 4;
        }

        else
        {
            psecbin = 2;
        }

        pmin = *min_element(presid.begin(),presid.end());
        pmax = *max_element(presid.begin(),presid.end());
        nbins = static_cast<int> ((pmax - pmin) / psecbin + 1e-15L);

        saresid = Argsort(aresid);
        apresid.clear();
        apresid.reserve(saresid.size());

        for(std::size_t i = 0; i < saresid.size(); i++)
        {
            apresid.push_back(presid[saresid[i]]);
        }

        while(nbins > 10000)
        {
            i_aux = static_cast<std::size_t>(i_aux*0.999);

            if(i_aux == 0 || i_aux >= apresid.size())
            {
                std::exit(EXIT_FAILURE);
            }
            pmin = *min_element(apresid.begin(),apresid.end()-i_aux);
            pmax = *max_element(apresid.begin(),apresid.end()-i_aux);
            nbins = static_cast<int> ((pmax - pmin) / psecbin + 1e-15L);
            std::cout << i <<" "<< nbins << " " << pmax << " " << pmin << " " << psecbin <<std::endl;
        }

        nbins = std::max(nbins,50);

        if(nbins > 5000)
        {
            std::cout<<"Large number of histogram bins required.  Plot Processing slow...    " + std::to_string(nbins)  + " bins."<<std::endl;
            runWarningList.push_back("Large number of histogram bins required.  Plot Processing slow...    " + std::to_string(nbins)  + " bins.");
        }

        hstep = (pmax - pmin) / (nbins - 1);

        hbins = linspaceStep(pmin,pmax,hstep);

        PEAK = 0.0;
        // -------------------------------------------------------------------------------------------------------------------------------------------------
        // Clipper option block
        if(clipGauss)
        {
            std::cout << "\n -- Clipping using gauss" << std::endl;
            histr = histcounts1D(presid, hbins);
            amp = histr;
            pbins = {};
            pbins.reserve(hbins.size()-1);

            std::transform(hbins.begin(),hbins.end(),std::back_inserter(pbins), [](const auto& a){
                return a - 0.5*hstep;});

            winlen = 4;
            std::fill(smth.begin(), smth.end(), 1.0L / static_cast<long double>(winlen));
            hprofil = convolve(amp,smth,"same");
            maxi = argmax(hprofil);
            amax = hprofil[maxi];
            bmax = hbins[maxi];
            aRMS = std::min(aRMS,1000.0);
            p0 = {amax,bmax,aRMS};

            coeffG = curve_fit_Gauss(pbins,amp,p0);
            gPEAK = coeffG[1];
            gRMS = coeffG[2];

            l1 = gPEAK - cfactor*gRMS;
            l2 = gPEAK + cfactor*gRMS;

            clpp = {0, gPEAK, cfactor2, gRMS};
        }
        elif(clipsigma)
        {
            std::cout << "\n -- Clipping at N-sigma" << std::endl;
            iRMS = aRMS;
            prevRMS = 0.0;
            imean = 0.0;
            citr = 0;
            std::cout << "\t    #   Num pts     RMS        Mean" << std::endl;

            while( std::abs(prevRMS - iRMS) > 0.0030)
            {
                citr++;
                Osel = where(abs(presid - imean) < cfactor * iRMS);
                prevRMS = iRMS;

                presid_aux = {};
                presid_aux.reserve(Osel.size());
                for(std::size_t i = 0; i < Osel.size(); i++)
                {
                    presid_aux.push_back(presid[Osel[i]]);
                }

                iRMS = stddev(presid_aux);
                imean = mean(presid_aux);
                std::cout << "\t " << std::setw(4) << citr << std::setw(8) << Osel.size()
                          << std::fixed << std::setprecision(3) << std::setw(8) << iRMS
                          << std::setw(8) << imean << std::endl;
            }

            if(citr < 5)
            {
                std::cout << "Only " << citr << " iterations in clipping at " << cfactor << "-sigma" << std::endl;
                runWarningList.push_back("Only " + std::to_string(citr) + " iterations in clipping at " + std::to_string(cfactor) + "-sigma");
            }

            l1 = imean - cfactor * iRMS;
            l2 = imean + cfactor * iRMS;
            clpp = {0, imean, cfactor2,iRMS};
            PEAK = imean;
        }

        elif(clipLEHM)
        {
            std::cout << "\n -- Clipping at limits from LEHM";
            histr = histcounts1D(presid, hbins);
            amp = histr;
            pbins = {};
            pbins.reserve(hbins.size()-1);

            std::transform(hbins.begin(),hbins.end(),std::back_inserter(pbins), [](const auto& a){
                return a - 0.5*hstep;});

            winlen = 6;
            std::fill(smth.begin(), smth.end(), 1.0L / static_cast<long double>(winlen));
            hprofil = convolve(amp,smth,"same");

            std::vector<std::size_t> ami_indices(hprofil.size());
            std::iota(ami_indices.begin(), ami_indices.end(), 0);
            std::sort(ami_indices.begin(), ami_indices.end(),[&hprofil](auto i, auto j)
                      {return hprofil[i] < hprofil[j];});

            ami.assign(ami_indices.end() - 20, ami_indices.end() - 1);

            while(ami.size() > 3)
            {
                pbins_aux = {};
                pbins.reserve(ami.size());
                for(std::size_t i = 0; i < ami.size(); i++)
                {
                    pbins_aux.push_back(pbins[ami[i]]);
                }

                if(stddev(1.0 * pbins_aux) > 5.0)
                {
                    break;
                }

                d = argmax(ami);
                ami.erase(v.begin() + d);
            }

            am = static_cast<int> mean(1.0 * ami) + 1;
            mhprofil = *max_element(hprofil.begin(),hprofil.end());
            af = where(hprofil > 0.25 * mhprofil)[0];
            ar = std::max(std::min(am - static_cast<int>(50 / psecbin),af),0);

            if(PWadjust)
            {
                ar = am - static_cast<int>(3 * PWidth / psecbin);
            }

            aRMS = std::min(aRMS,1000.0);
            p0 = {mhprofil, 0.0, aRMS};
            pp = argmax(hprofil);
            atmpt = 0;

            while(atmpt < 12)
            {
                atmpt++;

                try
                {
                    ar = std::max(ar,0);
                    hbinsx.assign(hbins.begin() + ar, hbins.begin() + am + 1);
                    ampy.assign(amp.begin() + ar, amp.begin() + am + 1);
                    coeff = curve_fit_Gauss(hbinsx,ampy,p0);

                }
                catch (const std::exception& inst)
                {
                    std::cout << "Gaussian Fit Function - Except: " << inst.what() << std::endl;
                    coeff = {0,0,0};
                }

                if(coeff[0] > 1.05 * hprofil[pp] || coeff[0] < 0.95 * hprofil[pp] && (std::abs(coeff[0] - hprofil[pp]) < 2))
                {
                    am = am + 2;
                    ar = ar + 1;
                    std::cout << "\t- Front Gaussian Fit - Too Tall/Short - Attempt # " << atmpt << "[" << coeff[0] << ", " << coeff[1] << ", " << coeff[2] << "]" << std::endl;
                }

                elif(coeff[2] < 0 || coeff[1] < pmin || coeff[1] > pmax)
                {
                    am = am + 1;
                    ar = ar + 1;
                    std::cout << "\t- Front Gaussian Fit - Error - Attempt # " << atmpt << std::endl;
                }

                elif(PWadjust && coeff[2] > 2.5 * PWidth)
                {
                    am = am - 1;
                    ar = ar - 1;
                    std::cout << "\t- Front Gaussian Fit - Too Wide - Attempt # " << atmpt << std::endl;
                }

                else
                {
                    atmpt = 100;
                }

                ar = std::max(ar,0);
            }

            if(coeff[0] > 1.05 * hprofil[pp] || coeff[0] < 0.95 * hprofil[pp])
            {
                runWarningList.push_back("Front Gaussian Fit - Too Tall/Short");
            }

            if(PWadjust && coeff[2] > 2.5 * PWidth)
            {
                std::cout << "- Front Gaussian Fit RMS above limit for 10ps laser pulse" << std::endl;
                runWarningList.push_back("Front Gaussian Fit RMS above limit for 10ps laser pulse");
            }

            if(coeff[0] == 0)
            {
                std::cout << "- Front Gaussian Fit Fail. " << neps << " Observations" << std::endl;
                runWarningList.push_back("Front Gaussian Fit Fail. " + std::to_string(neps) + " Observations");
                l1 = LEHMlow;
                l2 = LEHMupp;
                Osel = where(aresid != 0);
                OA = linspaceStep(0, Osel.size(), 1);
                OB = OA;
            }

            else
            {
                PEAK = coeff[1];
                PEAKi = argmin(abs(hbins-PEAK));
                PEAKm = coeff[0];
                gauss_hist_fit = gauss(hbins, coeff[0], coeff[1], coeff[2]);
                gauss_hist_fit_aux.assign(gauss_hist_fit.begin(),gauss_hist_fit.begin() + PEAKi);
                l = sortValArg(abs(gauss_hist_fit_aux - 0.5 * PEAKm));

                ghfl = {};
                hbinsl = {};
                ghfl.reserve(l.size());
                hbinsl.reserve(l.size());

                for(std::size_t i = 0; i < 7; i++)
                {
                    ghfl.push_back(gauss_hist_fit[l[i]]);
                    hbinsl.push_back(hbins[l[i]]);
                }

                LEHM = linearInterpolation(ghfl,hbinsl + 0.5*hstep, 0.5 * PEAKm);

                hprofil_aux.assign(hprofil.begin() + am,hprofil.end()-1);
                l = sortValArg(abs(hprofil_aux - 0.5 * PEAKm)) + am;

                hpl = {};
                pbinsl = {};
                hpl.reserve(l.size());
                pbinsl.reserve(l.size());

                for(std::size_t i = 0; i < 7; i++)
                {
                    hpl.push_back(hprofil[l[i]]);
                    pbinsl.push_back(pbins[l[i]]);
                }

                FEHM = linearInterpolation(hpl,pbinsl,0.5 * PEAKm);
                l1 = LEHM + LEHMlow;
                l2 = LEHM + LEHMupp;
                clpp(1, LEHM, LEHMlow, LEHMupp);
            }
        }

        else
        {
            Osel = where(aresid != 0);
            OA = linspaceStep(0,Osel.size(),1);
            OB = OA;
        }

        if(!clpp.empty())
        {
            Clip = clipper(l1, l2, clpp, presid, clipWIDE);
            Osel = Clip[0];
            OA = Clip[1];
            OB = Clip[2];
        }
        // If the clipping option is not needed, this entire block can be
        // removed and replaced by the last else block.
        // -------------------------------------------------------------------------------------------------------------------------------------------------

        OUTresid = {};
        OUTrng = {};
        OUTep = {};
        OUTdt = {};
        OUTmjd = {};

        OUTresid.reserve(Osel.size());
        OUTrng.reserve(Osel.size());
        OUTep.reserve(Osel.size());
        OUTdt.reserve(Osel.size());
        OUTmjd.reserve(Osel.size());

        for(std::size_t i = 0; i < Osel.size(); i++)
        {
            OUTresid.push_back(presid[Osel[i]]);
            OUTrng.push_back(Drng[Osel[i]]);
            OUTep.push_back(Depc[Osel[i]]);
            OUTdt.push_back(Ddatet[Osel[i]]);
            OUTmjd.push_back(Dmep[Osel[i]]);
        }

        NORMresid = {};
        NORMrng = {};
        NORMep = {};
        NORMdt = {};
        NORMmjd = {};

        NORMresid.reserve(OA.size());
        NORMrng.reserve(OA.size());
        NORMep.reserve(OA.size());
        NORMdt.reserve(OA.size());
        NORMmjd.reserve(OA.size());

        for(std::size_t i = 0; i < OA.size(); i++)
        {
            NORMresid.push_back(OUTresid[OA[i]]);
            NORMrng.push_back(OUTrng[OA[i]]);
            NORMep.push_back(OUTep[OA[i]]);
            NORMdt.push_back(OUTdt[OA[i]]);
            NORMmjd.push_back(OUTmjd[OA[i]]);
        }

        tRMS = stddev(NORMresid);
        std::cout << "\n\tClipped range residuals RMS " << std::fixed << std::setprecision(2)
                  << tRMS << " ps" << std::endl;
        fbin = NORMep[0] - (NORMep[0] - NPbin_length*std::floor(NORMep[0]/NPbin_length));
        fdtbin = NORMdt[0].add(-(NORMep[0] - NPbin_length * std::floor(NORMep[0] / NPbin_length)));
        lbin = NORMep[NORMep.size()-1];

        if(lbin > fbin)
        {
            NPbins = linspaceStep(fbin, lbin, NPbin_length);
        }

        else
        {
            b1 = linspaceStep(fbin, 86400.0, NPbin_length);
            b2 = linspaceStep(0.0,lbin, NPbin_length);
            NPbins = {};
            NPbins.reserve(b1.size() + b2.size());
            NPbins.insert(NPbins.end(), b1.begin(), b1.end());
            NPbins.insert(NPbins.end(), b2.begin(), b2.end());
        }

        NPdtbins = {};
        NPdtbins.reserve(NPbins.size());
        for(std::size_t i = 0; i < NPbins.size(); i++)
        {
            NPdtbins.push_back(fdtbin.add(i * NPbin_length));
        }

        Nav = {};
        Nstd = {};
        Nskw = {};
        Nkrt = {};
        Npk = {};
        Nep = {};
        Nmjd = {};
        Ndatet = {};
        NRng = {};
        Nnpts = {};
        Ndur = {};

        Nav.reserve(NPbins.size());
        Nstd.reserve(NPbins.size());
        Nskw.reserve(NPbins.size());
        Nkrt.reserve(NPbins.size());
        Npk.reserve(NPbins.size());
        Nep.reserve(NPbins.size());
        Nmjd.reserve(NPbins.size());
        Ndatet.reserve(NPbins.size());
        NRng.reserve(NPbins.size());
        Nnpts.reserve(NPbins.size());
        Ndur.reserve(NPbins.size());

        std::cout << "\n -- Form Normal Points" << std::endl;

        for(const auto& b : NPbins)
        {
            sel = where(NORMep >= b && NORMep < b+NPbin_length);

            if(sel.size() >= minNPn)
            {
                NORMresidSel = {};
                NORMresidSel.reserve(sel.size());

                for(std::size_t i = 0; i < sel.size(); i++)
                {
                    NORMresidSel.push_back(NORMresid[sel[i]]);
                }

                av = mean(NORMresidSel);
                Nav.push_back(av);
                Nstd.push_back(stddev(NORMresidSel));
                Nskw.push_back(skew(NORMresidSel));
                Nkrt.push_back(kurtosis(NORMresidSel,fisher = true));
                m1 = sig1PEAK(NORMresid[sel]);
                PEAK = m1;

                Npk.push_back(PEAK);

                NORMepSel = {};
                NORMepSel.reserve(sel.size());
                for(std::size_t i = 0; i < sel.size(); i++)
                {
                    NORMepSel.push_back(NORMep[sel[i]]);
                }

                mNORMep = mean(NORMepSel);

                Nc = argmin(NORMepSel - mNORMep);
                Nep.push_back(NORMep[sel[Nc]]);
                NMjd.push_back(NORMmjd[sel[Nc]]);
                Ndatet.push_back(NORMdt[sel[Nc]]);
                NRng.push_back(NORMrng[sel[Nc]]* 1e-12 + 2 * (av - NORMresid[sel[Nc]])*1e-12);
                Nnpts.push_back(sel.size());
                maxNep = *std::max_element(NORMepSel.begin(),NORMepSel.end());
                minNep = *std::min_element(NORMepSel.begin(),NORMepSel.end());

                if(minNep > maxNep)
                {
                    maxNep += 86400.0;
                }

                if(maxNep - minNep > 0)
                {
                    Ndur.push_back(maxNep - minNep);
                }

                else
                {
                    Ndur.push_back(NPbin_length);
                }

                if(sel.size() > 1)
                {
                    Nf = polynomialFit(NORMepSel,NORMresidSel,1);
                    if(std::abs(Nf[0]) > 20.0 / NPbin_length && np.size(sel) > 200)
                    {
                        std::cout << "\t- Slope " << std::fixed << std::setprecision(4) << Nf[0]
                                  << " ps/s for normal point at epoch " << std::fixed << std::setprecision(1)
                                  << NORMep[sel[Nc]] << std::endl;
                    }
                }
            }

            else if(sel.size() > 0)
            {
                std::cout << "\t- Normal point at " << b << " rejected - too few points " << sel.size() << std::endl;
            }

        }

        nN = Nep.size();

        if(frate != 0.0)
        {
            rp = 100.0 * Nnpts/ (Ndur * frate);
        }

        else
        {
            rR.reserve(Nnpts.size());
            std::fill(rR.begin(), rR.end(), 0);
        }

        if(nN == 0)
        {
            std::cout << "\n ** No Normal Points to output" << std::endl;
            setupWarningList.push_back("- No Normal Points were formed from the range data");
        }

        else if(NPout)
        {
            std::ofstream filenp("normalp.dat");

            jd_day = static_cast<long long> std::floor(Nmjd[0]);
            jd_fract = std::floor(Nmjd[0]) - static_cast<long double>(jd_day);

            jdtogr(jd_day, jd_fract, year, month, day, hour, minute, second);

            snprintf(buffer, sizeof(buffer), "%04d-%02u-%02uT%02u:%02u:%02u", year, month, day, hour, minute, second);

            c = std::string(buffer);

            h = static_cast<int>(Nep[0] / 3600.0 + + 1e-15L);

            for(std::size_t i = 0; i < Nep.size(); i++)
            {
                std::cout << "\t11 " << std::fixed << std::setprecision(12) << Nep[i] << " "
                          << NRng[i] << " " << CsysID << " 2 " << std::setprecision(1) << NPbin_length << " "
                          << Nnpts[i] << " " << std::setprecision(1) << (2.0 * Nstd[i]) << " "
                          << std::setprecision(3) << Nskw[i] << " "
                          << Nkrt[i] << " " << std::setprecision(1) << (2.0 * (Npk[i] - Nav[i])) << " "
                          << rR[i] << " " << Dchannel << " 0.0" << std::endl;
            }
        }

        std::cout << "\n -- Summary: " << std::endl;
        WList = join({setupWarningList,runWarningList});

        if(wList.empty())
        {
            std::cout << "\t Completed OK" << std::endl;
        }

        else
        {
            for(const auto& lineW : wList)
            {
                std::cout << "\tWarning: " << lineW << std::endl;
            }

            if(loop)
            {
                rawi = "";
                std::cout << "\n ** Warnings in process, hit Enter to continue\n                            (q to quit)\n  " << std::endl;
                std::cin >> rawi;

                if(rawi == "q")
                {
                    std::exit();
                }
            }
        }

        print("\n");
    }
}

