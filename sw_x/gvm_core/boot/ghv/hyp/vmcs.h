#ifndef __VMCS_H__
#define __VMCS_H__

#define VMCSE_WIDTH_SHIFT   13
#define VMCSE_WIDTH_WIDTH    2
#define VMCSE_TYPE_SHIFT    10
#define VMCSE_TYPE_WIDTH     2
#define VMCSE_INDEX_SHIFT    1
#define VMCSE_INDEX_WIDTH    9
#define VMCSE_ACCESS_SHIFT   0
#define VMCSE_ACCESS_WIDTH   1

#define VMCS_ENCODE(width, type, index, access) \
    ((width << VMCSE_WIDTH_SHIFT) | \
     (type << VMCSE_TYPE_SHIFT) | \
     (index << VMCSE_INDEX_SHIFT) | \
     (access))

#define _VMCE_WIDTH_16  0
#define _VMCE_WIDTH_64  1
#define _VMCE_WIDTH_32  2
#define _VMCE_WIDTH_N   3

#define _VMCE_TYPE_CONTROL  0
#define _VMCE_TYPE_RO       1
#define _VMCE_TYPE_GUEST    2
#define _VMCE_TYPE_HOST     3

#define _VMCE_ACCESS_FULL   0
#define _VMCE_ACCESS_HIGH   1

#define VMCSE_WIDTH_MASK    ((1 << VMCSE_WIDTH_WIDTH) - 1) 	
#define field_width_is_64(f) ((((f) >> VMCSE_WIDTH_SHIFT) & VMCSE_WIDTH_MASK) \
			      == _VMCE_WIDTH_64)

#define VMCS_ENCODE_16_CONTROL(index) VMCS_ENCODE(_VMCE_WIDTH_16, \
						  _VMCE_TYPE_CONTROL, \
						  (index), \
						  _VMCE_ACCESS_FULL)

#define _VMCE_VPID_IDX		0

#define VMCE_VPID		VMCS_ENCODE_16_CONTROL(_VMCE_VPID_IDX)

#define VMCS_ENCODE_16_GUEST(index) VMCS_ENCODE(_VMCE_WIDTH_16, \
                                                _VMCE_TYPE_GUEST, \
                                                (index), \
                                                _VMCE_ACCESS_FULL)

#define _VMCE_GUEST_ES_SEL_IDX    0
#define _VMCE_GUEST_CS_SEL_IDX    1
#define _VMCE_GUEST_SS_SEL_IDX    2
#define _VMCE_GUEST_DS_SEL_IDX    3
#define _VMCE_GUEST_FS_SEL_IDX    4
#define _VMCE_GUEST_GS_SEL_IDX    5
#define _VMCE_GUEST_LDTR_SEL_IDX  6
#define _VMCE_GUEST_TR_SEL_IDX    7

#define VMCE_GUEST_ES_SEL       VMCS_ENCODE_16_GUEST(_VMCE_GUEST_ES_SEL_IDX)
#define VMCE_GUEST_CS_SEL       VMCS_ENCODE_16_GUEST(_VMCE_GUEST_CS_SEL_IDX)
#define VMCE_GUEST_SS_SEL       VMCS_ENCODE_16_GUEST(_VMCE_GUEST_SS_SEL_IDX)
#define VMCE_GUEST_DS_SEL       VMCS_ENCODE_16_GUEST(_VMCE_GUEST_DS_SEL_IDX)
#define VMCE_GUEST_FS_SEL       VMCS_ENCODE_16_GUEST(_VMCE_GUEST_FS_SEL_IDX)
#define VMCE_GUEST_GS_SEL       VMCS_ENCODE_16_GUEST(_VMCE_GUEST_GS_SEL_IDX)
#define VMCE_GUEST_LDTR_SEL     VMCS_ENCODE_16_GUEST(_VMCE_GUEST_LDTR_SEL_IDX)
#define VMCE_GUEST_TR_SEL       VMCS_ENCODE_16_GUEST(_VMCE_GUEST_TR_SEL_IDX)

#define VMCS_ENCODE_16_HOST(index)  VMCS_ENCODE(_VMCE_WIDTH_16, \
                                                _VMCE_TYPE_HOST, \
                                                (index), \
                                                _VMCE_ACCESS_FULL)

#define _VMCE_HOST_ES_SEL_IDX     0
#define _VMCE_HOST_CS_SEL_IDX     1
#define _VMCE_HOST_SS_SEL_IDX     2
#define _VMCE_HOST_DS_SEL_IDX     3
#define _VMCE_HOST_FS_SEL_IDX     4
#define _VMCE_HOST_GS_SEL_IDX     5
#define _VMCE_HOST_TR_SEL_IDX     6

#define VMCE_HOST_ES_SEL        VMCS_ENCODE_16_HOST(_VMCE_HOST_ES_SEL_IDX)
#define VMCE_HOST_CS_SEL        VMCS_ENCODE_16_HOST(_VMCE_HOST_CS_SEL_IDX)
#define VMCE_HOST_SS_SEL        VMCS_ENCODE_16_HOST(_VMCE_HOST_SS_SEL_IDX)
#define VMCE_HOST_DS_SEL        VMCS_ENCODE_16_HOST(_VMCE_HOST_DS_SEL_IDX)
#define VMCE_HOST_FS_SEL        VMCS_ENCODE_16_HOST(_VMCE_HOST_FS_SEL_IDX)
#define VMCE_HOST_GS_SEL        VMCS_ENCODE_16_HOST(_VMCE_HOST_GS_SEL_IDX)
#define VMCE_HOST_TR_SEL        VMCS_ENCODE_16_HOST(_VMCE_HOST_TR_SEL_IDX)

