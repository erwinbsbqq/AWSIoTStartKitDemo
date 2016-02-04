/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file subscribe_publish_sample.c
 * @brief simple MQTT publish and subscribe on the same topic
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
 * It subscribes and publishes to the same topic - "sdkTest/sub"
 *
 * If all the certs are correct, you should see the messages received by the application in a loop.
 *
 * The application takes in the certificate path, host name , port and the number of times the publish should happen.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include <signal.h>
#include <memory.h>
#include <sys/time.h>
#include <limits.h>

#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_interface.h"
#include "aws_iot_config.h"
#include "event_gpio.h"

enum{
	gpioUserKey1 = 15,
	gpioUserKey2 = 28,	
	gpioYellowLed = 84,
	gpioRedLed = 83,
	gpioGreenLed = 82,
	gpioOrangeLed = 76,
	gpioWhiteLed = 78,
	gpioBlueLed = 80,
	gpioPurpleLed = 81,
	gpioPinkLed = 79
};

unsigned int leds[] = {gpioYellowLed, gpioRedLed, gpioGreenLed, gpioOrangeLed, gpioWhiteLed, gpioBlueLed, gpioPurpleLed, gpioPinkLed}; 

int lastled = 0;
int lednum = 0;

int MQTTcallbackHandler(MQTTCallbackParams params) {

	INFO("Subscribe callback");
	INFO("%.*s\t%.*s",
			(int)params.TopicNameLen, params.pTopicName,
			(int)params.MessageParams.PayloadLen, (char*)params.MessageParams.pPayload);

	char* str = (char*)params.MessageParams.pPayload;
	unsigned int curled = leds[lastled];
	unsigned int nextled = -1;
	
	if(strncmp(str, "KEY1", 4) == 0)
	{
		lastled = (lastled+1)%lednum;
		nextled = leds[lastled];
	}
	else if (strncmp(str, "KEY2", 4) == 0)
	{
		lastled = (lastled+lednum-1)%lednum;
		nextled = leds[lastled];
	}
	else
	{
		INFO("unknown msg!");
	}

	if(nextled != -1)
	{
		gpio_export(curled);
		gpio_set_direction(curled, OUTPUT);
		gpio_set_value(curled, HIGH);
		gpio_export(nextled);
		gpio_set_direction(nextled, OUTPUT);
		gpio_set_value(nextled, LOW);
		INFO("switch to led %d [GPIO %d]\n", lastled, nextled);
	}
	
	return 0;
}

void disconnectCallbackHandler(void) {
	WARN("MQTT Disconnect");
}

/**
 * @brief Default cert location
 */
char certDirectory[PATH_MAX + 1] = "../../certs";

/**
 * @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
 */
char HostAddress[255] = AWS_IOT_MQTT_HOST;

/**
 * @brief Default MQTT port is pulled from the aws_iot_config.h
 */
uint32_t port = AWS_IOT_MQTT_PORT;

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */
uint32_t publishCount = 0;

void parseInputArgsForConnectParams(int argc, char** argv) {
	int opt;

	while (-1 != (opt = getopt(argc, argv, "h:p:c:x:"))) {
		switch (opt) {
		case 'h':
			strcpy(HostAddress, optarg);
			DEBUG("Host %s", optarg);
			break;
		case 'p':
			port = atoi(optarg);
			DEBUG("arg %s", optarg);
			break;
		case 'c':
			strcpy(certDirectory, optarg);
			DEBUG("cert root directory %s", optarg);
			break;
		case 'x':
			publishCount = atoi(optarg);
			DEBUG("publish %s times\n", optarg);
			break;
		case '?':
			if (optopt == 'c') {
				ERROR("Option -%c requires an argument.", optopt);
			}
			else if (isprint(optopt)) {
				WARN("Unknown option `-%c'.", optopt);
			}
			else {
				WARN("Unknown option character `\\x%x'.", optopt);
			}
			break;
		default:
			ERROR("Error in command line argument parsing");
			break;
		}
	}

}

