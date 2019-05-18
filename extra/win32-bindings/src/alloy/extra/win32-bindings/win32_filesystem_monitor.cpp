#include "alloy/extra/win32-bindings/win32_filesystem_monitor.hpp"
#include "alloy/core/intrinsics.hpp"

#include "windows.hpp"

#include <vector>
#include <map>
#include <string>
#include <codecvt>
#include <locale>

namespace {

  struct watch_handle
  {
    ::HANDLE    handle;
    bool        recursive;
    std::string path;
    ::DWORD     notification_filter;
  };

} // anonymous namespace

//==============================================================================
// struct : win32_filesystem_monitor::impl
//==============================================================================

struct alloy::extra::win32_filesystem_monitor::impl
{
  std::vector<::watch_handle> watch_handles;
  std::map<::HANDLE,::HANDLE> directory_handles;
};

//==============================================================================
// class : win32_filesystem_monitor
//==============================================================================

//------------------------------------------------------------------------------
// Constructor / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::extra::win32_filesystem_monitor::win32_filesystem_monitor()
  : m_impl{std::make_unique<win32_filesystem_monitor::impl>()}
{

}

alloy::extra::win32_filesystem_monitor
  ::win32_filesystem_monitor( win32_filesystem_monitor&& ) noexcept = default;

//------------------------------------------------------------------------------

alloy::extra::win32_filesystem_monitor::~win32_filesystem_monitor()
  noexcept
{
  // Close the open notification handles
  for (auto const& handle : m_impl->watch_handles) {
    ::FindCloseChangeNotification( handle.handle );
  }

  // Close the open directory handles
  for (auto const& pair : m_impl->directory_handles) {
    ::CloseHandle( pair.second );
  }
}

//------------------------------------------------------------------------------

alloy::extra::win32_filesystem_monitor& alloy::extra::win32_filesystem_monitor
  ::operator=( win32_filesystem_monitor&& ) noexcept = default;

//------------------------------------------------------------------------------
// Private Hooks
//------------------------------------------------------------------------------

void alloy::extra::win32_filesystem_monitor::do_watch( std::string_view path,
                                                       bool recursive )
  noexcept
{
  const auto filter = FILE_NOTIFY_CHANGE_FILE_NAME  |
                      FILE_NOTIFY_CHANGE_DIR_NAME   |
                      FILE_NOTIFY_CHANGE_SIZE       |
                      FILE_NOTIFY_CHANGE_LAST_WRITE |
                      FILE_NOTIFY_CHANGE_CREATION;

  // TODO(bitwizeshift): Check that 'path' doesn't already exist

  auto handle = ::FindFirstChangeNotificationA( path.data(), recursive, filter );

  auto object = ::watch_handle{
    handle,
    recursive,
    std::string{path},
    static_cast<::DWORD>(filter)
  };

  m_impl->watch_handles.emplace_back( std::move(object) );

  m_impl->directory_handles[handle] = ::CreateFileA(
    path.data(),
    FILE_LIST_DIRECTORY,
    FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
    0,
    OPEN_EXISTING,
    FILE_FLAG_BACKUP_SEMANTICS,
    nullptr
  );
}

//------------------------------------------------------------------------------

void alloy::extra::win32_filesystem_monitor::pump( io::message_pump& p )
  noexcept
{
  static constexpr auto buffer_size = 256;

  using storage_type = std::aligned_storage_t<buffer_size,alignof(::FILE_NOTIFY_INFORMATION)>;

  auto buffer = storage_type{};

  // Poll each watch handle for new changes
  for (auto& handle : m_impl->watch_handles) {

    // TODO(bitwizeshift):
    //   Figure out how to get ALL file changes in a single pump call.
    //   Putting 'WaitForSingleObject in a loop, comparing the result against
    //   'WAIT_TIMEOUT' seems to trigger an infinite loop.

    const auto result = ::WaitForSingleObject( handle.handle, 0 );

    // If we timed out, skip this for later
    if (result == WAIT_TIMEOUT) {
      continue;
    }

    auto directory_handle = m_impl->directory_handles[handle.handle];

    auto bytes_read = ::DWORD{};

    ::ReadDirectoryChangesW(
      directory_handle,
      static_cast<void*>(&buffer),
      buffer_size,
      static_cast<::WINBOOL>(handle.recursive),
      handle.notification_filter,
      &bytes_read,
      nullptr,
      nullptr
    );

    // TODO(bitwizeshift): Manage error (?)
    if (bytes_read <= 0) {
      continue;
    }

    using converter_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<converter_type, wchar_t> converter;

    const auto* current = reinterpret_cast<const unsigned char*>(&buffer);
    const auto* prev = current;
    do {
      const auto* notify_info = reinterpret_cast<const ::FILE_NOTIFY_INFORMATION*>(current);
      const auto wfilename = std::wstring{
        notify_info->FileName,
        notify_info->FileNameLength / sizeof(::WCHAR)
      };
      auto filename = converter.to_bytes(wfilename);

      switch (notify_info->Action) {
        case FILE_ACTION_ADDED: {
          auto event = io::file_creation_event {
            std::move(filename)
          };
          p.post_event(event);
          break;
        }
        case FILE_ACTION_REMOVED: {
          auto event = io::file_removal_event {
            std::move(filename)
          };
          p.post_event(event);
          break;
        }
        case FILE_ACTION_MODIFIED: {
          auto event = io::file_update_event {
            std::move(filename)
          };
          p.post_event(event);
          break;
        }
        // Renaming a file is considered a deletion and addition
        case FILE_ACTION_RENAMED_OLD_NAME: {
          auto event = io::file_removal_event {
            std::move(filename)
          };
          p.post_event(event);
          break;
        }
        case FILE_ACTION_RENAMED_NEW_NAME: {
          auto event = io::file_creation_event {
            std::move(filename)
          };
          p.post_event(event);
          break;
        }
        default:
          ALLOY_UNREACHABLE();
          break;
      }

      if (notify_info->NextEntryOffset==0U) {
        break;
      }
      prev = current;
      current += notify_info->NextEntryOffset;
    } while (current != prev);

    // Keep waiting for the next notification
    ::FindNextChangeNotification( handle.handle );
  } // for (...)
}
