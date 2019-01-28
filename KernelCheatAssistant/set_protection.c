//#include "kca.h"
//#include "set_protection.h"
//
///// <summary>
///// Enable/disable process protection flag
///// </summary>
///// <param name="pProtection">Request params</param>
///// <returns>Status code</returns>
//NTSTATUS BBSetProtection(IN PSET_PROC_PROTECTION pProtection)
//{
//	NTSTATUS status = STATUS_SUCCESS;
//	PEPROCESS pProcess = NULL;
//
//	status = PsLookupProcessByProcessId((HANDLE)pProtection->pid, &pProcess);
//	if (NT_SUCCESS(status))
//	{
//		if (dynData.Protection != 0)
//		{
//			PUCHAR pValue = (PUCHAR)pProcess + dynData.Protection;
//
//			// Win7
//			if (dynData.ver <= WINVER_7_SP1)
//			{
//				if (pProtection->protection == Policy_Enable)
//					*(PULONG)pValue |= 1 << 0xB;
//				else if (pProtection->protection == Policy_Disable)
//					*(PULONG)pValue &= ~(1 << 0xB);
//			}
//			// Win8
//			else if (dynData.ver == WINVER_8)
//			{
//				if (pProtection->protection != Policy_Keep)
//					*pValue = pProtection->protection;
//			}
//			// Win8.1
//			else if (dynData.ver >= WINVER_81)
//			{
//				// Protection
//				if (pProtection->protection == Policy_Disable)
//				{
//					*pValue = 0;
//				}
//				else if (pProtection->protection == Policy_Enable)
//				{
//					PS_PROTECTION protBuf = { 0 };
//
//					protBuf.Flags.Signer = PsProtectedSignerWinTcb;
//					protBuf.Flags.Type = PsProtectedTypeProtected;
//					*pValue = protBuf.Level;
//				}
//
//				// Dynamic code
//				if (pProtection->dynamicCode != Policy_Keep && dynData.EProcessFlags2 != 0)
//				{
//					if (dynData.ver >= WINVER_10_RS3)
//					{
//						PMITIGATION_FLAGS pFlags2 = (PMITIGATION_FLAGS)((PUCHAR)pProcess + dynData.EProcessFlags2);
//						pFlags2->DisableDynamicCode = pProtection->dynamicCode;
//					}
//					else
//					{
//						PEPROCESS_FLAGS2 pFlags2 = (PEPROCESS_FLAGS2)((PUCHAR)pProcess + dynData.EProcessFlags2);
//						pFlags2->DisableDynamicCode = pProtection->dynamicCode;
//					}
//
//				}
//
//				// Binary signature
//				if (pProtection->signature != Policy_Keep)
//				{
//					PSE_SIGNING_LEVEL pSignLevel = (PSE_SIGNING_LEVEL)((PUCHAR)pProcess + dynData.Protection - 2);
//					PSE_SIGNING_LEVEL pSignLevelSection = (PSE_SIGNING_LEVEL)((PUCHAR)pProcess + dynData.Protection - 1);
//
//					if (pProtection->signature == Policy_Enable)
//						*pSignLevel = *pSignLevelSection = SE_SIGNING_LEVEL_MICROSOFT;
//					else
//						*pSignLevel = *pSignLevelSection = SE_SIGNING_LEVEL_UNCHECKED;
//				}
//			}
//			else
//				status = STATUS_NOT_SUPPORTED;
//		}
//		else
//		{
//			dprintf("BlackBone: %s: Invalid protection flag offset\n", __FUNCTION__);
//			status = STATUS_INVALID_ADDRESS;
//		}
//	}
//	else
//		dprintf("BlackBone: %s: PsLookupProcessByProcessId failed with status 0x%X\n", __FUNCTION__, status);
//
//	if (pProcess)
//		ObDereferenceObject(pProcess);
//
//	return status;
//}