int main(int argc, char** argv) {
	IoT_Error_t rc = NONE_ERROR;
	int32_t i = 0, j = 0, k = 0;
	bool infinitePublishFlag = true;

	char rootCA[PATH_MAX + 1];
	char clientCRT[PATH_MAX + 1];
	char clientKey[PATH_MAX + 1];
	char CurrentWD[PATH_MAX + 1];
	char cafileName[] = AWS_IOT_ROOT_CA_FILENAME;
	char clientCRTName[] = AWS_IOT_CERTIFICATE_FILENAME;
	char clientKeyName[] = AWS_IOT_PRIVATE_KEY_FILENAME;
	unsigned int value1, lastValue1 = HIGH, value2, lastValue2 = HIGH;

	parseInputArgsForConnectParams(argc, argv);

	lednum = ARRAY_SIZE(leds);
	for(k=0; k<lednum; k++)
	{
		gpio_export(leds[k]);
		gpio_set_direction(leds[k], OUTPUT);
		gpio_set_value(leds[k], HIGH);
	}
	gpio_export(leds[0]);
	gpio_set_direction(leds[0], OUTPUT);
	gpio_set_value(leds[0], LOW);

	INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);


	getcwd(CurrentWD, sizeof(CurrentWD));
	sprintf(rootCA, "%s/%s/%s", CurrentWD, certDirectory, cafileName);
	sprintf(clientCRT, "%s/%s/%s", CurrentWD, certDirectory, clientCRTName);
	sprintf(clientKey, "%s/%s/%s", CurrentWD, certDirectory, clientKeyName);

	DEBUG("rootCA %s", rootCA);
	DEBUG("clientCRT %s", clientCRT);
	DEBUG("clientKey %s", clientKey);

	MQTTConnectParams connectParams = MQTTConnectParamsDefault;

	connectParams.KeepAliveInterval_sec = 10;
	connectParams.isCleansession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = "CSDK-test-device";
	connectParams.pHostURL = HostAddress;
	connectParams.port = port;
	connectParams.isWillMsgPresent = false;
	connectParams.pRootCALocation = rootCA;
	connectParams.pDeviceCertLocation = clientCRT;
	connectParams.pDevicePrivateKeyLocation = clientKey;
	connectParams.mqttCommandTimeout_ms = 2000;
	connectParams.tlsHandshakeTimeout_ms = 5000;
	connectParams.isSSLHostnameVerify = true;// ensure this is set to true for production
	connectParams.disconnectHandler = disconnectCallbackHandler;

	INFO("Connecting...");
	rc = aws_iot_mqtt_connect(&connectParams);
	if (NONE_ERROR != rc) {
		ERROR("Error(%d) connecting to %s:%d", rc, connectParams.pHostURL, connectParams.port);
	}

	MQTTSubscribeParams subParams = MQTTSubscribeParamsDefault;
	subParams.mHandler = MQTTcallbackHandler;
	subParams.pTopic = "sdkTest/sub";
	subParams.qos = QOS_0;

	if (NONE_ERROR == rc) {
		INFO("Subscribing...");
		rc = aws_iot_mqtt_subscribe(&subParams);
		if (NONE_ERROR != rc) {
			ERROR("Error subscribing");
		}
	}

	MQTTMessageParams Msg = MQTTMessageParamsDefault;
	Msg.qos = QOS_0;
	char cPayload[100];
	sprintf(cPayload, "%s : %d ", "hello from SDK", i);
	Msg.pPayload = (void *) cPayload;


	MQTTPublishParams Params = MQTTPublishParamsDefault;
	Params.pTopic = "sdkTest/sub";


	if(publishCount != 0){
		infinitePublishFlag = false;
	}

	while (NONE_ERROR == rc && (publishCount > 0 || infinitePublishFlag)) {

		//Max time the yield function will wait for read messages
		rc = aws_iot_mqtt_yield(100);
		//INFO("-->sleep");
		sleep(1);

		gpio_export(gpioUserKey1);
		gpio_set_direction(gpioUserKey1, INPUT);
		gpio_get_value(gpioUserKey1, &value1);
		//INFO("UserKey1 value %d\n", value1);
		if((value1==LOW) && (lastValue1!=value1)) // one switch
		{
			sprintf(cPayload, "%s:%d", "KEY1", i++);			
			Msg.PayloadLen = strlen(cPayload) + 1;
			Params.MessageParams = Msg;
			INFO("publish %s\n", cPayload);
			rc = aws_iot_mqtt_publish(&Params);
			if(publishCount > 0){
				publishCount--;
			}
		}
		lastValue1 = value1;

		gpio_export(gpioUserKey2);
		gpio_set_direction(gpioUserKey2, INPUT);
		gpio_get_value(gpioUserKey2, &value2);
		//INFO("UserKey2 value %d\n", value2);
		if((value2==LOW) && (lastValue2!=value2)) // one switch
		{
			sprintf(cPayload, "%s:%d", "KEY2", j++);			
			Msg.PayloadLen = strlen(cPayload) + 1;
			Params.MessageParams = Msg;
			INFO("publish %s\n", cPayload);
			rc = aws_iot_mqtt_publish(&Params);
			if(publishCount > 0){
				publishCount--;
			}
		}
		lastValue2 = value2;		
	}

	if(NONE_ERROR != rc){
		ERROR("An error occurred in the loop.\n");
	}
	else{
		INFO("Publish done\n");
	}

	return rc;
}

