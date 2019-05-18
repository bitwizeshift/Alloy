# Example: WIN32 Filesystem Watcher

This contains a simple example using the `win32_filesystem_watcher` extra.
The watcher is a pump-source that injects events into the `message_pump`
for when files or directories change.

**Usage:**
```
./win32-filesystem-watcher-example.exe <directory>
```
Watches `<directory>` and pumps messages until 10 events have occurred.