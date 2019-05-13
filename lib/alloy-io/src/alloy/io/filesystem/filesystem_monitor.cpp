#include "alloy/io/filesystem/filesystem_monitor.hpp"

//==============================================================================
// class : filesystem_monitor
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

void alloy::io::filesystem_monitor::watch( std::string_view path,
                                           bool recursive )
  noexcept
{
  do_watch(path, recursive);
}