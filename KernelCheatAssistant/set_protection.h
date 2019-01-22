//#pragma once
//typedef struct _EPROCESS_FLAGS2
//{
//	unsigned int JobNotReallyActive : 1;
//	unsigned int AccountingFolded : 1;
//	unsigned int NewProcessReported : 1;
//	unsigned int ExitProcessReported : 1;
//	unsigned int ReportCommitChanges : 1;
//	unsigned int LastReportMemory : 1;
//	unsigned int ForceWakeCharge : 1;
//	unsigned int CrossSessionCreate : 1;
//	unsigned int NeedsHandleRundown : 1;
//	unsigned int RefTraceEnabled : 1;
//	unsigned int DisableDynamicCode : 1;
//	unsigned int EmptyJobEvaluated : 1;
//	unsigned int DefaultPagePriority : 3;
//	unsigned int PrimaryTokenFrozen : 1;
//	unsigned int ProcessVerifierTarget : 1;
//	unsigned int StackRandomizationDisabled : 1;
//	unsigned int AffinityPermanent : 1;
//	unsigned int AffinityUpdateEnable : 1;
//	unsigned int PropagateNode : 1;
//	unsigned int ExplicitAffinity : 1;
//	unsigned int ProcessExecutionState : 2;
//	unsigned int DisallowStrippedImages : 1;
//	unsigned int HighEntropyASLREnabled : 1;
//	unsigned int ExtensionPointDisable : 1;
//	unsigned int ForceRelocateImages : 1;
//	unsigned int ProcessStateChangeRequest : 2;
//	unsigned int ProcessStateChangeInProgress : 1;
//	unsigned int DisallowWin32kSystemCalls : 1;
//} EPROCESS_FLAGS2, *PEPROCESS_FLAGS2;
//
//typedef struct _MITIGATION_FLAGS
//{
//	unsigned int ControlFlowGuardEnabled : 1;
//	unsigned int ControlFlowGuardExportSuppressionEnabled : 1;
//	unsigned int ControlFlowGuardStrict : 1;
//	unsigned int DisallowStrippedImages : 1;
//	unsigned int ForceRelocateImages : 1;
//	unsigned int HighEntropyASLREnabled : 1;
//	unsigned int StackRandomizationDisabled : 1;
//	unsigned int ExtensionPointDisable : 1;
//	unsigned int DisableDynamicCode : 1;
//	unsigned int DisableDynamicCodeAllowOptOut : 1;
//	unsigned int DisableDynamicCodeAllowRemoteDowngrade : 1;
//	unsigned int AuditDisableDynamicCode : 1;
//	unsigned int DisallowWin32kSystemCalls : 1;
//	unsigned int AuditDisallowWin32kSystemCalls : 1;
//	unsigned int EnableFilteredWin32kAPIs : 1;
//	unsigned int AuditFilteredWin32kAPIs : 1;
//	unsigned int DisableNonSystemFonts : 1;
//	unsigned int AuditNonSystemFontLoading : 1;
//	unsigned int PreferSystem32Images : 1;
//	unsigned int ProhibitRemoteImageMap : 1;
//	unsigned int AuditProhibitRemoteImageMap : 1;
//	unsigned int ProhibitLowILImageMap : 1;
//	unsigned int AuditProhibitLowILImageMap : 1;
//	unsigned int SignatureMitigationOptIn : 1;
//	unsigned int AuditBlockNonMicrosoftBinaries : 1;
//	unsigned int AuditBlockNonMicrosoftBinariesAllowStore : 1;
//	unsigned int LoaderIntegrityContinuityEnabled : 1;
//	unsigned int AuditLoaderIntegrityContinuity : 1;
//	unsigned int EnableModuleTamperingProtection : 1;
//	unsigned int EnableModuleTamperingProtectionNoInherit : 1;
//} MITIGATION_FLAGS, *PMITIGATION_FLAGS;
//typedef enum _PS_PROTECTED_TYPE
//{
//	PsProtectedTypeNone = 0,
//	PsProtectedTypeProtectedLight = 1,
//	PsProtectedTypeProtected = 2,
//	PsProtectedTypeMax = 3
//} PS_PROTECTED_TYPE;
//typedef enum _PS_PROTECTED_SIGNER
//{
//	PsProtectedSignerNone = 0,
//	PsProtectedSignerAuthenticode = 1,
//	PsProtectedSignerCodeGen = 2,
//	PsProtectedSignerAntimalware = 3,
//	PsProtectedSignerLsa = 4,
//	PsProtectedSignerWindows = 5,
//	PsProtectedSignerWinTcb = 6,
//	PsProtectedSignerMax = 7
//} PS_PROTECTED_SIGNER;
//typedef union _PS_PROTECTION
//{
//	UCHAR Level;
//	struct
//	{
//		int Type : 3;
//		int Audit : 1;
//		int Signer : 4;
//	} Flags;
//} PS_PROTECTION, *PPS_PROTECTION;
//typedef enum _WinVer
//{
//	WINVER_7 = 0x0610,
//	WINVER_7_SP1 = 0x0611,
//	WINVER_8 = 0x0620,
//	WINVER_81 = 0x0630,
//	WINVER_10 = 0x0A00,
//	WINVER_10_RS1 = 0x0A01, // Anniversary update
//	WINVER_10_RS2 = 0x0A02, // Creators update
//	WINVER_10_RS3 = 0x0A03, // Fall creators update
//	WINVER_10_RS4 = 0x0A04, // Spring creators update
//} WinVer;
///// <summary>
///// OS-dependent stuff
///// </summary>
//typedef struct _DYNAMIC_DATA
//{
//	WinVer  ver;            // OS version
//	ULONG   buildNo;        // OS build revision
//	BOOLEAN correctBuild;   // OS kernel build number is correct and supported
//
//	ULONG KExecOpt;         // KPROCESS::ExecuteOptions 
//	ULONG Protection;       // EPROCESS::Protection
//	ULONG EProcessFlags2;   // EPROCESS::Flags2
//	ULONG ObjTable;         // EPROCESS::ObjectTable
//	ULONG VadRoot;          // EPROCESS::VadRoot
//	ULONG NtProtectIndex;   // NtProtectVirtualMemory SSDT index
//	ULONG NtCreateThdIndex; // NtCreateThreadEx SSDT index
//	ULONG NtTermThdIndex;   // NtTerminateThread SSDT index
//	ULONG PrevMode;         // KTHREAD::PreviousMode
//	ULONG ExitStatus;       // ETHREAD::ExitStatus
//	ULONG MiAllocPage;      // MiAllocateDriverPage offset
//	ULONG ExRemoveTable;    // Ex(p)RemoveHandleTable offset
//
//	ULONG_PTR DYN_PDE_BASE; // Win10 AU+ relocated PDE base VA
//	ULONG_PTR DYN_PTE_BASE; // Win10 AU+ relocated PTE base VA
//} DYNAMIC_DATA, *PDYNAMIC_DATA;
//// OS Dependant data
//DYNAMIC_DATA dynData;
//
//
//
//
//NTSTATUS BBSetProtection(IN PSET_PROC_PROTECTION pProtection);