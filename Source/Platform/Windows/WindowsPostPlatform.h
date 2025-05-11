
#undef INT
#undef UINT
#undef DWORD
#undef FLOAT

// Undo any Windows defines.
#undef uint8
#undef uint16
#undef uint32
#undef int32
#undef float
#undef CDECL
#undef PF_MAX
#undef CaptureStackBackTrace
#undef CopyFile
#undef CreateDesktop
#undef CreateDirectory
#undef CreateFont
#undef DeleteFile
#undef DrawText
#undef FindWindow
#undef GetClassInfo
#undef GetClassName
#undef GetCommandLine
#undef GetCurrentTime
#undef GetEnvironmentVariable
#undef GetFileAttributes
#undef GetFreeSpace
#undef GetMessage
#undef GetNextSibling
#undef GetObject
#undef GetProp
#undef GetTempFileName
#undef IMediaEventSink
#undef IsMaximized
#undef IsMinimized
#undef LoadString
#undef MemoryBarrier
#undef MoveFile
#undef PlaySound
#undef PostMessage
#undef ReportEvent
#undef SendMessage
#undef SetPort
#undef SetProp
#undef UpdateResource
#undef Yield

// Restore any previously defined macros
#pragma pop_macro("MAX_uint8")
#pragma pop_macro("MAX_uint16")
#pragma pop_macro("MAX_uint32")
#pragma pop_macro("MAX_int32")
#pragma pop_macro("TEXT")
#pragma pop_macro("TRUE")
#pragma pop_macro("FALSE")