#define VMCS_ENCODE_64_CONTROL(index)	VMCS_ENCODE(_VMCE_WIDTH_64, \
                                                    _VMCE_TYPE_CONTROL, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_ADDR_IO_BITMAP_A_IDX		0
#define _VMCE_ADDR_IO_BITMAP_B_IDX		1
#define _VMCE_ADDR_MSR_BITMAPS_IDX		2
#define _VMCE_VMEXIT_MSR_STORE_ADDR_IDX		3
#define _VMCE_VMEXIT_MSR_LOAD_ADDR_IDX		4
#define _VMCE_VMENTRY_MSR_LOAD_ADDR_IDX		5
#define _VMCE_EXECUTIVE_VMCS_PTR_IDX		6
/* unused					7 */
#define _VMCE_TSC_OFFSET_IDX			8
#define _VMCE_VIRTUAL_APIC_ADDR_IDX		9
#define _VMCE_APIC_ACCESS_ADDR_IDX		10
#define _VMCE_EPT_PTR_IDX			13

#define VMCE_ADDR_IO_BITMAP_A \
	VMCS_ENCODE_64_CONTROL(_VMCE_ADDR_IO_BITMAP_A_IDX)
#define VMCE_ADDR_IO_BITMAP_B \
	VMCS_ENCODE_64_CONTROL(_VMCE_ADDR_IO_BITMAP_B_IDX)
#define VMCE_ADDR_MSR_BITMAPS \
	VMCS_ENCODE_64_CONTROL(_VMCE_ADDR_MSR_BITMAPS_IDX)
#define VMCE_VMEXIT_MSR_STORE_ADDR \
	VMCS_ENCODE_64_CONTROL(_VMCE_VMEXIT_MSR_STORE_ADDR_IDX)
#define VMCE_VMEXIT_MSR_LOAD_ADDR \
	VMCS_ENCODE_64_CONTROL(_VMCE_VMEXIT_MSR_LOAD_ADDR_IDX)
#define VMCE_VMENTRY_MSR_LOAD_ADDR \
	VMCS_ENCODE_64_CONTROL(_VMCE_VMENTRY_MSR_LOAD_ADDR_IDX)
#define VMCE_EXECUTIVE_VMCS_PTR \
	VMCS_ENCODE_64_CONTROL(_VMCE_EXECUTIVE_VMCS_PTR_IDX)
#define VMCE_TSC_OFFSET \
	VMCS_ENCODE_64_CONTROL(_VMCE_TSC_OFFSET_IDX)
#define VMCE_VIRTUAL_APIC_ADDR \
	VMCS_ENCODE_64_CONTROL(_VMCE_VIRTUAL_APIC_ADDR_IDX)
#define VMCE_APIC_ACCESS_ADDR \
	VMCS_ENCODE_64_CONTROL(_VMCE_APIC_ACCESS_ADDR_IDX)
#define VMCE_EPT_PTR \
	VMCS_ENCODE_64_CONTROL(_VMCE_EPT_PTR_IDX)

#define VMCS_ENCODE_64_RO(index)	VMCS_ENCODE(_VMCE_WIDTH_64, \
                                                    _VMCE_TYPE_RO, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_GUEST_PHYS_ADDR_IDX		0

#define VMCE_GUEST_PHYS_ADDR \
	VMCS_ENCODE_64_RO(_VMCE_GUEST_PHYS_ADDR_IDX)

#define VMCS_ENCODE_64_GUEST(index)	VMCS_ENCODE(_VMCE_WIDTH_64, \
                                                    _VMCE_TYPE_GUEST, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_VMCS_LINK_PTR_IDX			0
#define _VMCE_GUEST_IA32_DEBUGCTL_IDX		1
#define _VMCE_GUEST_IA32_PAT_IDX		2
#define _VMCE_GUEST_IA32_EFER_IDX		3
#define _VMCE_GUEST_IA32_PERF_GLOBAL_CTRL_IDX	4
#define _VMCE_GUEST_PDPTE0_IDX			5
#define _VMCE_GUEST_PDPTE1_IDX			6
#define _VMCE_GUEST_PDPTE2_IDX			7
#define _VMCE_GUEST_PDPTE3_IDX			8

#define VMCE_VMCS_LINK_PTR \
	VMCS_ENCODE_64_GUEST(_VMCE_VMCS_LINK_PTR_IDX)
#define VMCE_GUEST_IA32_DEBUGCTL \
	VMCS_ENCODE_64_GUEST(_VMCE_GUEST_IA32_DEBUGCTL_IDX)
#define VMCE_GUEST_IA32_PAT \
	VMCS_ENCODE_64_GUEST(_VMCE_GUEST_IA32_PAT_IDX)
#define VMCE_GUEST_IA32_EFER \
	VMCS_ENCODE_64_GUEST(_VMCE_GUEST_IA32_EFER_IDX)
#define VMCE_GUEST_IA32_PERF_GLOBAL_CTRL \
	VMCS_ENCODE_64_GUEST(_VMCE_GUEST_IA32_PERF_GLOBAL_CTRL_IDX)
