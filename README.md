# RoboIAP

## Introduction

The RoboIAP project seeks to automate (robo) and improve upon how a 24-hour intermittent access paradigm (IAP) is run. Experiment hardware, experiment software and post-hoc data analysis will be covered on this page. At present, RoboIAP improves upon IAP by collecting information on how rodents consume alcohol in a 24-hour trial. By measures the liquid level every second, individual bouts and various bout characteristics can be analyzed. Thus, RoboIAP presents an improvement upon how IAP is run.

The rest of this page is broken down into experiment hardware, experiment software, running the experiment and post-hoc analysis. Each section is further broken down into subsections, one being an overview with the others being specific to the section, allowing for expounding on the pertinent details.

## Experiment Hardware

### Overview:

All hardware components can be found in the parts list and the details of the setup will be hashed out below. Briefly, A microcontroller (Arduino Uno) collects analog data from multiple vegetronix liquid level sensors and stores the data on an SD card via a data logging shield. The sensor wire is immersed in a graduated custom bottle in order to sense the level of liquid. At the other end, the bottle is connected via silicone tubing to a sipper. Thorough sealing with a rubber stopper and silicone gel creates the airtight chamber necessary for the sipper to properly function. As the rodents drink, the liquid level falls and is detected by the sensor for storage on the SD card. As the rodents cannot be given access simultaneously, a button is used to act as a timestamp, facilitating trial alignment between animals. 

### Arduino/SD Shield:

Depending on the SD shield purchased, headers may need to be soldered on. If so, it is recommended to use “helping hands” rather than soldering while the headers are in the Arduino, as the heat can damage the analog ports. Wires are soldered into the metal rings of the SD shield, offering superior stability over the standard female headers of the Arduino. The SD shield takes a 3 volt CR1220 battery. A SD to microSD adapter is used with the SD shield to allow for storage on the now commonplace microSD. The sketch is uploaded to the Arduino via a A-B USB cable. 

### Bottles:

The bottles are constructed from 40 mL Fisher brand? automatic pipette tips. Each tube is cut 2 inches from each end with a saw. ¼ inch diameter rubber stoppers with a 2mm diamter hole are siliconed with the sensor wire running through the hole. This is done in such a way that when the rubber stopper is firmly inserted into the tube, the end of the taught sensor wire does not clog the other opening of the bottle. ½ inch diamater silicone tubing is inserted over the tapered end of the tube. The other end of the silicone tube is attached to the sipper. 

### Sensors:

The vegetronix sensor wiring is not intuitive. The black wire of the sensor is the signal while the silver (unsheathed) wire is the ground. The sensors are powered by the 5 volt power pin of the Arduino and also grounded with the Arduino. The black wire of the sensor is connected to an analog port on the Arduino. This wiring can be seen in the diagram and image below. 

### Button:

The button, when pressed, connects a 3.3 volt current to digital pin 2 on the arduino and the ground through a 220 ohm resistor. This is achieved with the wiring shown in the diagram and image below.

PICTURES NEEDED 
Sensor in tube
Silicone rubber stoppers
On rack picture
Circuit

## Experimental Software

### Overview:

All experimental software is written in the Arduino coding language and is contained in one sketch. The software records the analog voltage values and button state it detects. The sketch is descriptively commented and is the best place to look for an understanding of the code.

### Libraries:

The sketch uses the SPI, SD, Wire, and RTClib libraries. If missing one or more of these, they can be installed with “Sketch>Include Library>”Manage Libraries” in the Arduino application. 

### Setup():

The setup function of an Arduino sketch is only run once upon turning the microcontroller on. In the setup function, connections are established over the USB port and with the SD shield if available. The USB connection is not necessary, but is beneficial during debugging. To use this feature go to: “Tools>Serial Monitor” in the Arduino application after uploading the sketch to the Arduino. Digital pin 2 is set as an input for the push button in the setup function as well. 

### Loop():

The loop function is run continuously after the setup function runs its course. In this function, the data is written to the SD card and the program checks for button presses until this iteration of the loop function has been running for a second. Upon completion, the loop function runs again, ensuring the data is recorded every second.

## Running the Experiment

### Overview:

Running a RoboIAP experiment is much like running an IAP experiment. The animals are weighed, ethanol bottles are put on and then taken off 24 hours later. However, instead of weighing the alcohol bottle before and after the trial, its volume is taken and recorded via the custom graduated bottles. Additionally, the sensors must be calibrated daily. After completing a trial, the data on the SD card of each Arduino should be transferred to a more permanent location for analysis. At current, the data on the SD card is overwritten every time the arduino restarts. Thus, failure to transfer the data before turning the Arduino back on may lead to a loss of data.

