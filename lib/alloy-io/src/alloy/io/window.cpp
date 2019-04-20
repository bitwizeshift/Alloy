#include "alloy/io/window.hpp"

#include "alloy/core/assert.hpp"

//==============================================================================
// class : window
//==============================================================================

//------------------------------------------------------------------------------
// Destructors
//------------------------------------------------------------------------------

alloy::io::window::~window() = default;

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

int alloy::io::window::width()
  const
{
  return do_width();
}

int alloy::io::window::height()
  const
{
  return do_height();
}

bool alloy::io::window::is_fullscreen()
  const
{
  return do_is_fullscreen();
}

bool alloy::io::window::is_focused()
  const
{
  return do_is_focused();
}

bool alloy::io::window::is_hidden()
  const
{
  return do_is_hidden();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

void alloy::io::window::update()
{
  do_update();
}

void alloy::io::window::set_dimensions( int width, int height )
{
  ALLOY_ASSERT( width > 0 );
  ALLOY_ASSERT( height > 0 );

  do_set_dimensions(width,height);
}

void alloy::io::window::set_position( int x, int y )
{
  do_set_position(x,y);
}

//------------------------------------------------------------------------------
// Access
//------------------------------------------------------------------------------

void alloy::io::window::fullscreen()
{
  do_fullscreen();
}

void alloy::io::window::focus()
{
  do_focus();
}

void alloy::io::window::show()
{
  do_show();
}

void alloy::io::window::hide()
{
  do_hide();
}
