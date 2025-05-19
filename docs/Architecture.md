# Architecture

## Process Separation

Subspace is broken up into two processes.

### Main Process

The main process is typcally called `subspace.exe`. Its source code lives in the `subspace/` subdirectory. It is responsible for the following tasks:

- Displaying the UI
- Saving / loading user settings
- Handling the database for statistics tracking
- Writing the log file
- Running and managing the game process
- Sending configuration to the game process
- Processing events sent by the game

The main process is also sometimes referred to as the 'server' process because it hosts the control server.

### Game Process

The game process hosts the FTL game. It is called `subspacegame.exe` and the source code is under the `game/` subdirectory. Its function is:

- Open a communication channel with the main process.
- Get the basic configuration including location of the FTL game executable.
- Open the FTL game executable and load it into memory. On Windows this is done with a custom re-implementation of the runtime linker that takes care of loading all of the dependencies and relocating the image.
- Run code analysis to locate symbols and offsets, and validate patches.
- Patch the code by hooking and/or replacing functions.
- Hand off communication with the main Subspace process to a background thead.
- Jump to the FTL game code's entry point and transfer control.

The game process effectively becomes the FTL executable, but with additional patches and hooks installed.

One thing to note about the game process is that it is a 'bare' executable that does not link against the system C runtime library. This is to avoid conflicts by potentially having two different C runtimes loaded. A bare-bones implementation of some necessities is located in the `lib/minicrt` library.

It is left to the runtime that FTL links against to do all of the initial process setup during the final step of the list above.

The game process is sometimes referred to as the 'client' process because it's running the control client.

## Control Protocol

The control protocol is how the main (server) and game (client) processes communicate. It is established over a random TCP port bound to 127.0.0.1 -- the port number is passed to the game process on its command line.

A named pipe or some other form of IPC would potentially be cleaner. This is a concession to eventual cross-platform support as TCP sockets on localhost work the same across all platforms that FTL can run on.

It is a very simple protocol that can be fairly easily processed in the restricted environment that the client starts with (no C runtime; limited memory allocation capabilities).

The common parts of the implementation lives in the `shared/` subdirectory, while the client and server implement the actual send/receive loop in different ways, in a dedicated thread for the client, and using a workqueue backed by a thread pool for the server.

The protocol itself consists of messages with a string command name and arbitrary named fields that support some basic data types.

```c
// Control protocol:
// Sync bytes (0x0e 0x19 0x01 0x17)
// 4 bytes total message size
// 4 bytes unique Message ID
// 4 bytes In-Reply-To Message ID (or 0 if not in reply to anything)
// 16 bytes command name (null-terminated string)
// 4 bytes number of fields in payload
// payload: repeating
//    32-bit field size
//    16 bytes field name (null terminated string)
//    16-bit type ID
//        0 = int (32 bit)
//        1 = 32-bit float
//        2 = 64-bit float
//        3 = boolean (1 byte)
//        4 = string
//        65536 = raw data
//    16-bit flags
//        0x0001 = unsigned integer
//        0x0100 = Array (32-bit array count follows)
//    Strings are represented by a 16-bit length field, then a string WITHOUT null terminator.
//    Each field is padded to align to a 4-byte boundary
```