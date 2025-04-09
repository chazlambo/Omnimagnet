// CORRECT INPUT TEMPLATE:
// {PC}: [FUNCTION: getPos ADDRESS: 72]

String key_start = "{PC}: [";
String key_end = "]\n";
String key_arduino = "{ARDUINO}: [";
String str_getPos = "getPos";
String str_getTemp = "getTemp";
String tag_function = "FUNCTION:";
String tag_address = "ADDRESS:";

String possible_functions = str_getPos + " " + str_getTemp;
char *tag_pots[] = {"HORZ: ", "WRIST: ", "BASE: ", "VERT: "};
char *tag_temps[]= {"INNER: ", "MIDDLE: ", "OUTER: "};

int potVals[4];
int tempVals[3];

void setup() {
  Serial.begin(9600); //Start Serial Communication
  delay(100);
  Serial.print(key_arduino);
  Serial.print("Arduino is ready."); //Send ready message
  Serial.print(key_end);
}

void loop() {
  if (Serial.available()) {

    String input = Serial.readString(); // Read string from serial input

    if (input.startsWith(key_start) && input.endsWith(key_end)) { // Check for correct format

      // Check for address tag
      if (input.indexOf(tag_address) == -1) {
        Serial.println("Error: Input string did not contain address tag. Check format.");
        return 0;
      }
      // Check for function tag
      if (input.indexOf(tag_function) == -1) {
        Serial.println("Error: Input string did not contain function tag. Check format.");
        return 0;
      }

      int tag_address_index = input.indexOf(tag_address);                                           // Get index for first element of address tag
      int tag_address_index_end = tag_address_index + tag_address.length() + 1;                     // Get index for the beginning of the address number
      String address_string = input.substring(tag_address_index_end,input.lastIndexOf(key_end));    // Extract string for address
      int address = address_string.toInt();                                                         // Convert address string to integer

      // Check for viable address
      // ADC modules possible addresses range from 0x48 to 0x4F (72 to 79)
      if (address < 72 || address > 79) {
        Serial.print("Error: Inputted address is not possible. Input address was: ");
        Serial.print(address);
        Serial.println(", Possible addresses range from 72 to 79.");
        return 0;
      }

      int tag_function_index = input.indexOf(tag_function);                                         // Get index for start of function tag
      int tag_function_index_end = tag_function_index + tag_function.length() + 1;                  // Get index for end of function tag
      String function = input.substring(tag_function_index_end, tag_address_index - 1);             // Extract string for selected function

      // Check if specified function exists
      if (possible_functions.indexOf(function) == -1) {
        Serial.print("Error: Inputted function: \"");
        Serial.print(function);
        Serial.println("\" does not exist. Possible functions are:");
        Serial.println(possible_functions);
        Serial.println();
        return 0;
      }

      Serial.println("Correct Keys were found.");
      Serial.print("Address Int: ");
      Serial.println(address);
      Serial.print("Selected Program: ");
      Serial.println(function);

      if(function ==  str_getPos){
        getPos();
      }
      else if(function == str_getTemp){
        getTemp();
      }
      else {
        Serial.println("Error: Something happened where the input function didn't match the options but got through the first check.");
      }
    }

    if (input.indexOf(key_start) == -1) {
      Serial.println("Error: Could not find correct start key. Check input format.");
      return 0;
    }
    if (input.indexOf(key_end) == -1) {
      Serial.println("Error: Could not find correct end key. Check input format.");
      return 0;
    }
  }
}

int * getPos(){
  int rando = random(9);
  potVals[0] = 100 * rando;
  potVals[1] = 200 * rando;
  potVals[2] = 300 * rando;
  potVals[3] = 400 * rando;
  Serial.print(key_arduino);
  for (int i = 0; i < 4; i++) {
    if(i>0){
      Serial.print("\t");
    }
    Serial.print(tag_pots[i]);
    Serial.print(potVals[i]);
  }
  Serial.print(key_end);
  Serial.flush();
}

int * getTemp(){
  int rando = random(9);
  tempVals[0] = 10;
  tempVals[1] = 20;
  tempVals[2] = 30;
  Serial.print(key_arduino);
  for (int i = 0; i < 3; i++) {
    if(i>0){
      Serial.print("\t");
    }
    Serial.print(tag_temps[i]);
    Serial.print(tempVals[i]);
  }
  Serial.print(key_end);
  Serial.flush();
}
