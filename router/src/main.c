// /*
//  * Copyright (c) 2020 Nordic Semiconductor ASA
//  *
//  * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
//  */

#include <zephyr.h>
#include <logging/log.h>
#include <net/openthread.h>
#include <openthread/instance.h>
#include <openthread/joiner.h>
#include <net/socket.h>
#include <net/mqtt.h>
#include <random/rand32.h>
#include <string.h>
#include <errno.h>
#include "config.h"
#include <openthread/dataset.h>

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

struct k_timer my_timer;

/* sending data to backend server every 2 seconds */
void timer_handler();


/* define an instance of thread */
otInstance *instance;

void main(void)
{

/* Setting up for OPENTHREAD*/
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

	//Error
	otError err;

	//set up for router
	otOperationalDataset *Dataset = malloc(sizeof(otOperationalDataset));	

	//1. initialise dataset
	err = otDatasetCreateNewNetwork(instance, Dataset);

	//commit dataset
	err = otDatasetSetActive(instance, Dataset);
	
	//2. hardcode a masterkey/networkkey 537df8da171e038eb120bc98b7f790c0
	err = otThreadSetMasterKey(instance,"537df8da171e038eb120bc98b7f790c0");

	//commit dataset
	err = otDatasetSetActive(instance, Dataset);
	
	//3. set up ifconfig	
	err = otIp6SetEnabled(instance, true);

	//4. set up thread
	err = otThreadSetEnabled(instance, true);

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
		if (otThreadGetParentInfo(instance, &parentInfo) == OT_ERROR_NONE && parentInfo.mLinkEstablished == true) {
			//rssi value obtained directly
			// error = otThreadGetParentLastRssi(instance,&aLastRssi);
			// error = otThreadGetParentAverageRssi(instance,&aParentRssi);


		} 
	}

}






