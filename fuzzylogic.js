// This is the code that goes into the fuzzylogic module on the node red server
// If that code ever gets deleted, this is our backup
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
    and: function (a, b, cbA, cbB) {
        var ret = Math.min(a, b);
        if (ret === a) {
            cbA(ret);
        } else {
            cbB(ret);
        }
        return ret;
    },
    or: function (a, b, cbA, cbB) {
        var ret = Math.max(a, b);
        if (ret === a) {
            cbA(ret);
        } else {
            cbB(ret);
        }
        return ret;
    },
    not: function (a) {
        return 1.0 - a;
    }
};

var light = msg.payload.myWifiSensor.light;
//value of 800 means output of 0 V, which is 100% opaque
msg.payload.myWifiSensor.veryShady = fuzzylogic.trapezoid(light, 0, 80, 160, 240) * 100; //clear
msg.payload.myWifiSensor.shady  = fuzzylogic.trapezoid(light, 160, 240, 320, 400) * 100;
msg.payload.myWifiSensor.normal = fuzzylogic.trapezoid(light, 320, 400, 480, 560) * 100;
msg.payload.myWifiSensor.sunny = fuzzylogic.trapezoid(light, 480, 560, 640, 720) * 100;
msg.payload.myWifiSensor.verySunny = fuzzylogic.trapezoid(light, 640, 720, 800, 800) * 100; //opaque
//msg.payload.fuzzy = fuzzylogic.trapezoid(msg.payload.myWifiSensor.light, 0, 200, 300, 500);
return msg
