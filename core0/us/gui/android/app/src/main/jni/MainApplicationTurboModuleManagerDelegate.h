//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the GPLv3 License as published by the Free
//===-    Software Foundation.
//===-
//===-    This program is distributed in the hope that it will be useful,
//===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
//===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//===-
//===-    You should have received a copy of the General Public License
//===-    along with this program, see LICENCE file.
//===-    see https://www.gnu.org/licenses
//===-
//===----------------------------------------------------------------------------
//===-
#include <memory>
#include <string>

#include <ReactCommon/TurboModuleManagerDelegate.h>
#include <fbjni/fbjni.h>

namespace facebook {
namespace react {

class MainApplicationTurboModuleManagerDelegate
    : public jni::HybridClass<
          MainApplicationTurboModuleManagerDelegate,
          TurboModuleManagerDelegate> {
 public:
  // Adapt it to the package you used for your Java class.
  static constexpr auto kJavaDescriptor =
      "Lcom/gui/newarchitecture/modules/MainApplicationTurboModuleManagerDelegate;";

  static jni::local_ref<jhybriddata> initHybrid(jni::alias_ref<jhybridobject>);

  static void registerNatives();

  std::shared_ptr<TurboModule> getTurboModule(
      const std::string name,
      const std::shared_ptr<CallInvoker> jsInvoker) override;
  std::shared_ptr<TurboModule> getTurboModule(
      const std::string name,
      const JavaTurboModule::InitParams &params) override;

  /**
   * Test-only method. Allows user to verify whether a TurboModule can be
   * created by instances of this class.
   */
  bool canCreateTurboModule(std::string name);
};

} // namespace react
} // namespace facebook
