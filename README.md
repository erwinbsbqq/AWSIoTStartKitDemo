GoWarrior AWS IoT StartKit
===================================
GoWarrior TigerBoard AWS IoT StartKit enables users to easily prototype cloud-connected IoT designs. It provides embedded developers a way to quickly develop internet connected embedded products that interface with peripherals over the UART, I2C, SPI, GPIO, etc, and talk to the AWS IoT service.   
    
TigerBoard has 1Ghz dual-core Cortex-A9, 4GB nand flash, 1GB DDR, Wi-Fi, Ethernet, and HDMI display.    
The StartKit has 2 demo applications of LED switcher using 8 onboard leds and 2 onboard user keys on GoWarrior GoBian platform located in the apps folder:    
    
1) shadow_LED_switcher:    
   This demo use 2 onboard user keys as input to simulate mobile application remote control 8 onboard leds. GoWarrior GoBian platform acts as a device of aws iot connected to a shadow named ‘gwiottest01’. In this demo, a shadow update with desired led color will be sent to aws iot when triggering user key. And then aws iot will publish a shadow delta message back to the demo which subscribed to the shadow delta topic. Finally demo switches led to the color in delta message and sends a shadow update with reported led color to aws iot.    
   The user key1 and user key2 is different from led color change order.    
    
2) pub_sub_LED_switcher:    
This demo uses MQTT message publish and subscribe through aws iot to do same LED switcher.    
    
    
You need to do as following steps to try the demo:
------------------
1.	If you don’t have AWS account, you need to create one on “http://aws.amazon.com/”.
2.	Open the AWS IoT Dashboard in console.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step2.png) 
3.	Click “Get started” button to continue. Now you can create a Thing named ‘gowiottest01’.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step3.png) 
4.	Click “View thing” to continue.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step4.png) 
5.	Here is the detail information of the thing. Now click “Connect a device” button to associate the StartKit.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step5.png) 
6.	Choose ‘Embedded C’ since GoWarrior GoBian is a Debian system, then click “Generate certificate and policy” to continue.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step6.png) 
7.	Download all the files, and copy the ‘certificate’ and ‘private key’ files to StartKit folder ‘GoWarrior-IoT-StartKit\certs\’. Click “Confirm & start connecting” to continue.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step7.png) 
8.	Copy code sample in blue color to StartKit ‘GoWarrior-IoT-StartKit\aws_iot_src\aws_iot_config.h’. Now you’ve done all work on AWS console.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step8.png) 
9.	Compile and try the StartKit on GoWarrior TigerBoard. You may need to wait few minutes for AWS IoT spreading certificate. Here is the hardware components diagram.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step9.png) 
10.	You can easily find 2 user keys and 8 leds.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step10.png) 
11.	The shadow LED switcher may looks like this. First change to direction ‘GoWarrior-IoT-StartKit\apps\shadow_LED_switcher’ and execute shell cmd ‘sudo make’ to compile the demo. If success, then execute ‘./ shadow_LED_switcher’ to run the demo.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step11.png) 
12.	Trigger the user key, you can see the steps using shadow from several logs. The orange led will be lighted according to the scene.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step12.png) 
13.	And the TigerBoard.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step13.png) 
14.	And the shadow detail information on AWS console.
![image](https://github.com/GoWarrior/AWSIoTStartKitDemo/raw/master/readme-res/step14.png) 
15.	Now you can try the other demo pub_sub_LED_switcher