#define VMCE_GUEST_PDPTE0 \
	VMCS_ENCODE_64_GUEST(_VMCE_GUEST_PDPTE0_IDX)
#define VMCE_GUEST_PDPTE1 \
	VMCS_ENCODE_64_GUEST(_VMCE_GUEST_PDPTE1_IDX)
#define VMCE_GUEST_PDPTE2 \
	VMCS_ENCODE_64_GUEST(_VMCE_GUEST_PDPTE2_IDX)
#define VMCE_GUEST_PDPTE3 \
	VMCS_ENCODE_64_GUEST(_VMCE_GUEST_PDPTE3_IDX)

#define VMCS_ENCODE_64_HOST(index)	VMCS_ENCODE(_VMCE_WIDTH_64, \
                                                    _VMCE_TYPE_HOST, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_HOST_IA32_PAT_IDX			0
#define _VMCE_HOST_IA32_EFER_IDX		1
#define _VMCE_HOST_IA32_PERF_GLOBAL_CTRL_IDX	2

#define VMCE_HOST_IA32_PAT	VMCS_ENCODE_64_HOST(_VMCE_HOST_IA32_PAT_IDX)
#define VMCE_HOST_IA32_EFER	VMCS_ENCODE_64_HOST(_VMCE_HOST_IA32_EFER_IDX)
#define VMCE_HOST_IA32_PERF_GLOBAL_CTRL \
	VMCS_ENCODE_64_HOST(_VMCE_HOST_IA32_PERF_GLOBAL_CTRL_IDX)

#define VMCS_ENCODE_32_CONTROL(index)	VMCS_ENCODE(_VMCE_WIDTH_32, \
                                                    _VMCE_TYPE_CONTROL, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_PIN_BASED_CONTROLS_IDX			0
#define _VMCE_PRIMARY_CPU_BASED_CONTROLS_IDX		1
#define _VMCE_EXCEPTION_BITMAP_IDX			2
#define _VMCE_PAGE_FAULT_ERROR_CODE_MASK_IDX		3
#define _VMCE_PAGE_FAULT_ERROR_CODE_MATCH_IDX		4
#define _VMCE_CR3_TARGET_COUNT_IDX			5
#define _VMCE_VMEXIT_CONTROLS_IDX			6
#define _VMCE_VMEXIT_MSR_STORE_COUNT_IDX		7
#define _VMCE_VMEXIT_MSR_LOAD_COUNT_IDX			8
#define _VMCE_VMENTRY_CONTROLS_IDX			9
#define _VMCE_VMENTRY_MSR_LOAD_COUNT_IDX		10
#define _VMCE_VMENTRY_INTERRUPT_INFO_FIELD_IDX		11
#define _VMCE_VMENTRY_EXCEPTION_ERROR_CODE_IDX		12
#define _VMCE_VMENTRY_INSTRUCTION_LENGTH_IDX		13
#define _VMCE_TPR_THRESHOLD_IDX				14
#define _VMCE_SECONDARY_CPU_BASED_CONTROLS_IDX		15

#define VMCE_PIN_BASED_CONTROLS \
	VMCS_ENCODE_32_CONTROL(_VMCE_PIN_BASED_CONTROLS_IDX)
#define VMCE_PRIMARY_CPU_BASED_CONTROLS \
       	VMCS_ENCODE_32_CONTROL(_VMCE_PRIMARY_CPU_BASED_CONTROLS_IDX)
#define VMCE_EXCEPTION_BITMAP \
	VMCS_ENCODE_32_CONTROL(_VMCE_EXCEPTION_BITMAP_IDX)
#define VMCE_PAGE_FAULT_ERROR_CODE_MASK \
	VMCS_ENCODE_32_CONTROL(_VMCE_PAGE_FAULT_ERROR_CODE_MASK_IDX)
#define VMCE_PAGE_FAULT_ERROR_CODE_MATCH \
	VMCS_ENCODE_32_CONTROL(_VMCE_PAGE_FAULT_ERROR_CODE_MATCH_IDX)
#define VMCE_CR3_TARGET_COUNT \
	VMCS_ENCODE_32_CONTROL(_VMCE_CR3_TARGET_COUNT_IDX)
#define VMCE_VMEXIT_CONTROLS \
	VMCS_ENCODE_32_CONTROL(_VMCE_VMEXIT_CONTROLS_IDX)
#define VMCE_VMEXIT_MSR_STORE_COUNT \
	VMCS_ENCODE_32_CONTROL(_VMCE_VMEXIT_MSR_STORE_COUNT_IDX)
#define VMCE_VMEXIT_MSR_LOAD_COUNT \
	VMCS_ENCODE_32_CONTROL(_VMCE_VMEXIT_MSR_LOAD_COUNT_IDX)
#define VMCE_VMENTRY_CONTROLS \
	VMCS_ENCODE_32_CONTROL(_VMCE_VMENTRY_CONTROLS_IDX)
#define VMCE_VMENTRY_MSR_LOAD_COUNT \
	VMCS_ENCODE_32_CONTROL(_VMCE_VMENTRY_MSR_LOAD_COUNT_IDX)
#define VMCE_VMENTRY_INTERRUPT_INFO_FIELD \
	VMCS_ENCODE_32_CONTROL(_VMCE_VMENTRY_INTERRUPT_INFO_FIELD_IDX)
