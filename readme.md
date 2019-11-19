# DIY IoT Workshop Thermostat Example

This folder contains the files to create the themostat example used in Workshop #3.
There are two folders containing the "client" or web-browser user interface (UI) simulating a thermostat and esp8266-wifi-thermostat which contains the Arduino sketch for reading the DHT-11 and controlling the LED to simulate the turning on/off of the thermostat from the UI.

## esp8266-wifi-thermostat

This folder contains the complete code for creating a web server and web services to control the simulated thermostat actuator (LED) and thermostat sensor DHT-11.

The sketch includes the embedded HTML and Javascript needed to display the UI and functions to call the REST web services exposed on the ESP8266 web server. These services control the LED state (on/off) and query the temperature from the DHT-11 sensor.

## Client (Web-Browser client)

This example is the client-side (web browser) code that is used on the themostat.ino sketch
used in the workshop to demonstrate the use of the DHT-11 in a faux 'real-world' scenario in which a themostat is controlled remotely on the local WiFi network.

The file structure consists of two main files: index.html and src/index.js.
The additional supporting files are retrieved from a CDN to minimize file size.

```
+-iot-workshop-thermostat
  +-src
  |  |
  |  |-index.js
  |
  |-index.html
```

The files were minimized and re-combined to add to the thermostat.ino sketch.

## Minify the files for inclusion in thermostat.ino

The files can be minified using the following commands. The full CLI command
with appropriate flags can be found in the package.json

### Minify the html file

```bash
$ npm run minify:html
```

### Minify the js file

```bash
$ npm run minify:js
```
