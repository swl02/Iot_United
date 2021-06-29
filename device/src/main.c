/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr.h>
#include <logging/log.h>
#include <net/openthread.h>
#include <openthread/instance.h>
#include <openthread/joiner.h>


// #if CONFIG_BT
// #include "ble.h"
// #endif

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

#define WELLCOME_TEXT \
	"\n\r"\
	"\n\r"\
	"OpenThread Command Line Interface is now running.\n\r" \
	"Use the 'ot' keyword to invoke OpenThread commands e.g. " \
	"'ot thread start.'\n\r" \
	"For the full commands list refer to the OpenThread CLI " \
	"documentation at:\n\r" \
	"https://github.com/openthread/openthread/blob/master/src/cli/README.md\n\r"


struct k_timer my_timer;

//sending data to backend server every 2 seconds
void timer_handler();

//define a timer for sending Mesh-Local EID to broaker
K_TIMER_DEFINE(my_timer, timer_handler, NULL);



//define an instance of thread
//very bad for memory management
otInstance *instance;

void main(void)
{

#if OPENTHREAD_ENABLE_MULTIPLE_INSTANCES

	otError error;

	// Call to query the buffer size
	(void)otInstanceInit(NULL, &otInstanceBufferLength);

	// Call to allocate the buffer
	otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
	assert(otInstanceBuffer);
	// Initialize OpenThread with the buffer
	instance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else
	instance = otInstanceInitSingle();
#endif	

	//start the timer for 2 second
	k_timer_start(&my_timer, K_SECONDS(2), K_SECONDS(2));

}

void timer_handler() {
	//check state of thread
	otDeviceRole role = otThreadGetDeviceRole(instance);
	otRouterInfo parentInfo;
	otError error;
	
	//storing rssi value
	int8_t aLastRssi;
	int8_t aParentRssi;


	//all child will send to server
	if (role == OT_DEVICE_ROLE_CHILD) {
		printk("life sucks\n");

		if (otThreadGetParentInfo(instance, &parentInfo) == OT_ERROR_NONE && parentInfo.mLinkEstablished == true) {
			printk("might be correct\n");
			//link quality conversion to rssi
			int8_t link_in_rssi = otLinkConvertLinkQualityToRss(instance,parentInfo.mLinkQualityIn);
			int8_t link_out_rssi = otLinkConvertLinkQualityToRss(instance,parentInfo.mLinkQualityOut);
			printk("link in %d\n",link_in_rssi);
			printk("link out %d\n",link_out_rssi);

			//rssi value obtained directly
			error = otThreadGetParentLastRssi(instance,&aLastRssi);
			error = otThreadGetParentAverageRssi(instance,&aParentRssi);
			printk("last rssi %d\n",aLastRssi);
			printk("average rssi %d\n",aParentRssi);


			printk("path cost %d\n",parentInfo.mRloc16);
			printk("router identifier %x\n",parentInfo.mRloc16);
		
		} 
	}

}