#define VMCE_VMENTRY_EXCEPTION_ERROR_CODE \
	VMCS_ENCODE_32_CONTROL(_VMCE_VMENTRY_EXCEPTION_ERROR_CODE_IDX)
#define VMCE_VMENTRY_INSTRUCTION_LENGTH \
	VMCS_ENCODE_32_CONTROL(_VMCE_VMENTRY_INSTRUCTION_LENGTH_IDX)
#define VMCE_TPR_THRESHOLD \
	VMCS_ENCODE_32_CONTROL(_VMCE_TPR_THRESHOLD_IDX)
#define VMCE_SECONDARY_CPU_BASED_CONTROLS \
	VMCS_ENCODE_32_CONTROL(_VMCE_SECONDARY_CPU_BASED_CONTROLS_IDX)

#define VMCS_ENCODE_32_RO(index)	VMCS_ENCODE(_VMCE_WIDTH_32, \
                                                    _VMCE_TYPE_RO, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_VM_INSTRUCTION_ERROR_IDX		0
#define _VMCE_EXIT_REASON_IDX			1
#define _VMCE_VMEXIT_INTERRUPT_INFO_IDX		2
#define _VMCE_VMEXIT_INTERRUPT_ERROR_CODE_IDX	3
#define _VMCE_IDT_VECTORING_INFO_FIELD_IDX	4
#define _VMCE_IDT_VECTORING_ERROR_CODE_IDX	5
#define _VMCE_VMEXIT_INSTRUCTION_LENGTH_IDX	6
#define _VMCE_VMEXIT_INSTRUCTION_INFO_IDX	7

#define VMCE_VM_INSTRUCTION_ERROR \
	VMCS_ENCODE_32_RO(_VMCE_VM_INSTRUCTION_ERROR_IDX)
#define VMCE_EXIT_REASON \
	VMCS_ENCODE_32_RO(_VMCE_EXIT_REASON_IDX)
#define VMCE_VMEXIT_INTERRUPT_INFO \
	VMCS_ENCODE_32_RO(_VMCE_VMEXIT_INTERRUPT_INFO_IDX)
#define VMCE_VMEXIT_INTERRUPT_ERROR_CODE \
	VMCS_ENCODE_32_RO(_VMCE_VMEXIT_INTERRUPT_ERROR_CODE_IDX)
#define VMCE_IDT_VECTORING_INFO_FIELD \
	VMCS_ENCODE_32_RO(_VMCE_IDT_VECTORING_INFO_FIELD_IDX)
#define VMCE_IDT_VECTORING_ERROR_CODE \
	VMCS_ENCODE_32_RO(_VMCE_IDT_VECTORING_ERROR_CODE_IDX)
#define VMCE_VMEXIT_INSTRUCTION_LENGTH \
	VMCS_ENCODE_32_RO(_VMCE_VMEXIT_INSTRUCTION_LENGTH_IDX)
#define VMCE_VMEXIT_INSTRUCTION_INFO \
	VMCS_ENCODE_32_RO(_VMCE_VMEXIT_INSTRUCTION_INFO_IDX)

#define VMCS_ENCODE_32_GUEST(index)	VMCS_ENCODE(_VMCE_WIDTH_32, \
                                                    _VMCE_TYPE_GUEST, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_GUEST_ES_LIMIT_IDX		0
#define _VMCE_GUEST_CS_LIMIT_IDX		1
#define _VMCE_GUEST_SS_LIMIT_IDX		2
#define _VMCE_GUEST_DS_LIMIT_IDX		3
#define _VMCE_GUEST_FS_LIMIT_IDX		4
#define _VMCE_GUEST_GS_LIMIT_IDX		5
#define _VMCE_GUEST_LDTR_LIMIT_IDX		6
#define _VMCE_GUEST_TR_LIMIT_IDX		7
#define _VMCE_GUEST_GDTR_LIMIT_IDX		8
#define _VMCE_GUEST_IDTR_LIMIT_IDX		9
#define _VMCE_GUEST_ES_ACCESS_RIGHTS_IDX	10
#define _VMCE_GUEST_CS_ACCESS_RIGHTS_IDX	11
#define _VMCE_GUEST_SS_ACCESS_RIGHTS_IDX	12
#define _VMCE_GUEST_DS_ACCESS_RIGHTS_IDX	13
#define _VMCE_GUEST_FS_ACCESS_RIGHTS_IDX	14
#define _VMCE_GUEST_GS_ACCESS_RIGHTS_IDX	15
#define _VMCE_GUEST_LDTR_ACCESS_RIGHTS_IDX	16
#define _VMCE_GUEST_TR_ACCESS_RIGHTS_IDX	17
#define _VMCE_GUEST_INTERRUPTIBILITY_STATE_IDX	18
#define _VMCE_GUEST_ACTIVITY_STATE_IDX		19
#define _VMCE_GUEST_SMBASE_IDX			20
#define _VMCE_GUEST_IA32_SYSENTER_CS_IDX	21
#define _VMCE_VMX_PREEMPTION_TIMER_VALUE_IDX	23

