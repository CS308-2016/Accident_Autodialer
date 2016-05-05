2016 - CS308  Group X : Project README TEMPLATE
================================================

Group Info:
------------
+   Karan Ganju (120050021)
+   Sanket Kanjalkar (120050011)
+   Deepanjan Kundu (120050009)
+   Roshan (120050082)


Project Description
-------------------

The aim of the project is to create an accident autodialler. The accident auto dialer should consist of three parts which are each explained as follows

Accident and driver rating detection - The embedded system should, via detection of changes in linear and angular acceleration, detect appropriately whether an accident has occurred. The frequency and severity of jerks or momentum changes also provide a basis to judge the drive rating.

Location detection - At the time of the accident, the location of the accident, in terms of GPS coordinates has to be relayed to either an emergency contact or to the hospital. Hence, as soon as an accident is detected the GPS should be powered on and coordinates should be obtained.

Communication - This relay of information should take place via cellular network as WiFi networks cannot be expected in the car. Hence, communication of this data has to take place over 2G/3G networks with the occasional use of SMS messages to relay messages for emergency contacts.


Technologies Used
-------------------

Remove the items that do no apply to your project and keep the remaining ones.

+   Embedded C
+   Android


Installation Instructions
=========================

Install CCStudio for micro-controller code and Android studio for the UI code.  


Demonstration Video
=========================  

https://youtu.be/okb6rUx2YTg - micro-controller screencast
https://youtu.be/RnZ_UCMMSF4 - UI Screencast
https://youtu.be/OYxc4accaWk - project demo


References
===========

+   [MPU 9250 Datasheet](https://store.invensense.com/datasheets/invensense/MPU9250REV1.0.pdf)
+   [SIM908 AT Commands](http://www.dfrobot.com/image/data/TEL0051/3.0/SIM908_AT%20Command%20Manual_V1.01.pdf)
