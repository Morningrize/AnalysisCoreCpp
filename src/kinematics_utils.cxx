#include "kinematics_utils.h"

#include <cmath>
#include <stdexcept>

#include "constants.h"

namespace AnalysisCore {

double atomic_mass_amu_to_mass_MeV(double amu, unsigned int z_num){
  if (!std::isfinite(amu) || amu <= 0.0) {
    throw std::domain_error("atomic_mass_amu_to_mass_MeV: atomic mass must be finite and positive");
  }
  if (z_num < 0) {
    throw std::invalid_argument("atomic_mass_amu_to_mass_MeV: charge number must be non-negative");
  }
  if (amu < z_num) {
    throw std::domain_error("atomic_mass_amu_to_mass_MeV: atomic mass must be larger than the charge number");
  }

    return (amu * AMU_MEV) - 
    (z_num * ELECTRON_MASS_MEV);
}
}