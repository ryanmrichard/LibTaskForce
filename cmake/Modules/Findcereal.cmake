
#########################
# Finds cereal
#########################

find_path(CEREAL_INCLUDE_DIR NAMES cereal/cereal.hpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cereal DEFAULT_MSG CEREAL_INCLUDE_DIR)