### Filling/Emptying:

Liquid is held in the tube by a vacuum. If the vacuum is not maintained, the sipper will leak. This can cause trouble when filling, as the rubber stopper must be removed. This problem is circumnavigated by inserting a silicone stopper into the sipper anytime the rubber stopper is removed from the top of the bottle. Conversely, the rubber stopper can be removed without inserting a silicone stopper into the sipper if the tube is to be emptied. This is desired when dry calibrating or an experiment is concluded. 

After removing the rubber stopper, it is important to reinsert it such that the sensor wire is as straight as possible. Nonlinearities in the sensor wire correspond to nonlinearities in the voltage to liquid volume conversion, which is almost impossible to correct for. Additionally, the rubber stopper should be inserted as firmly as possible to ensure an airtight seal. 

Troubleshooting tip: On the contrary, attempting to insert the rubber stopper too hard may result in cracking the tube, preventing an airtight seal and thus preventing the formation of a vacuum. 

### Calibration:

The voltage output of the sensors scales linearly with the calibrated liquid level. Thus, proper calibration is essential for generating accurate results. The sensors must be calibrated both wet and dry. Accurate results can be obtained by dry calibrating the sensor once before the month long experiment begins and wet calibrating before every trial. The “dry” terminology may be misleading. As residual liquid would left in the tube if a rodent were to completely consume all the liquid in the bottle, the sensor would not be completely dry. Rather, a sensor is dry calibrated after filling a bottle and then emptying it. Quickly after emptying, the rubber stopper should be pressed firmly into the tube with the sensor wire straight as possible before holding down the dry calibrate button on the vegetronix sensor until a flashing red LED is observed (about 3 seconds). Thus, before the desired start day for the experiment, every bottle must be filled, emptied and dry calibrated. Before every trial, a wet calibration is done. This is achieved by clogging the sipper with a silicone stopper, filling every tube with an automatic pipette, inserting the rubber stopper into the top of the bottle, then holding down the wet calibration button until a flashing red LED is observed (about 3 seconds). 

Troubleshooting tip: If no red flashing LED is observed after 3 seconds, there are several possibilities.	
The sensor is not powered (damaged wire, short circuited, etc)
The button is not actually being pressed (the buttons are small and must be pressed firmly)
The sensor has been damaged (can occur if liquid falls on the electronic components of the sensor)

Note: At present, water measurements are taken by weight. Thus, the water bottle is weighed before and after every experiment. The RoboIAP project plans to incorporate water level sensing in the near future. 

PICTURES NEEDED
Wet and dry calibration buttons

### Data Recorded:

The starting and ending volumes as measured by the graduated bottle are necessary for the voltage to liquid level conversion. Thus, they should be recorded immediately after the sipper is put on the rodent cage and immediately before the sipper is taken off of the rodent cage.

## Post-Hoc Analysis

### Overview:

RoboIAP allows for the liquid consumption analysis with the resolution of a second. At this resolution, individual drinking bouts are apparent. Bouts can further be characterized by their duration and magnitude. In order to achieve this level of analysis, it is essential to identify the bout start and end times. With this information, the magnitude can also be calculated. 

### Manual:

The most labor intensive method of analysis is to manually identify when the bouts start and end. Indeed, it is readily evident when a rodent consumes alcohol by looking at the liquid level data. However, this is prone to human error and may yield incongruous results when performed by another individual. Furthermore, this method is difficult to scale to large datasets. Thus, this method is to be avoided unless labeling data for machine learning.



### Best Best-Fit Line (BBFL):

The BBFL technique finds the best-fit line that most properly matches a drop in voltage data. This is achieved by breaking the data up into overlapping segments and calculating the best-fit lines between all points in the segment. Several start and end points extracted from a matrix of a local minima are assessed for sufficient drops in voltage and sufficient duration. A benefit of this approach is a concrete definition of a bout: a bout start and end is defined as the indexes that correspond to a local minima of a least squares error matrix that contains the bout that has the greatest drop in voltage. A drawback of the method is the computation time. The algorithm can be sped up by only feeding it segments that are likely to have a bout, avoiding unnecessary computation.

PICTURES NEEDED:
Best fit error matrix

Machine Learning Method:

As the data from several experiments has been analyzed by hand, there is ample data for machine learning applications. RoboIAP plans to release some form of machine learning for bout analysis in the near future.

Troubleshooting tip: The vegetronix liquid level sensors are sensitive to temperature. If developing algorithms, this is important to keep in mind. The apparent voltage increases with increasing temperature and decreases with decreasing temperature. These fluctuation are on the order of hours while bouts are on the order of a minute or two, but these fluctuations can complicate analysis somewhat. 
