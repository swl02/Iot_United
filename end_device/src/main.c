// /*
//  * Copyright (c) 2020 Nordic Semiconductor ASA
//  *
//  * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
//  */

#include <zephyr.h>
#include <logging/log.h>
#include <net/openthread.h>
#include <net/socket.h>
#include <net/mqtt.h>
#include <random/rand32.h>
#include <string.h>
#include <errno.h>

#if defined(CONFIG_NET_L2_OPENTHREAD)
static struct openthread_context *ot_context;
#endif

#define CHILD_TIMEOUT 5

static otDeviceRole role;

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

struct k_timer my_timer;

/* sending data to backend server every 2 seconds */
void timer_handler();

static void update_role(void)
{
	role = otThreadGetDeviceRole(ot_context->instance);
	printk("OT role: %u", role);

	/* Device connected */
	if (role != OT_DEVICE_ROLE_DISABLED && role != OT_DEVICE_ROLE_DETACHED)
		return;

	/* Device disconnected */
	printk("OT disconnected!");
}

static void change_cb(otChangedFlags aFlags, void *aContext)
{
	/* Ignore if no role change */
	if ((aFlags & OT_CHANGED_THREAD_ROLE) == false)
		return;

	printk("OT Role changed");
	update_role();
}

int ot_config_init(void)
{
	struct net_if *iface;

	/* Load interface and context */
	LOG_DBG("Initializing OpenThread handler");
	iface = net_if_get_default();
	if (iface == NULL) {
		printk("Failed to get net interface");
		return -1;
	}

	ot_context = net_if_l2_data(iface);
	if (ot_context == NULL) {
		printk("Failed to get OT context");
		return -1;
	}

	return 0;
}

int ot_config_stop(void)
{
	int rc;

	/* Disable OpenThread service */
	LOG_DBG("Stopping OpenThread service");

	rc = otThreadSetEnabled(ot_context->instance, false);
	if (rc) {
		LOG_ERR("Failed to stop Thread protocol. (err %d)", rc);
		return rc;
	}

	rc = otIp6SetEnabled(ot_context->instance, false);
	if (rc)
		LOG_ERR("Failed to disable IPv6 communication. (err %d)", rc);

	return rc;
}


// start the thread when join successfully
otJoinerCallback HandleJoinerCallback(otError aError, void *aContext) {

	// if (aError == OT_ERROR_NONE) {
	// 	printk("successful\n");
	// 	//set up thread
	// 	aError = otThreadSetEnabled(instance, true);
	// }
}

int ot_config_start(void)
{
	int rc;

	/* Enable OpenThread service */
	printk("Starting OpenThread service");

	rc = otIp6SetEnabled(ot_context->instance, true);
	if (rc) {
		printk("Failed to enable IPv6 communication. (err %d)", rc);
		return rc;
	}

	rc = otThreadSetEnabled(ot_context->instance, true);
	if (rc)
		printk("Failed to start Thread protocol. (err %d)", rc);

	return rc;
}

K_TIMER_DEFINE(my_timer,timer_handler,NULL);

void main(void)
{

	//initialising
	ot_config_init();

	otError error;
	error = otIp6SetEnabled(ot_context->instance, true);
	error = otThreadSetRouterEligible(ot_context->instance,false);

	k_timer_start(&my_timer, K_SECONDS(2), K_SECONDS(2));
}

void timer_handler() {
	// //check state of thread
	otDeviceRole role = otThreadGetDeviceRole(ot_context->instance);
	otRouterInfo parentInfo;
	otError error;
	
	// storing rssi value
	int8_t aParentRssi;

	// // all child will send to server
	if (role == OT_DEVICE_ROLE_CHILD) {
		//rssi
		if (otThreadGetParentInfo(ot_context->instance, &parentInfo) == OT_ERROR_NONE && parentInfo.mLinkEstablished == true) {
			error = otThreadGetParentAverageRssi(ot_context->instance,&aParentRssi);
			printk("here\n");
		}

	} 

}







