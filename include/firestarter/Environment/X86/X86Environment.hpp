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

#include <asmjit/asmjit.h>

#include "../Environment.hpp"
#include "Platform/BulldozerConfig.hpp"
#include "Platform/HaswellConfig.hpp"
#include "Platform/HaswellEPConfig.hpp"
#include "Platform/KnightsLandingConfig.hpp"
#include "Platform/NaplesConfig.hpp"
#include "Platform/NehalemConfig.hpp"
#include "Platform/NehalemEPConfig.hpp"
#include "Platform/RomeConfig.hpp"
#include "Platform/SandyBridgeConfig.hpp"
#include "Platform/SandyBridgeEPConfig.hpp"
#include "Platform/SkylakeConfig.hpp"
#include "Platform/SkylakeSPConfig.hpp"
#include "Platform/X86PlatformConfig.hpp"
#include "X86CPUTopology.hpp"

#define REGISTER(NAME)                                                                                                 \
  [](asmjit::CpuFeatures const& supportedFeatures, unsigned family, unsigned model,                                    \
     unsigned threads) -> std::unique_ptr<platform::X86PlatformConfig> {                                               \
    return std::make_unique<platform::NAME>(supportedFeatures, family, model, threads);                                \
  }

namespace firestarter::environment::x86 {

class X86Environment final : public Environment {
public:
  X86Environment()
      : Environment(std::make_unique<X86CPUTopology>()) {}

  auto topology() -> X86CPUTopology const& { return *dynamic_cast<X86CPUTopology*>(Topology.get()); }

  void evaluateFunctions() override;
  void selectFunction(unsigned FunctionId, bool AllowUnavailablePayload) override;
  void selectInstructionGroups(std::string Groups) override;
  void printAvailableInstructionGroups() override;
  void setLineCount(unsigned LineCount) override;
  void printSelectedCodePathSummary() override;
  void printFunctionSummary() override;

private:
  // The available function IDs are generated by iterating through this list
  // of PlatformConfig. Add new PlatformConfig at the bottom to maintain
  // stable IDs.
  const std::list<std::function<std::unique_ptr<platform::X86PlatformConfig>(asmjit::CpuFeatures const&, unsigned,
                                                                             unsigned, unsigned)>>
      PlatformConfigsCtor = {REGISTER(KnightsLandingConfig), REGISTER(SkylakeConfig),   REGISTER(SkylakeSPConfig),
                             REGISTER(HaswellConfig),        REGISTER(HaswellEPConfig), REGISTER(SandyBridgeConfig),
                             REGISTER(SandyBridgeEPConfig),  REGISTER(NehalemConfig),   REGISTER(NehalemEPConfig),
                             REGISTER(BulldozerConfig),      REGISTER(NaplesConfig),    REGISTER(RomeConfig)};

  std::list<std::unique_ptr<platform::X86PlatformConfig>> PlatformConfigs;

  // List of fallback PlatformConfig. Add one for each x86 extension.
  const std::list<std::function<std::unique_ptr<platform::X86PlatformConfig>(asmjit::CpuFeatures const&, unsigned,
                                                                             unsigned, unsigned)>>
      FallbackPlatformConfigsCtor = {
          REGISTER(SkylakeSPConfig),   // AVX512
          REGISTER(BulldozerConfig),   // FMA4
          REGISTER(HaswellConfig),     // FMA
          REGISTER(SandyBridgeConfig), // AVX
          REGISTER(NehalemConfig)      // SSE2
      };

  std::list<std::unique_ptr<platform::X86PlatformConfig>> FallbackPlatformConfigs;

#undef REGISTER
};

} // namespace firestarter::environment::x86
