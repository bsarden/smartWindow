/*
 * JSONify.ino
 * takes our light sensor information and outputs it to server
 * as a JSON(JavaScript Object Notation) event. 
 * Creates a standard for us to pass information to third party front-end
 * tools, such as freeboard.io and our node-red server
 */

void setupJSONify() {
    myEvent.begin();
    delay(100);     // give time for myEvent to start
//    Serial.begin(115200);    // uncomment to print out JSON event
    wifiReady.waitFor();    // waits for a wireless connection before starting up events
}

/*Create JSON with following format:
{ 
    "myWifiSensor": { 
        "myName" : "smartWindowSensor", 
        "light" : "221" 
     }
}
*/
void loopJSON(){
    // convert light val into char array for json parsing
    String str = (String)light;
    int str_len = str.length() + 1;   // one extra character for the null terminator
    char char_array[str_len];
    str.toCharArray(char_array, str_len);  // copy string into char buffer
    aJsonObject *root = aJson.createObject();
    aJsonObject *d = aJson.createObject();
    aJson.addItemToObject(root, "myWifiSensor", d);
    aJson.addStringToObject(d, "myName" , "smartWindowSensor"); // add name to JSON
    aJson.addStringToObject(d, "light" , char_array); // add light to JSON
//    aJson.addNumberToObject(d, "light", light);
    jsonPayload = aJson.print(root)+'\0'; // Convert JSON object to char array
//    Serial.println(jsonPayload); // Un-comment to print out JSON-encoded data
    aJson.deleteItem(d); // this library uses malloc. Free mem.
    aJson.deleteItem(root); // this library uses malloc. Free mem.
    myEvent.send(); // Trigger event! New JSON payload ready
    delay(300); // Determines how often a new JSON payload is created  
}

