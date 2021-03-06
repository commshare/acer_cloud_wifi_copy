#ifndef __VMXDUMP_H__
#define __VMXDUMP_H__

typedef struct {
	u8	n;
	char	*name;
} bitname_t;

#define N(n)	{ n, #n }

bitname_t VMCE_PIN_BASED_CONTROLS_BITS[] = {
	N(VMEXEC_PIN_EXT_INTR_EXIT),
	N(VMEXEC_PIN_NMI_EXIT),
	N(VMEXEC_PIN_VIRTUAL_NMIS),
	N(VMEXEC_PIN_PREEMPTION_TIMER),
	N(99)
};

bitname_t VMCE_PRIMARY_CPU_BASED_CONTROLS_BITS[] = {
	N(VMEXEC_CPU1_INTR_WINDOW_EXIT),
	N(VMEXEC_CPU1_USE_TSC_OFFSET),
	N(VMEXEC_CPU1_HLT_EXIT),
	N(VMEXEC_CPU1_INVLPG_EXIT),
	N(VMEXEC_CPU1_MWAIT_EXIT),
	N(VMEXEC_CPU1_RDPMC_EXIT),
	N(VMEXEC_CPU1_RDTSC_EXIT),
	N(VMEXEC_CPU1_CR3_LOAD_EXIT),
	N(VMEXEC_CPU1_CR3_STORE_EXIT),
	N(VMEXEC_CPU1_CR8_LOAD_EXIT),
	N(VMEXEC_CPU1_CR8_STORE_EXIT),
	N(VMEXEC_CPU1_USE_TPR_SHADOW),
	N(VMEXEC_CPU1_NMI_WINDOW_EXIT),
	N(VMEXEC_CPU1_MOV_DR_EXIT),
	N(VMEXEC_CPU1_IO_EXIT),
	N(VMEXEC_CPU1_USE_IO_BITMAPS),
	N(VMEXEC_CPU1_MONITOR_TRAP_FLAG),
	N(VMEXEC_CPU1_USE_MSR_BITMAPS),
	N(VMEXEC_CPU1_MONITOR_EXIT),
	N(VMEXEC_CPU1_PAUSE_EXIT),
	N(VMEXEC_CPU1_ACTIVATE_SECONDARY_CTRLS),
	N(99)
};

bitname_t VMCE_SECONDARY_CPU_BASED_CONTROLS_BITS[] = {
	N(VMEXEC_CPU2_VIRTUALIZE_APIC),
	N(VMEXEC_CPU2_ENABLE_EPT),
	N(VMEXEC_CPU2_DESCRIPTOR_TABLE_EXIT),
	N(VMEXEC_CPU2_ENABLE_RDTSCP),
	N(VMEXEC_CPU2_VIRTUALIZE_x2APIC),
	N(VMEXEC_CPU2_ENABLE_VPID),
	N(VMEXEC_CPU2_WBINVD_EXIT),
	N(VMEXEC_CPU2_UNRESTRICTED_GUEST),
	N(VMEXEC_CPU2_PAUSE_LOOP_EXIT),
	N(99)
};

bitname_t VMCE_VMEXIT_CONTROLS_BITS[] = {
	N(VMEXIT_CTL_SAVE_DEBUG_CTLS),
	N(VMEXIT_CTL_HOST_64),
	N(VMEXIT_CTL_LOAD_MSR_IA32_PERF_GLOBAL_CTRL),
	N(VMEXIT_CTL_ACK_INTERRUPT),
	N(VMEXIT_CTL_SAVE_IA32_PAT),
	N(VMEXIT_CTL_LOAD_IA32_PAT),
	N(VMEXIT_CTL_SAVE_IA32_EFER),
	N(VMEXIT_CTL_LOAD_IA32_EFER),
	N(VMEXIT_CTL_SAVE_PREEMPTION_TIMER),
	N(99)
};

bitname_t VMCE_VMENTRY_CONTROLS_BITS[] = {
	N(VMENTRY_CTL_LOAD_DEBUG_CTLS),
	N(VMENTRY_CTL_GUEST_64),
	N(VMENTRY_CTL_SMM),
	N(VMENTRY_CTL_NOT_DUAL_MONITOR),
	N(VMENTRY_CTL_LOAD_MSR_IA32_PERF_GLOBAL_CTRL),
	N(VMENTRY_CTL_LOAD_IA32_PAT),
	N(VMENTRY_CTL_LOAD_IA32_EFER),
	N(99)
};

#define vmcs_check(x) \
({ \
	if (!(x)) { \
		kprintf("check failed [%d]: %s\n", \
			__LINE__, #x); \
	} \
})

#define seg_check(name, x) \
({ \
	if (!(x)) { \
		kprintf("check failed [%d](%s): %s\n", \
		       	__LINE__, name, #x); \
	} \
})

#endif
