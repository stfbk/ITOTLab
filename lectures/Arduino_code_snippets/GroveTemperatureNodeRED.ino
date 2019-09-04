// Demo code for Grove - Temperature Sensor V1.1/1.2
// Loovee @ 2015-8-26

/* Import and adapt in NodeRED
[{"id":"ffa17d8f.8c3708","type":"tab","label":"Flow 1","disabled":false,"info":""},{"id":"f17ad95c.5dbae8","type":"serial-port","z":"ffa17d8f.8c3708","serialport":"COM7","serialbaud":"57600","databits":"8","parity":"none","stopbits":"1","newline":"\\n","bin":"false","out":"char","addchar":false},{"id":"9c5f2bc4.00bb98","type":"ui_group","z":"ffa17d8f.8c3708","name":"Temperature","tab":"5bc07256.8d8d54","disp":true,"width":"5","collapse":false},{"id":"5bc07256.8d8d54","type":"ui_tab","z":"ffa17d8f.8c3708","name":"Home","icon":"dashboard"},{"id":"12c06ee7.206249","type":"ui_base","theme":{"name":"theme-light","lightTheme":{"default":"#0094CE","baseColor":"#0094CE","baseFont":"-apple-system,BlinkMacSystemFont,Segoe UI,Roboto,Oxygen-Sans,Ubuntu,Cantarell,Helvetica Neue,sans-serif","edited":true,"reset":false},"darkTheme":{"default":"#097479","baseColor":"#097479","baseFont":"-apple-system,BlinkMacSystemFont,Segoe UI,Roboto,Oxygen-Sans,Ubuntu,Cantarell,Helvetica Neue,sans-serif","edited":false},"customTheme":{"name":"Untitled Theme 1","default":"#4B7930","baseColor":"#4B7930","baseFont":"-apple-system,BlinkMacSystemFont,Segoe UI,Roboto,Oxygen-Sans,Ubuntu,Cantarell,Helvetica Neue,sans-serif"},"themeState":{"base-color":{"default":"#0094CE","value":"#0094CE","edited":false},"page-titlebar-backgroundColor":{"value":"#0094CE","edited":false},"page-backgroundColor":{"value":"#fafafa","edited":false},"page-sidebar-backgroundColor":{"value":"#ffffff","edited":false},"group-textColor":{"value":"#1bbfff","edited":false},"group-borderColor":{"value":"#ffffff","edited":false},"group-backgroundColor":{"value":"#ffffff","edited":false},"widget-textColor":{"value":"#111111","edited":false},"widget-backgroundColor":{"value":"#0094ce","edited":false},"widget-borderColor":{"value":"#ffffff","edited":false},"base-font":{"value":"-apple-system,BlinkMacSystemFont,Segoe UI,Roboto,Oxygen-Sans,Ubuntu,Cantarell,Helvetica Neue,sans-serif"}},"angularTheme":{"primary":"indigo","accents":"blue","warn":"red","background":"grey"}},"site":{"name":"Node-RED Dashboard","hideToolbar":"false","allowSwipe":"false","lockMenu":"false","allowTempTheme":"true","dateFormat":"DD/MM/YYYY","sizes":{"sx":48,"sy":48,"gx":6,"gy":6,"cx":6,"cy":6,"px":0,"py":0}}},{"id":"a39dc66e.53a008","type":"serial in","z":"ffa17d8f.8c3708","name":"","serial":"f17ad95c.5dbae8","x":170,"y":200,"wires":[["659e3927.c910c8","7cab6b0f.7868c4"]]},{"id":"659e3927.c910c8","type":"debug","z":"ffa17d8f.8c3708","name":"","active":true,"tosidebar":true,"console":false,"tostatus":false,"complete":"payload","targetType":"msg","x":470,"y":260,"wires":[]},{"id":"7cab6b0f.7868c4","type":"ui_text","z":"ffa17d8f.8c3708","group":"9c5f2bc4.00bb98","order":0,"width":0,"height":0,"name":"Temperature (C°)","label":"Arduino Reading","format":"{{msg.payload}}","layout":"col-center","x":490,"y":180,"wires":[]}]
*/

#include <math.h>

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A0

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    int a = analogRead(pinTempSensor);

    float R = 1023.0/a-1.0;
    R = R0*R;

    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet

    Serial.println(temperature);

    delay(3000);
}
