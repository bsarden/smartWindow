// This is the code that goes into the fuzzylogic module on the node red server
// If that code ever gets deleted, this is our backup

// rules functionality

function Shape(x0, x1, x2, x3) {
    this.x0 = x0;
    this.x1 = x1;
    this.x2 = x2;
    this.x3 = x3;
}

function Trapezoid(x0, x1, x2, x3) {
    Shape.call(this, x0, x1, x2, x3);
}

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
    // and: function (a, b, cbA, cbB) {
    //     var ret = Math.min(a, b);
    //     if (ret === a) {
    //         cbA(ret);
    //     } else {
    //         cbB(ret);
    //     }
    //     return ret;
    // },
    // or: function (a, b, cbA, cbB) {
    //     var ret = Math.max(a, b);
    //     if (ret === a) {
    //         cbA(ret);
    //     } else {
    //         cbB(ret);
    //     }
    //     return ret;
    // },
    not: function (a) {
        return 1.0 - a;
    }
};

var light = msg.payload.myWifiSensor.light;

// value of 800 means output of 0 V, which is 100% opaque
// first, calculate the membership grade for each fuzzy set
var veryShadyGrade = fuzzylogic.trapezoid(light, 0, 80, 160, 240); //clear
var shadyGrade = fuzzylogic.trapezoid(light, 160, 240, 320, 400);
var normalGrade = fuzzylogic.trapezoid(light, 320, 400, 480, 560);
var sunnyGrade = fuzzylogic.trapezoid(light, 480, 560, 640, 720);
var verySunnyGrade = fuzzylogic.trapezoid(light, 640, 720, 800, 800); //opaque

// var or = fuzzylogi.or;
// var and = fuzzylogic.and;
// var not = fuzzylogic.not;
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
// I can use a weighted average centered at the midpoint to achieve a balanced output 
// for the control system
// I am centering the system around the middle value, 128 so I will be picking values accordingly

var crispOutput = ((1 * fastTransparent) + (60  * slowTransparent) + (120 * normal) + (180 * slowOpague) + (fastOpague * 255))/
    (fastTransparent + slowTransparent + normal + slowOpague + fastOpague);

//msg.payload.fuzzy = fuzzylogic.trapezoid(msg.payload.myWifiSensor.light, 0, 200, 300, 500);
//value of 800 means output of 0 V, which is 100% opaque
msg.payload.myWifiSensor.veryShady = Math.round((veryShadyGrade * 100)); //clear
msg.payload.myWifiSensor.shady  = Math.round((shadyGrade * 100));
msg.payload.myWifiSensor.normal = Math.round((normalGrade * 100));
msg.payload.myWifiSensor.sunny = Math.round((sunnyGrade * 100));
msg.payload.myWifiSensor.verySunny = Math.round((verySunnyGrade * 100)); //opaque
msg.payload.myWifiSensor.crispVal = Math.floor(crispOutput);
//msg.payload.fuzzy = fuzzylogic.trapezoid(msg.payload.myWifiSensor.light, 0, 200, 300, 500);
return msg
