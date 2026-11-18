////////////////////////////////////////////////////////////////////////
/// \file    SRBeamBranch.h
/// \brief   Beam-related information
/// \author  J. Wolcott <jwolcott@fnal.gov>  [but mostly copied from NOvA/SBN sources]
///          NuMI beam-quality fields: G. Ingratta (York U.), J. L. Barrow (UMN)

#ifndef DUNEANAOBJ_SRBEAMBRANCH_H
#define DUNEANAOBJ_SRBEAMBRANCH_H

#include <cmath>
#include <limits>
#include <vector>

namespace caf
{
  class SRBeamBranch
  {
    private:
      // save on typing below
      constexpr static float NaN = std::numeric_limits<float>::signaling_NaN();

    public:
      /// @name beam variables
      /// @brief Quantities used to characterize the spill-by-spill beam quality.
      ///
      /// For data these come from the IFBeam database's archive of the NuMI beamline
      /// devices. The archive files one record per NuMI spill under the TCLK $A9 event
      /// (there is no separate record for the MIBS $74 "proton extraction into NuMI"
      /// beam-sync event that NOvA triggers on). The ND_CAFMaker matches the trigger(s)
      /// of each CAF entry to the $A9 record within its BeamMatchDeltaT window.
      ///
      /// - **POT (Protons on Target):** Measured by the toroids E:TRTGTD, E:TOR101, and E:TR101D.
      ///   All POT values are per pulse and include the 1e12 normalization of the IFBeam
      ///   "E12" units, so users do not need to apply any additional scaling.
      ///
      /// - **Horn Current:** Determined as a linear combination of four strip-line measurements
      ///   (NSLINA, NSLINB, NSLINC, and NSLIND), with the calibration of NOvA's
      ///   IFDBSpillInfo module ("Updated numbers from Jim Hylen, email Oct 4, 2013"):
      ///   <https://github.com/novaexperiment/novasoft/blob/main/IFDBSpillInfo/IFDBSpillInfo_module.cc>.
      ///   The strip-line readings are negative in BOTH horn polarities, so #hornI is
      ///   about -200 kA in forward (neutrino-mode) and reverse (antineutrino-mode)
      ///   running alike. The polarity is in #hornDir instead (see below).
      ///
      /// - **Beam Position (Pre-target):** Measured by the monitors "121" and "TGT", which provide
      ///   the horizontal and vertical position of the beam, as well as its intensity, in the
      ///   pre-target region for each of the six batches.
      ///
      /// - **Beam Position (On target):** NOvA's intensity-weighted average of the per-batch
      ///   121 and TGT readings, each linearly extrapolated to the target z
      ///   (IFDBSpillInfo::BpmProjection/BpmAtTarget): #beamHpos, #beamVpos, and per batch
      ///   #beamHposBatch, #beamVposBatch.
      ///
      /// - **Beam Width (TGT location):** the MTGTDS device contains the readout voltages of
      ///   horizontal and vertical wires (216 values: 48 horizontal at indices 103-150 and
      ///   48 vertical at 151-198, plus header/housekeeping words).
      ///   The horizontal and vertical spread of the beam is obtained from a gaussian fit of the voltages
      ///   as function of the wire position (IFDBSpillInfo::ProfileProjection/GetGaussFit).
      ///
      /// - **Beam quality:** #isgoodpulse is NOvA's per-spill "goodbeam" selection: every
      ///   pass* flag below must be true. The cut values are configurable in the ND_CAFMaker
      ///   (NuMIBeamQuality FHiCL table) and default to NOvA's values for its periods 13+,
      ///   which cover all 2x2 running. For simulation and for non-beam triggers no beam
      ///   cuts apply, and all of them are left true (as NOvA does for MC and cosmic triggers).
      ///
      /// - **Timing:** #pulsetimesec/#pulsetimensec is the $A9 record time (NOvA's
      ///   "spilltime"); #gpspulsetimesec/#gpspulsetimensec is the detector trigger time that
      ///   was matched to it (NOvA's convention since 2025: the DAQ event time); and
      ///   #deltapulsetimensec is their difference, trigger minus $A9. The IFBeam clock has
      ///   millisecond resolution, so a stable offset in #deltapulsetimensec identifies which
      ///   accelerator timing event drives the detector's beam trigger.
      ///@{
      bool                 ismc;      ///< data or simulated beam pulse?

