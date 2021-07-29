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

// LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

struct k_timer my_timer;

/* sending data to backend server every 2 seconds */
void timer_handler();

/* debugging */
void print_dataset();

/* setting mode for router */
void set_mode();

uint32_t DefaultJoinerTimeout = 120; ///< Default timeout for Joiners, in seconds.

/* define an instance of thread */
otInstance *instance;

/* set up for dataset */
otOperationalDataset Dataset;	

K_TIMER_DEFINE(my_timer,timer_handler,NULL);

otCommissionerStateCallback commission_handler(otCommissionerState aState, void *aContext) {
	if (aState == OT_COMMISSIONER_STATE_ACTIVE) {
		printk("successful\n");
	} else {
		printk("patience\n");
	}

}



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

	//set mode
	set_mode();

	//initialise dataset
	err = otDatasetCreateNewNetwork(instance, &Dataset);

	//commit dataset
	err = otDatasetSetActive(instance, &Dataset);
	
	if (err == OT_ERROR_NONE) {
		printk("successful \n");
	}
	
	//set up ifconfig	
	err = otIp6SetEnabled(instance, true);

	//set up thread
	err = otThreadSetEnabled(instance, true);

	// //manually do in-band commissioning
	// while (otCommissionerStart(instance,NULL,NULL,NULL) != OT_ERROR_NONE) {	
	// 	err = otCommissionerStop(instance);
	// }

	// //commit dataset
	// err = otDatasetSetActive(instance, &Dataset);

	
	// if (OT_COMMISSIONER_STATE_ACTIVE != otCommissionerGetState(instance)) {
	// 	printk("why \n");
	// }



	//start the timer for 2 second
	// k_timer_start(&my_timer, K_SECONDS(10), K_SECONDS(10));

}

void timer_handler() {
	//print_dataset();
}



void print_dataset() {
   if (Dataset.mComponents.mIsPendingTimestampPresent)
    {
        printk("Pending Timestamp: %lu", Dataset.mPendingTimestamp);
    }

    if (Dataset.mComponents.mIsActiveTimestampPresent)
    {
        printk("Active Timestamp: %lu", Dataset.mActiveTimestamp);
    }

    if (Dataset.mComponents.mIsChannelPresent)
    {
        printk("Channel: %d", Dataset.mChannel);
    }

    if (Dataset.mComponents.mIsChannelMaskPresent)
    {
        printk("Channel Mask: 0x%08x", Dataset.mChannelMask);
    }

    if (Dataset.mComponents.mIsDelayPresent)
    {
        printk("Delay: %d", Dataset.mDelay);
    }

    if (Dataset.mComponents.mIsMasterKeyPresent)
    {
        printk("Master Key: ");
        printk("%x\n",Dataset.mMasterKey.m8);
        printk("\n");
    }

    if (Dataset.mComponents.mIsNetworkNamePresent)
    {
        printk("Network Name: ");
        printk("%s", Dataset.mNetworkName.m8);
    }

    if (Dataset.mComponents.mIsPanIdPresent)
    {
        printk("PAN ID: 0x%04x", Dataset.mPanId);
    }

}

void set_mode() {
	otLinkModeConfig linkMode;

	/* rdn */
	linkMode.mRxOnWhenIdle = true;
	linkMode.mDeviceType = true;
	linkMode.mNetworkData = true;

	otThreadSetLinkMode(instance,linkMode);
}