#define VMCE_GUEST_ES_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_ES_LIMIT_IDX)
#define VMCE_GUEST_CS_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_CS_LIMIT_IDX)
#define VMCE_GUEST_SS_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_SS_LIMIT_IDX)
#define VMCE_GUEST_DS_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_DS_LIMIT_IDX)
#define VMCE_GUEST_FS_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_FS_LIMIT_IDX)
#define VMCE_GUEST_GS_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_GS_LIMIT_IDX)
#define VMCE_GUEST_LDTR_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_LDTR_LIMIT_IDX)
#define VMCE_GUEST_TR_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_TR_LIMIT_IDX)
#define VMCE_GUEST_GDTR_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_GDTR_LIMIT_IDX)
#define VMCE_GUEST_IDTR_LIMIT	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_IDTR_LIMIT_IDX)
#define VMCE_GUEST_ES_ACCESS_RIGHTS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_ES_ACCESS_RIGHTS_IDX)
#define VMCE_GUEST_CS_ACCESS_RIGHTS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_CS_ACCESS_RIGHTS_IDX)
#define VMCE_GUEST_SS_ACCESS_RIGHTS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_SS_ACCESS_RIGHTS_IDX)
#define VMCE_GUEST_DS_ACCESS_RIGHTS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_DS_ACCESS_RIGHTS_IDX)
#define VMCE_GUEST_FS_ACCESS_RIGHTS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_FS_ACCESS_RIGHTS_IDX)
#define VMCE_GUEST_GS_ACCESS_RIGHTS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_GS_ACCESS_RIGHTS_IDX)
#define VMCE_GUEST_LDTR_ACCESS_RIGHTS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_LDTR_ACCESS_RIGHTS_IDX)
#define VMCE_GUEST_TR_ACCESS_RIGHTS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_TR_ACCESS_RIGHTS_IDX)
#define VMCE_GUEST_INTERRUPTIBILITY_STATE \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_INTERRUPTIBILITY_STATE_IDX)
#define VMCE_GUEST_ACTIVITY_STATE \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_ACTIVITY_STATE_IDX)
#define VMCE_GUEST_SMBASE \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_SMBASE_IDX)
#define VMCE_GUEST_IA32_SYSENTER_CS \
	VMCS_ENCODE_32_GUEST(_VMCE_GUEST_IA32_SYSENTER_CS_IDX)
#define VMCE_VMX_PREEMPTION_TIMER_VALUE \
	VMCS_ENCODE_32_GUEST(_VMCE_VMX_PREEMPTION_TIMER_VALUE_IDX)

#define VMCS_ENCODE_32_HOST(index)	VMCS_ENCODE(_VMCE_WIDTH_32, \
                                                    _VMCE_TYPE_HOST, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_HOST_IA32_SYSENTER_CS_IDX		0

#define VMCE_HOST_IA32_SYSENTER_CS \
	VMCS_ENCODE_32_HOST(_VMCE_HOST_IA32_SYSENTER_CS_IDX)

#define VMCS_ENCODE_N_CONTROL(index)	VMCS_ENCODE(_VMCE_WIDTH_N, \
                                                    _VMCE_TYPE_CONTROL, \
                                                    (index), \
                                                    _VMCE_ACCESS_FULL)

#define _VMCE_CR0_GUEST_HOST_MASK_IDX		0
#define _VMCE_CR4_GUEST_HOST_MASK_IDX		1
#define _VMCE_CR0_READ_SHADOW_IDX		2
#define _VMCE_CR4_READ_SHADOW_IDX		3
#define _VMCE_CR3_TARGET_VALUE_0_IDX		4
#define _VMCE_CR3_TARGET_VALUE_1_IDX		5
#define _VMCE_CR3_TARGET_VALUE_2_IDX		6
#define _VMCE_CR3_TARGET_VALUE_3_IDX		7

#define VMCE_CR0_GUEST_HOST_MASK \
	VMCS_ENCODE_N_CONTROL(_VMCE_CR0_GUEST_HOST_MASK_IDX)
#define VMCE_CR4_GUEST_HOST_MASK \
	VMCS_ENCODE_N_CONTROL(_VMCE_CR4_GUEST_HOST_MASK_IDX)
#define VMCE_CR0_READ_SHADOW	VMCS_ENCODE_N_CONTROL(_VMCE_CR0_READ_SHADOW_IDX)
#define VMCE_CR4_READ_SHADOW	VMCS_ENCODE_N_CONTROL(_VMCE_CR4_READ_SHADOW_IDX)
#define VMCE_CR3_TARGET_VALUE_0 \
	VMCS_ENCODE_N_CONTROL(_VMCE_CR3_TARGET_VALUE_0_IDX)
#define VMCE_CR3_TARGET_VALUE_1 \
	VMCS_ENCODE_N_CONTROL(_VMCE_CR3_TARGET_VALUE_1_IDX)
#define VMCE_CR3_TARGET_VALUE_2 \
	VMCS_ENCODE_N_CONTROL(_VMCE_CR3_TARGET_VALUE_2_IDX)
#define VMCE_CR3_TARGET_VALUE_3 \
	VMCS_ENCODE_N_CONTROL(_VMCE_CR3_TARGET_VALUE_3_IDX)

#define VMCS_ENCODE_N_RO(index)		VMCS_ENCODE(_VMCE_WIDTH_N, \
						    _VMCE_TYPE_RO, \
						    (index), \
						    _VMCE_ACCESS_FULL)

