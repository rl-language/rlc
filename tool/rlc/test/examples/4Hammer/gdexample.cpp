#include "stdint.h"
#include <gdextension_interface.h>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/godot.hpp>

#define RLC_GET_TYPE_DEFS
#define RLC_GODOT
#include "rules.inc"

using namespace godot;

#if 0
#include <execinfo.h>
static void printStackTrace() {
  void *buffer[64];
  int nptrs = backtrace(buffer, 64);
  char **symbols = backtrace_symbols(buffer, nptrs);
  for (int i = 0; i < nptrs; i++) {
    std::fprintf(stderr, "%s\n", symbols[i]);
  }
  free(symbols);
}
#else

static void printStackTrace() {}
#endif

extern "C" {
void rlc_abort(char *message) {
  printStackTrace();
  ERR_FAIL_MSG(message);
}
}

static godot::Variant rlc_string_to_godot_string(godot::Variant s) {
  if (not s) {
    return nullptr;
  }
  auto casted =
      godot::Object::cast_to<RLCString>(*((godot::Ref<RLCString>)(s)));
  int64_t index = 0;
  return godot::String((char *)casted->content->get(index));
}

static godot::Variant godot_string_to_rlc_string(godot::String inputS) {
  CharString char_str = inputS.utf8();
  char *data = const_cast<char *>(char_str.get_data());

  ::String *mallocated = (::String *)malloc(sizeof(::String));
  godot::Ref<RLCString> to_return;
  to_return.instantiate();
  godot::Object::cast_to<RLCString>(*to_return)->setNonOwning(mallocated);
  rl_s__strlit_r_String(mallocated, &data);
  return to_return;
}

void initialize_example_module(ModuleInitializationLevel p_level) {

  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  godot_nativescript_init();

  godot::ClassDB::bind_static_method(
      "RLCLib", godot::D_METHOD("convert_string"), &rlc_string_to_godot_string);
  godot::ClassDB::bind_static_method(
      "RLCLib", godot::D_METHOD("godot_string_to_rlc_string"),
      &godot_string_to_rlc_string);
}

void uninitialize_example_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  godot_gdnative_terminate();
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT
rules_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                   const GDExtensionClassLibraryPtr p_library,
                   GDExtensionInitialization *r_initialization) {

  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library,
                                                 r_initialization);

  init_obj.register_initializer(initialize_example_module);
  init_obj.register_terminator(uninitialize_example_module);
  init_obj.set_minimum_library_initialization_level(
      MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}
