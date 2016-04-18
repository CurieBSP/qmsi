/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "qm_aon_counters.h"
#include "qm_interrupt.h"

/* QMSI Always-on Counters app example */

/* Example callback function */
static void aonpt_example_callback();

int main(void)
{
	volatile uint32_t delay;
	uint32_t c_val = 0, pt_val = 0;
	qm_aonpt_config_t cfg;

	QM_PUTS("\nAlways-on Counter example app\n");

	/* Always-on Counter enable, disable and read value */
	qm_aonc_disable(QM_SCSS_AON_0);
	qm_aonc_enable(QM_SCSS_AON_0);

	c_val = qm_aonc_get_value(QM_SCSS_AON_0);
	if (c_val) {
		QM_PRINTF("Always-on Counter value: %u\n", c_val);
	} else {
		QM_PRINTF("ERROR: Could not read aonc value\n");
	}

	/* Request an IRQ and write the Always-on Periodic Timer config */
	cfg.count = 0x10000;
	cfg.int_en = true;
	cfg.callback = aonpt_example_callback;

	qm_irq_request(QM_IRQ_AONPT_0, qm_aonpt_isr_0);

	qm_aonpt_set_config(QM_SCSS_AON_0, &cfg);

	/* The AON Periodic Timer runs from the RTC clock at 32KHz (rather than
	 * the system clock which is 32MHz) so we need to spin for a few cycles
	 * allow the register change to propagate */
	for (delay = 500; delay--;) {
	}

	/* Get the value of the Always-on Periodic Timer */
	pt_val = qm_aonpt_get_value(QM_SCSS_AON_0);
	if (pt_val) {
		QM_PRINTF("Always-on Periodic Timer value: %u\n", pt_val);
	} else {
		QM_PRINTF("ERROR: Could not read Periodic timer value\n\n");
	}

	QM_PUTS("Always-on counter example app complete\n");
	return 0;
}

void aonpt_example_callback()
{
	bool pt_status = false;
	QM_PUTS("Periodic Timer callback\n");

	/* Clear the timer so it can fire again */
	pt_status = qm_aonpt_get_status(QM_SCSS_AON_0);
	if (pt_status == true) {
		qm_aonpt_clear(QM_SCSS_AON_0);
	}
}
