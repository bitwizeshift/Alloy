#if defined(WIN32) || defined(_WIN32)
# define ALLOY_EXTRA_API_EXPORT __declspec(dllexport)
#else
# define ALLOY_EXTRA_API_EXPORT __attribute__ ((__visibility__("default")))
#endif

// Note: The 'extern' declaration and value definition have been separated into
//       two separate statements in order to avoid errors when using -Wall due
//       to using 'extern' in a definition.

/// \brief A global variable indicating to devices with both embedded and
///        discrete Nvidia GPUs to prefer to use the discrete one.
///
/// \see https://docs.nvidia.com/gameworks/content/technologies/desktop/optimus.htm
///
/// \note 'unsigned long' is used to avoid importing windows.h for a DWORD
///       which would not be cross-platform.
extern "C" ALLOY_EXTRA_API_EXPORT unsigned long NvOptimusEnablement;
unsigned long NvOptimusEnablement = 0x00000001;

/// \brief A global variable indicating to devices with both embedded and
///        discrete AMD GPUs to prefer to use the discrete one.
///
/// \see https://community.amd.com/thread/169965
/// \see https://gpuopen.com/amdpowerxpressrequesthighperformance/
///
/// \note The first reference lists 'DWORD' as the required underlying type, whereas
///       AMD lists 'int' as the underlying type. 'unsigned long' is used to avoid
///       an import of 'windows.h' for a DWORD, which would not be cross-platform.
extern "C" ALLOY_EXTRA_API_EXPORT unsigned long AmdPowerXpressRequestHighPerformance;
unsigned long AmdPowerXpressRequestHighPerformance = 1;