#define _VMCE_EXIT_QUALIFICATION_IDX		0
#define _VMCE_IO_RCX_IDX			1
#define _VMCE_IO_RSI_IDX			2
#define _VMCE_IO_RDI_IDX			3
#define _VMCE_IO_RIP_IDX			4
#define _VMCE_GUEST_LINEAR_ADDR_IDX		5

#define VMCE_EXIT_QUALIFICATION	VMCS_ENCODE_N_RO(_VMCE_EXIT_QUALIFICATION_IDX)
#define VMCE_IO_RCX		VMCS_ENCODE_N_RO(_VMCE_IO_RCX_IDX)
#define VMCE_IO_RSI		VMCS_ENCODE_N_RO(_VMCE_IO_RSI_IDX)
#define VMCE_IO_RDI		VMCS_ENCODE_N_RO(_VMCE_IO_RDI_IDX)
#define VMCE_IO_RIP		VMCS_ENCODE_N_RO(_VMCE_IO_RIP_IDX)
#define VMCE_GUEST_LINEAR_ADDR	VMCS_ENCODE_N_RO(_VMCE_GUEST_LINEAR_ADDR_IDX)

#define VMCS_ENCODE_N_GUEST(index)	VMCS_ENCODE(_VMCE_WIDTH_N, \
						    _VMCE_TYPE_GUEST, \
						    (index), \
						    _VMCE_ACCESS_FULL)

#define _VMCE_GUEST_CR0_IDX				0
#define _VMCE_GUEST_CR3_IDX				1
#define _VMCE_GUEST_CR4_IDX				2
#define _VMCE_GUEST_ES_BASE_IDX				3
#define _VMCE_GUEST_CS_BASE_IDX				4
#define _VMCE_GUEST_SS_BASE_IDX				5
#define _VMCE_GUEST_DS_BASE_IDX				6
#define _VMCE_GUEST_FS_BASE_IDX				7
#define _VMCE_GUEST_GS_BASE_IDX				8
#define _VMCE_GUEST_LDTR_BASE_IDX			9
#define _VMCE_GUEST_TR_BASE_IDX				10
#define _VMCE_GUEST_GDTR_BASE_IDX			11
#define _VMCE_GUEST_IDTR_BASE_IDX			12
#define _VMCE_GUEST_DR7_IDX				13
#define _VMCE_GUEST_RSP_IDX				14
#define _VMCE_GUEST_RIP_IDX				15
#define _VMCE_GUEST_RFLAGS_IDX				16
#define _VMCE_GUEST_PENDING_DEBUG_EXCEPTIONS_IDX	17
#define _VMCE_GUEST_IA32_SYSENTER_ESP_IDX		18
#define _VMCE_GUEST_IA32_SYSENTER_EIP_IDX		19

#define VMCE_GUEST_CR0		VMCS_ENCODE_N_GUEST(_VMCE_GUEST_CR0_IDX)
#define VMCE_GUEST_CR3		VMCS_ENCODE_N_GUEST(_VMCE_GUEST_CR3_IDX)
#define VMCE_GUEST_CR4		VMCS_ENCODE_N_GUEST(_VMCE_GUEST_CR4_IDX)
#define VMCE_GUEST_ES_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_ES_BASE_IDX)
#define VMCE_GUEST_CS_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_CS_BASE_IDX)
#define VMCE_GUEST_SS_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_SS_BASE_IDX)
#define VMCE_GUEST_DS_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_DS_BASE_IDX)
#define VMCE_GUEST_FS_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_FS_BASE_IDX)
#define VMCE_GUEST_GS_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_GS_BASE_IDX)
#define VMCE_GUEST_LDTR_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_LDTR_BASE_IDX)
#define VMCE_GUEST_TR_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_TR_BASE_IDX)
#define VMCE_GUEST_GDTR_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_GDTR_BASE_IDX)
#define VMCE_GUEST_IDTR_BASE	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_IDTR_BASE_IDX)
#define VMCE_GUEST_DR7		VMCS_ENCODE_N_GUEST(_VMCE_GUEST_DR7_IDX)
#define VMCE_GUEST_RSP		VMCS_ENCODE_N_GUEST(_VMCE_GUEST_RSP_IDX)
#define VMCE_GUEST_RIP		VMCS_ENCODE_N_GUEST(_VMCE_GUEST_RIP_IDX)
#define VMCE_GUEST_RFLAGS	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_RFLAGS_IDX)
#define VMCE_GUEST_PENDING_DEBUG_EXCEPTIONS \
	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_PENDING_DEBUG_EXCEPTIONS_IDX)
#define VMCE_GUEST_IA32_SYSENTER_ESP \
	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_IA32_SYSENTER_ESP_IDX)
#define VMCE_GUEST_IA32_SYSENTER_EIP \
	VMCS_ENCODE_N_GUEST(_VMCE_GUEST_IA32_SYSENTER_EIP_IDX)

#define VMCS_ENCODE_N_HOST(index)	VMCS_ENCODE(_VMCE_WIDTH_N, \
						    _VMCE_TYPE_HOST, \
						    (index), \
						    _VMCE_ACCESS_FULL)

