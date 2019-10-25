get_property(LLDB_ALL_PLUGINS GLOBAL PROPERTY LLDB_PLUGINS)

add_lldb_tool(lldb-v8
  ProcessV8Wasm.cpp
  RegisterContextV8Wasm.cpp
  ThreadV8Wasm.cpp
  FrameV8Wasm.cpp
  lldb-v8.cpp
  SystemInitializerV8.cpp

  LINK_LIBS
    lldbBase
    lldbBreakpoint
    lldbCore
    lldbDataFormatters
    lldbExpression
    lldbHost
    lldbInitialization
    lldbInterpreter
    lldbSymbol
    lldbTarget
    lldbUtility
    ${LLDB_ALL_PLUGINS}
    ${host_lib}

  LINK_COMPONENTS
    Support
  )

include_directories(${LLDB_SOURCE_DIR}/source)
