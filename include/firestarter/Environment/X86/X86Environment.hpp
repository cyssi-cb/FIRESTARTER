/******************************************************************************
 * FIRESTARTER - A Processor Stress Test Utility
 * Copyright (C) 2020 TU Dresden, Center for Information Services and High
 * Performance Computing
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/\>.
 *
 * Contact: daniel.hackenberg@tu-dresden.de
 *****************************************************************************/

#pragma once

#include <firestarter/Environment/Environment.hpp>
#include <firestarter/Environment/X86/X86CPUTopology.hpp>

#include <firestarter/Environment/X86/Platform/SapphireRapidsConfig.hpp>
#include <firestarter/Environment/X86/Platform/BulldozerConfig.hpp>
#include <firestarter/Environment/X86/Platform/HaswellConfig.hpp>
#include <firestarter/Environment/X86/Platform/HaswellEPConfig.hpp>
#include <firestarter/Environment/X86/Platform/KnightsLandingConfig.hpp>
#include <firestarter/Environment/X86/Platform/NaplesConfig.hpp>
#include <firestarter/Environment/X86/Platform/NehalemConfig.hpp>
#include <firestarter/Environment/X86/Platform/NehalemEPConfig.hpp>
#include <firestarter/Environment/X86/Platform/RomeConfig.hpp>
#include <firestarter/Environment/X86/Platform/SandyBridgeConfig.hpp>
#include <firestarter/Environment/X86/Platform/SandyBridgeEPConfig.hpp>
#include <firestarter/Environment/X86/Platform/SkylakeConfig.hpp>
#include <firestarter/Environment/X86/Platform/SkylakeSPConfig.hpp>
#include <firestarter/Environment/X86/Platform/X86PlatformConfig.hpp>

#include <asmjit/asmjit.h>

#include <functional>

#define REGISTER(NAME)                                                         \
  [](asmjit::CpuFeatures const &supportedFeatures, unsigned family,          \
     unsigned model, unsigned threads) -> platform::X86PlatformConfig * {      \
    return new platform::NAME(supportedFeatures, family, model, threads);      \
  }

namespace firestarter::environment::x86 {

class X86Environment final : public Environment {
public:
  X86Environment() : Environment(new X86CPUTopology()) {}

  ~X86Environment() {
    for (auto const &config : platformConfigs) {
      delete config;
    }
    for (auto const &config : fallbackPlatformConfigs) {
      delete config;
    }
  }

  X86CPUTopology const &topology() {
    return *reinterpret_cast<X86CPUTopology *>(this->_topology);
  }

  void evaluateFunctions() override;
  int selectFunction(unsigned functionId,
                     bool allowUnavailablePayload) override;
  int selectInstructionGroups(std::string groups) override;
  void printAvailableInstructionGroups() override;
  void setLineCount(unsigned lineCount) override;
  void printSelectedCodePathSummary() override;
  void printFunctionSummary() override;

private:
  // The available function IDs are generated by iterating through this list of
  // PlatformConfig. Add new PlatformConfig at the bottom to maintain stable
  // IDs.
  const std::list<std::function<platform::X86PlatformConfig *(
      asmjit::CpuFeatures const &, unsigned, unsigned, unsigned)>>
      platformConfigsCtor = {
          REGISTER(KnightsLandingConfig), REGISTER(SkylakeConfig),
          REGISTER(SkylakeSPConfig),      REGISTER(HaswellConfig),
          REGISTER(HaswellEPConfig),      REGISTER(SandyBridgeConfig),
          REGISTER(SandyBridgeEPConfig),  REGISTER(NehalemConfig),
          REGISTER(NehalemEPConfig),      REGISTER(BulldozerConfig),
          REGISTER(NaplesConfig),         REGISTER(RomeConfig)};

  std::list<platform::X86PlatformConfig *> platformConfigs;

  // List of fallback PlatformConfig. Add one for each x86 extension.
  const std::list<std::function<platform::X86PlatformConfig *(
      asmjit::CpuFeatures const &, unsigned, unsigned, unsigned)>>
      fallbackPlatformConfigsCtor = {
          REGISTER(SkylakeSPConfig),   // AVX512
          REGISTER(BulldozerConfig),   // FMA4
          REGISTER(HaswellConfig),     // FMA
          REGISTER(SandyBridgeConfig), // AVX
          REGISTER(NehalemConfig)      // SSE2
      };

  std::list<platform::X86PlatformConfig *> fallbackPlatformConfigs;

#undef REGISTER
};

} // namespace firestarter::environment::x86