#define _VMCE_HOST_CR0_IDX			0
#define _VMCE_HOST_CR3_IDX			1
#define _VMCE_HOST_CR4_IDX			2
#define _VMCE_HOST_FS_BASE_IDX			3
#define _VMCE_HOST_GS_BASE_IDX			4
#define _VMCE_HOST_TR_BASE_IDX			5
#define _VMCE_HOST_GDTR_BASE_IDX		6
#define _VMCE_HOST_IDTR_BASE_IDX		7
#define _VMCE_HOST_IA32_SYSENTER_ESP_IDX	8
#define _VMCE_HOST_IA32_SYSENTER_EIP_IDX	9
#define _VMCE_HOST_RSP_IDX			10
#define _VMCE_HOST_RIP_IDX			11

#define VMCE_HOST_CR0		VMCS_ENCODE_N_HOST(_VMCE_HOST_CR0_IDX)
#define VMCE_HOST_CR3		VMCS_ENCODE_N_HOST(_VMCE_HOST_CR3_IDX)
#define VMCE_HOST_CR4		VMCS_ENCODE_N_HOST(_VMCE_HOST_CR4_IDX)
#define VMCE_HOST_FS_BASE	VMCS_ENCODE_N_HOST(_VMCE_HOST_FS_BASE_IDX)
#define VMCE_HOST_GS_BASE	VMCS_ENCODE_N_HOST(_VMCE_HOST_GS_BASE_IDX)
#define VMCE_HOST_TR_BASE	VMCS_ENCODE_N_HOST(_VMCE_HOST_TR_BASE_IDX)
#define VMCE_HOST_GDTR_BASE	VMCS_ENCODE_N_HOST(_VMCE_HOST_GDTR_BASE_IDX)
#define VMCE_HOST_IDTR_BASE	VMCS_ENCODE_N_HOST(_VMCE_HOST_IDTR_BASE_IDX)
#define VMCE_HOST_IA32_SYSENTER_ESP \
	VMCS_ENCODE_N_HOST(_VMCE_HOST_IA32_SYSENTER_ESP_IDX)
#define VMCE_HOST_IA32_SYSENTER_EIP \
	VMCS_ENCODE_N_HOST(_VMCE_HOST_IA32_SYSENTER_EIP_IDX)
#define VMCE_HOST_RSP		VMCS_ENCODE_N_HOST(_VMCE_HOST_RSP_IDX)
#define VMCE_HOST_RIP		VMCS_ENCODE_N_HOST(_VMCE_HOST_RIP_IDX)

#ifdef DUMP_ENCODINGS

#define pg(x) printf("%-40s 0x%08x\n", \
		     "VMCE_GUEST_" #x "_SEL", \
                     VMCE_GUEST_ ## x ## _SEL)

#define ph(x) printf("%-40s 0x%08x\n", \
		     "VMCE_HOST_" #x "_SEL", \
                     VMCE_HOST_ ## x ## _SEL)

#define p(x)	printf("%-40s 0x%08x\n", #x, x)
#define n(x)	printf("%-40s 0x%08x NEW\n", #x, x)

