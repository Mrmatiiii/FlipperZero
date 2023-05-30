# flipperzero-geigercounter
A geiger counter application for the Flipper Zero

![banner](https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/logo.jpg)

You need a **geiger counter** board to run this application. This board can be used : https://aliexpress.com/item/1005004074447209.html

You also need jumper wires to connect the board on the **Flipper Zero**.

**Note 1 :** this board uses a **J305** geiger tube. According this [website](https://www.rhelectronics.store/j305-glassy-geiger-muller-tube-nuclear-radiation-sensor) gamma conversion factor is **0.0081** for this tube. This value has been declared in the header of the source file so you can change it easily if needed. Incorrect conversion factor will give false measurements when **Sv** or **Rad** is selected.

**Note 2 :** **J305** geiger tube is only sensible to **beta** and **gamma** rays. **Alpha** rays cannot be detected. 

**Usable** radioactive sources : 
- natural uranium (alpha, beta, gamma)
- natural thorium (alpha, beta, gamma)
- radium-226 (alpha, beta, gamma)
- cobalt-60 (beta & gamma)

**Not really usable** radioactive sources : 
- americium-241 (alpha & low gamma, some strong beta/gamma rays are emitted during radioactive cascade)
- high purity metallic uranium/thorium (same as am241)


**Totaly unusable** radioactive sources : 
- polonium-210 (pure alpha)
- tritium (very low beta)


The geiger counter board can be powered with +5V power pin of the **Flipper Zero**. This pin will automatically be enabled when the program is launched. 

Output pin for measure on arduino cannot be used on the **Flipper Zero** because output voltage is too low. You can use jack out port instead. Just cut audio jack cable and connect audio channel (left, right or both together) with a cut half male jumper wire to **A7** GPIO :

<p align="center"><img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/jack.png" width=40% height=40%></p>

Black wire is usually used for the ground (sleeve on the schematic). You can use a multimeter to be sure or simply test other wires.

Global schema :

<p align="center"><img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/schema.jpg" width=75% height=75%></p>

## Build the program

Assuming the toolchain is already installed, copy **flipper_geiger** directory to **applications_user**

Plug your **Flipper Zero** and build the geiger counter :
```
./fbt launch_app APPSRC=applications_user/flipper_geiger
```

The program will automatically be launched after compilation

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper1.png" width=25% height=25%>

**A4** GPIO can be connected on **A7** GPIO to test this application without using a geiger tube. **A4** GPIO is generating a signal whose frequency changes every second.

Press **Ok** button to clear the graph, press **Left/Right** to choose unit (cpm, μSv/h, mSv/y, Rad/h, mRad/h, uRad/h), press **Back** button to quit

If you don't want to build this application, just simply copy **flipper_geiger.fap** on your **Flipper Zero**

## Use cases

Ambient radioactivity (descendants of radon gas are detected, not radon itself) :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper2.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper8.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper9.png" width=25% height=25%>

Measure of uranium ore piece inside a lead container :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper3.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper12.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper13.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper14.png" width=25% height=25%>

**Note :** measures in **Sv** or **Rad** are not precise

Measure of uranium ore piece (the most radioactive part) in contact with the geiger tube :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper4.png" width=25% height=25%>

Measure of radium dial pointers in contact with the geiger tube :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper5.png" width=25% height=25%>

All previous measures in a row (the scale of the graph is automatically adjusted) :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper6.png" width=25% height=25%>

Measure of uranium orange pottery :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper10.png" width=25% height=25%>

Measure of americium-241 button from a smoke detector :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper11.png" width=25% height=25%>

**A4** GPIO on **A7** GPIO :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper7.png" width=25% height=25%>

## What's next ?

Here are some nice ideas to improve this app :

* Save output data in XML / JSON file
* Use the geiger board as random number generator
* Send data on the air in real time to monitor remotly
* Buzz when it gets dangerous like a dosimeter

## Changelog

* 2023-04-11
  * More usable/unusable sources
  * Rad unit has been added
  * Code refactoring by replacing old mutex call by new method

* 2023-03-01
  * Usable/unusable sources have been added

* 2023-02-26
  * More clarity about how to connect audio jack cable on A7 GPIO

* 2023-02-02
  * μSv/h and mSv/y have been added
  * 5V pin is automatically enabled when the program is launched

* 2023-01-15
  * Code fix & optimizations
  * More events can be handled without any issue

* 2023-01-09
  * Code fix
  * Global schema was added

* 2023-01-08
  * Initial release