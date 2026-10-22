#include <limits>
#include <stdexcept>

#include <gtest/gtest.h>

#include "constants.h"
#include "kinematics_utils.h"

inline constexpr double TOLERANCE = 1e-7;

TEST(AtomicMassAmuToMassMeVTest, GetsProtonMassRight) {
  constexpr double amu = 1.0078250319;
  constexpr unsigned int z_num = 1;

  EXPECT_NEAR(AnalysisCore::atomic_mass_amu_to_mass_MeV(amu, z_num),
              AnalysisCore::PROTON_MASS_MEV,
              TOLERANCE * AnalysisCore::PROTON_MASS_MEV);
}

TEST(AtomicMassAmuToMassMeVTest, GetsNeutronMassRight) {
  constexpr double amu = 1.0086649159;
  EXPECT_NEAR(AnalysisCore::atomic_mass_amu_to_mass_MeV(amu, 0),
              AnalysisCore::NEUTRON_MASS_MEV,
              TOLERANCE * AnalysisCore::NEUTRON_MASS_MEV);
}

TEST(AtomicMassAmuToMassMeVTest, GetsCarbonTwelweMassRight) {
  constexpr double amu = 12.0;
  constexpr unsigned int z_num = 6;
  const double expected = 11174.86332822;
  EXPECT_NEAR(AnalysisCore::atomic_mass_amu_to_mass_MeV(amu, z_num), expected,
              TOLERANCE * expected);
}

TEST(AtomicMassAmuToMassMeVTest, RejectsNonPositiveMass) {
  EXPECT_THROW(
      static_cast<void>(AnalysisCore::atomic_mass_amu_to_mass_MeV(0.0, 0)),
      std::domain_error);
  EXPECT_THROW(
      static_cast<void>(AnalysisCore::atomic_mass_amu_to_mass_MeV(-1.0, 0)),
      std::domain_error);
}

TEST(AtomicMassAmuToMassMeVTest, RejectsNonFiniteMass) {
  EXPECT_THROW(static_cast<void>(AnalysisCore::atomic_mass_amu_to_mass_MeV(
                   std::numeric_limits<double>::quiet_NaN(), 0)),
               std::domain_error);
  EXPECT_THROW(static_cast<void>(AnalysisCore::atomic_mass_amu_to_mass_MeV(
                   std::numeric_limits<double>::infinity(), 0)),
               std::domain_error);
  EXPECT_THROW(static_cast<void>(AnalysisCore::atomic_mass_amu_to_mass_MeV(
                   -std::numeric_limits<double>::infinity(), 0)),
               std::domain_error);
}

TEST(AtomicMassAmuToMassMeVTest, RejectsMassSmallerThanChargeNumber) {
  EXPECT_THROW(
      static_cast<void>(AnalysisCore::atomic_mass_amu_to_mass_MeV(1.0, 2)),
      std::domain_error);
}