      bool                 isgoodpulse         = true;      ///< NOvA "goodbeam": all pass* flags below are true (only applicable to data beam triggers)
      unsigned long int    pulsetimesec        = 0;         ///< pulse time from the IFBeam $A9 record (E:TRTGTD record, else E:TR101D, else E:TOR101), seconds part [s]
      unsigned long int    pulsetimensec       = 0;         ///< pulse time from the IFBeam $A9 record, nanoseconds part (millisecond resolution) [ns]
      unsigned long int    gpspulsetimesec     = 0;         ///< detector trigger time matched to this pulse, seconds part [s]
      unsigned long int    gpspulsetimensec    = 0;         ///< detector trigger time matched to this pulse, nanoseconds part [ns]
      signed long long int deltapulsetimensec  = -9999999;  ///< detector trigger time minus IFBeam $A9 pulse time [ns]; default -9999999 = no pulse matched
      float                pulsepot            = NaN;       ///< POT counted for this pulse: E:TRTGTD if above ND_CAFMaker's MinSpillPOT, else E:TOR101 if above it, else 0
      float                potTOR101           = NaN;       ///< POT as measured by the toroid E:TOR101 device (direct readback)
      float                potTR101D           = NaN;       ///< POT as measured by the toroid E:TR101D device (datalogged readback of TOR101)
      float                hornI               = NaN;       ///< Horn current [kA]: calibrated strip-line sum, ~ -200 kA in BOTH polarities
      float                hornDir             = NaN;       ///< Horn polarity readback E:HRNDIR [V]: > 0 in reverse (antineutrino-mode) running, < 0 in forward (neutrino-mode)
      float                beamHwidth          = NaN;       ///< Beam horizontal width at the target [mm]: Gaussian sigma of the E:MTGTDS horizontal wires
      float                beamVwidth          = NaN;       ///< Beam vertical width at the target [mm]: Gaussian sigma of the E:MTGTDS vertical wires
      std::vector<double>  horizontalposTGT;                ///< horizontal beam position as measured by E:HPTGT device (7 values = 1 auto-tune mean value + 6 batches) [mm]
      std::vector<double>  horizontalintTGT;                ///< horizontal beam intensity as measured by E:HITGT device (7 values = 1 auto-tune mean value + 6 batches) [a.u.]
      std::vector<double>  horizontalpos121;                ///< horizontal beam position as measured by E:HP121 device (7 values = 1 auto-tune mean value + 6 batches) [mm]
      std::vector<double>  verticalposTGT;                  ///< vertical beam position as measured by E:VPTGT device (7 values = 1 auto-tune mean value + 6 batches) [mm]
      std::vector<double>  verticalintTGT;                  ///< vertical beam intensity as measured by E:VITGT device (7 values = 1 auto-tune mean value + 6 batches) [a.u.]
      std::vector<double>  verticalpos121;                  ///< vertical beam position as measured by E:VP121 device (7 values = 1 auto-tune mean value + 6 batches) [mm]
      std::vector<double>  multiwireInfo;                   ///< horizontal and vertical readout voltages measured by E:MTGTDS device (216 values) [V]

      // --- NOvA-style derived beam-quality quantities (ND_CAFMaker NuMIBeamQuality) ---
      float                potTRTGTD           = NaN;       ///< POT as measured by the target toroid E:TRTGTD device alone (no fallback)
      float                beamHpos            = NaN;       ///< horizontal beam position at the target [mm]: intensity-weighted, 121/TGT extrapolated (NOvA posx)
      float                beamVpos            = NaN;       ///< vertical beam position at the target [mm]: as NOvA posy, including NOvA's weighting by the horizontal intensity
      std::vector<float>   beamHposBatch;                   ///< per-batch horizontal beam position at the target [mm] (NOvA bposx; 0 for a batch with zero BPM intensity)
      std::vector<float>   beamVposBatch;                   ///< per-batch vertical beam position at the target [mm] (NOvA bposy; 0 for a batch with zero BPM intensity)
      std::vector<float>   beamHintBatch;                   ///< per-batch horizontal BPM intensity scaled to POT; sums to the NOvA spill POT (NOvA intx, x1e12)
      std::vector<float>   beamVintBatch;                   ///< per-batch vertical BPM intensity scaled to POT; sums to the NOvA spill POT (NOvA inty, x1e12)

      bool                 passPOT             = true;      ///< NOvA spill POT (E:TRTGTD, E:TR101D fallback) above the minimum (default 2e12)
      bool                 passHornI           = true;      ///< #hornI inside the window (default -202 < hornI < -196.4 kA, both polarities)
      bool                 passHpos            = true;      ///< #beamHpos inside the window (default -2.0 < x < +2.0 mm)
      bool                 passVpos            = true;      ///< #beamVpos inside the window (default -3.5 < y < +0.5 mm, NOvA periods 13+)
      bool                 passHwidth          = true;      ///< #beamHwidth inside the window (default 0.57 < width < 1.88 mm)
      bool                 passVwidth          = true;      ///< #beamVwidth inside the window (default 0.57 < width < 1.88 mm, NOvA periods 13+)
      bool                 passDeltaT          = true;      ///< |#deltapulsetimensec| below the maximum (default 0.5 s); false if no pulse was matched

      /// Horn-off (zero horn current) pulse: |#hornI| < 1 kA, NOvA's is0HC definition.
      bool is0HC() const  { return std::abs(hornI) < 1; };
      /// Forward horn current (neutrino mode): horn on and #hornDir negative.
      /// (The sign of #hornI must NOT be used: it is negative in both polarities.)
      bool isFHC() const  { return !is0HC() && hornDir < 0; };
      bool is_neutrino_mode() const  { return isFHC(); };   ///< Same as #isFHC
      /// Reverse horn current (antineutrino mode): horn on and #hornDir positive
      /// (NOvA's original E:HRNDIR convention, isRHC = horndir > 0).
      bool isRHC() const  { return !is0HC() && hornDir > 0; }
      bool is_antineutrino_mode() const  { return isRHC(); } ///< Same as #isRHC
      ///@}

  };
}

#endif //DUNEANAOBJ_SRBEAMBRANCH_H
