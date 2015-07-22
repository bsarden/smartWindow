// fuzzylogic.js
// Author: Bryce Arden
// July, 2015

// This is the code that goes into the fuzzylogic module on the node red server
// If that code ever gets deleted, this is our backup
// The basis for the fuzzylogic code was adopted from the NodeJS fuzzy logic library
// found here https://github.com/sebs/node-fuzzylogic

// definition of a fuzzylogic shape
function Shape(x0, x1, x2, x3) {
    this.x0 = x0;
    this.x1 = x1;
    this.x2 = x2;
    this.x3 = x3;
}

// for the purpose of smartWindow, we will only be using the trapezoid shape for 
// membership sets. However, this functionality can be extended to use other shapes
// for membership sets, such as Triangles, Rectangles, etc...
function Trapezoid(x0, x1, x2, x3) {
    Shape.call(this, x0, x1, x2, x3);
}

// defines how membership grade is determined from an input value
// uses basic trig
Trapezoid.prototype = {
    evaluate: function (val) {
        var result = 0, x = val;

        if (x <= this.x0) {
            result = 0;
        } else if (x >= this.x3) {
            result = 0;
        } else if ((x >= this.x1) && (x <= this.x2)) {
            result = 1;
        } else if ((x > this.x0) && (x < this.x1)) {
            result = (x / (this.x1 - this.x0)) - (this.x0 / (this.x1 - this.x0));
        } else {
            result = (-x / (this.x3 - this.x2)) + (this.x3 / (this.x3 - this.x2));
        }
        return result;
    }
};

// declaration of the fuzzylogic struct that we will use for this project
var fuzzylogic = { 
    trapezoid: function (val, x0, x1, x2, x3) {
        return new Trapezoid(x0, x1, x2, x3).evaluate(val);
    },
    and : function (a,b) {
        return Math.min(a, b);
    },
    or : function (a, b) {
        return Math.max(a,b);
    },
    not: function (a) {
        return 1.0 - a;
    }
};

// read in the global values from the iOSInput message
var summerMode = context.global.summerMode;
var winterMode = context.global.winterMode;
var manualMode = context.global.manualMode;
var manualVal = parseInt(context.global.manualVal); // convert JSON string to int
// var automaticMode = context.global.automaticMode;
var automaticMode = context.global.automaticMode;

// read in the light value from the mspMessage
var light = msg.payload.myWifiSensor.light;

// handle all of the iOS control functionality
if(winterMode == "true") {          // for winter mode, reverse the logic
    light = 850 - light;
}

if(manualMode == "true") {          // if we are in manualMode,append manualVal to msg and return
    msg.payload.myWifiSensor.veryShady = 0;         // set all other vals to 0 for freeboard UI
    msg.payload.myWifiSensor.shady  = 0;
    msg.payload.myWifiSensor.normal = 0; 
    msg.payload.myWifiSensor.sunny = 0;
    msg.payload.myWifiSensor.verySunny =  0; //opaque
    // iOSInput val update
    msg.payload.myWifiSensor.winterMode = winterMode;
    msg.payload.myWifiSensor.summerMode = summerMode;
    msg.payload.myWifiSensor.manualMode = manualMode;
    msg.payload.myWifiSensor.manualVal = manualVal;
    msg.payload.myWifiSensor.automaticMode = automaticMode;

    msg.payload.myWifiSensor.crispVal = Math.floor(manualVal);   
}
else if(automaticMode == "true") {
    // value of 800 means output of 0 V, which is 100% opaque
    // first, calculate the membership grade for each fuzzy set
    var veryShadyGrade = fuzzylogic.trapezoid(light, 0, 80, 160, 240); //clear
    var shadyGrade = fuzzylogic.trapezoid(light, 160, 240, 320, 400);
    var normalGrade = fuzzylogic.trapezoid(light, 320, 400, 480, 560);
    var sunnyGrade = fuzzylogic.trapezoid(light, 480, 560, 640, 720);
    var verySunnyGrade = fuzzylogic.trapezoid(light, 640, 720, 800, 800); //opaque
    
    // rule evaluation
    // slow transparent IF (normal AND shady) OR (shady)
    var slowTransparent = fuzzylogic.or(fuzzylogic.and(normalGrade, shadyGrade), shadyGrade);
    
    // fast transparent IF (shady AND veryShady) or (veryShady)
    var fastTransparent = fuzzylogic.or(fuzzylogic.and(shadyGrade, veryShadyGrade), veryShadyGrade);
    
    // normal IF normal AND NOT(shady) AND NOT(sunny)
    var normal = fuzzylogic.and(fuzzylogic.and(normalGrade, fuzzylogic.not(shadyGrade)), fuzzylogic.not(sunnyGrade));
    
    // slow Opague IF (normal AND sunny) OR (sunny)
    var slowOpague = fuzzylogic.or(fuzzylogic.and(normalGrade, shadyGrade), sunnyGrade);
    
    // fast Opague IF (sunny AND verySunny) OR verySunny
    var fastOpague = fuzzylogic.or(fuzzylogic.and(sunnyGrade, verySunnyGrade), verySunnyGrade);
    
    // because we know that the output of the dpot has the range [0, 255]
    // I can use a weighted average centered at the midpoint to achieve a balanced output for the control system
    // I am centering the system around the middle value, 128 so I will be picking values accordingly
    
    // constants that are used for calibration
    var LOW = 1;
    var LOW_MID = 60;
    var MID = 120;
    var MID_HIGH = 180;
    var HIGH = 255;
    var crispOutput = ((LOW * fastTransparent) + (LOW_MID  * slowTransparent) + (MID * normal) + 
            (MID_HIGH * slowOpague) + (HIGH * fastOpague)) / (fastTransparent + slowTransparent + normal + slowOpague + fastOpague);
    
    //value of 800 means output of 0 V, which is 100% opaque
    msg.payload.myWifiSensor.veryShady = Math.round((veryShadyGrade * 100)); //clear
    msg.payload.myWifiSensor.shady  = Math.round((shadyGrade * 100));
    msg.payload.myWifiSensor.normal = Math.round((normalGrade * 100));
    msg.payload.myWifiSensor.sunny = Math.round((sunnyGrade * 100));
    msg.payload.myWifiSensor.verySunny = Math.round((verySunnyGrade * 100)); //opaque
    msg.payload.myWifiSensor.light = Math.round(light);
    msg.payload.myWifiSensor.crispVal = Math.floor(crispOutput);
    // iOSInput val update
    msg.payload.myWifiSensor.automaticMode = automaticMode;
    msg.payload.myWifiSensor.winterMode = winterMode;
    msg.payload.myWifiSensor.summerMode = summerMode;
    msg.payload.myWifiSensor.manualMode = manualMode;
    msg.payload.myWifiSensor.manualVal = manualVal;
}
return msg;