inline static void
dump_encodings()
{
	n(VMCE_VPID);

	printf("\n");

	pg(ES);
	pg(CS);
	pg(SS);
	pg(DS);
	pg(FS);
	pg(GS);
	pg(LDTR);
	pg(TR);

	printf("\n");

	ph(ES);
	ph(CS);
	ph(SS);
	ph(DS);
	ph(FS);
	ph(GS);
	ph(TR);

	printf("\n");

	p(VMCE_ADDR_IO_BITMAP_A);
	p(VMCE_ADDR_IO_BITMAP_B);
	p(VMCE_ADDR_MSR_BITMAPS);
	p(VMCE_VMEXIT_MSR_STORE_ADDR);
	p(VMCE_VMEXIT_MSR_LOAD_ADDR);
	p(VMCE_VMENTRY_MSR_LOAD_ADDR);
	p(VMCE_EXECUTIVE_VMCS_PTR);
	p(VMCE_TSC_OFFSET);
	p(VMCE_VIRTUAL_APIC_ADDR);
	p(VMCE_APIC_ACCESS_ADDR);
	n(VMCE_EPT_PTR);

	printf("\n");

	n(VMCE_GUEST_PHYS_ADDR);

	printf("\n");

	p(VMCE_VMCS_LINK_PTR);
	p(VMCE_GUEST_IA32_DEBUGCTL);
	n(VMCE_GUEST_IA32_PAT);
	n(VMCE_GUEST_IA32_EFER);
	p(VMCE_GUEST_IA32_PERF_GLOBAL_CTRL);
	n(VMCE_GUEST_PDPTE0);
	n(VMCE_GUEST_PDPTE1);
	n(VMCE_GUEST_PDPTE2);
	n(VMCE_GUEST_PDPTE3);

	printf("\n");

	p(VMCE_HOST_IA32_PERF_GLOBAL_CTRL);

	printf("\n");

	p(VMCE_PIN_BASED_CONTROLS);
	p(VMCE_PRIMARY_CPU_BASED_CONTROLS);
	p(VMCE_EXCEPTION_BITMAP);
	p(VMCE_PAGE_FAULT_ERROR_CODE_MASK);
	p(VMCE_PAGE_FAULT_ERROR_CODE_MATCH);
	p(VMCE_CR3_TARGET_COUNT);
	p(VMCE_VMEXIT_CONTROLS);
	p(VMCE_VMEXIT_MSR_STORE_COUNT);
	p(VMCE_VMEXIT_MSR_LOAD_COUNT);
	p(VMCE_VMENTRY_CONTROLS);
	p(VMCE_VMENTRY_MSR_LOAD_COUNT);
	p(VMCE_VMENTRY_INTERRUPT_INFO_FIELD);
	p(VMCE_VMENTRY_EXCEPTION_ERROR_CODE);
	p(VMCE_VMENTRY_INSTRUCTION_LENGTH);
	p(VMCE_TPR_THRESHOLD);
	p(VMCE_SECONDARY_CPU_BASED_CONTROLS);

	printf("\n");

	p(VMCE_VM_INSTRUCTION_ERROR);
	p(VMCE_EXIT_REASON);
	p(VMCE_VMEXIT_INTERRUPT_INFO);
	p(VMCE_VMEXIT_INTERRUPT_ERROR_CODE);
	p(VMCE_IDT_VECTORING_INFO_FIELD);
	p(VMCE_IDT_VECTORING_ERROR_CODE);
	p(VMCE_VMEXIT_INSTRUCTION_LENGTH);
	p(VMCE_VMEXIT_INSTRUCTION_INFO);

	printf("\n");

	p(VMCE_GUEST_ES_LIMIT);
	p(VMCE_GUEST_CS_LIMIT);
	p(VMCE_GUEST_SS_LIMIT);
	p(VMCE_GUEST_DS_LIMIT);
	p(VMCE_GUEST_FS_LIMIT);
	p(VMCE_GUEST_GS_LIMIT);
	p(VMCE_GUEST_LDTR_LIMIT);
	p(VMCE_GUEST_TR_LIMIT);
	p(VMCE_GUEST_GDTR_LIMIT);
	p(VMCE_GUEST_IDTR_LIMIT);
	p(VMCE_GUEST_ES_ACCESS_RIGHTS);
	p(VMCE_GUEST_CS_ACCESS_RIGHTS);
	p(VMCE_GUEST_SS_ACCESS_RIGHTS);
	p(VMCE_GUEST_DS_ACCESS_RIGHTS);
	p(VMCE_GUEST_FS_ACCESS_RIGHTS);
	p(VMCE_GUEST_GS_ACCESS_RIGHTS);
	p(VMCE_GUEST_LDTR_ACCESS_RIGHTS);
	p(VMCE_GUEST_TR_ACCESS_RIGHTS);
	p(VMCE_GUEST_INTERRUPTIBILITY_STATE);
	p(VMCE_GUEST_ACTIVITY_STATE);
	p(VMCE_GUEST_SMBASE);
	p(VMCE_GUEST_IA32_SYSENTER_CS);
	n(VMCE_VMX_PREEMPTION_TIMER_VALUE);

	printf("\n");

	p(VMCE_HOST_IA32_SYSENTER_CS);

	printf("\n");

	p(VMCE_CR0_GUEST_HOST_MASK);
	p(VMCE_CR4_GUEST_HOST_MASK);
	p(VMCE_CR0_READ_SHADOW);
	p(VMCE_CR4_READ_SHADOW);
	p(VMCE_CR3_TARGET_VALUE_0);
	p(VMCE_CR3_TARGET_VALUE_1);
	p(VMCE_CR3_TARGET_VALUE_2);
	p(VMCE_CR3_TARGET_VALUE_3);

	printf("\n");

	p(VMCE_EXIT_QUALIFICATION);
	p(VMCE_IO_RCX);
	p(VMCE_IO_RSI);
	p(VMCE_IO_RDI);
	p(VMCE_IO_RIP);
	p(VMCE_GUEST_LINEAR_ADDR);

	printf("\n");

	p(VMCE_GUEST_CR0);
	p(VMCE_GUEST_CR3);
	p(VMCE_GUEST_CR4);
	p(VMCE_GUEST_ES_BASE);
	p(VMCE_GUEST_CS_BASE);
	p(VMCE_GUEST_SS_BASE);
	p(VMCE_GUEST_DS_BASE);
	p(VMCE_GUEST_FS_BASE);
	p(VMCE_GUEST_GS_BASE);
	p(VMCE_GUEST_LDTR_BASE);
	p(VMCE_GUEST_TR_BASE);
	p(VMCE_GUEST_GDTR_BASE);
	p(VMCE_GUEST_IDTR_BASE);
	p(VMCE_GUEST_DR7);
	p(VMCE_GUEST_RSP);
	p(VMCE_GUEST_RIP);
	p(VMCE_GUEST_RFLAGS);
	p(VMCE_GUEST_PENDING_DEBUG_EXCEPTIONS);
	p(VMCE_GUEST_IA32_SYSENTER_ESP);
	p(VMCE_GUEST_IA32_SYSENTER_EIP);

	printf("\n");

	p(VMCE_HOST_CR0);
	p(VMCE_HOST_CR3);
	p(VMCE_HOST_CR4);
	p(VMCE_HOST_FS_BASE);
	p(VMCE_HOST_GS_BASE);
	p(VMCE_HOST_TR_BASE);
	p(VMCE_HOST_GDTR_BASE);
	p(VMCE_HOST_IDTR_BASE);
	p(VMCE_HOST_IA32_SYSENTER_ESP);
	p(VMCE_HOST_IA32_SYSENTER_EIP);
	p(VMCE_HOST_RSP);
	p(VMCE_HOST_RIP);
}

#endif

#endif
