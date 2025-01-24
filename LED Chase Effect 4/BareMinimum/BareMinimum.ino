// libraries


// initialize variables

// LEDs
int led_pin_number[] = {7, 8, 9, 10, 11, 12, 13};
int number_of_leds = sizeof(led_pin_number) / sizeof(led_pin_number[0]);
int green_led_pin;
int led_position = 0;
int led_state = 0;

// button
int button_pin = 2;
int button_state = 0;
int last_button_state = 0;
bool is_paused = false;

// time
int time = 125;
int time_value;
int time_pin_number = A1;
unsigned long last_time_button_state_changed = millis();
unsigned long debounce_duration = 50;

// brightness
int bright = 255;
int intermediate = bright - 75;
int dim = intermediate - 75;
int off = 0;
int brightness_value;
int brightness_pin_number = A0;


// this function will have the LED chase effect run forward
void turn_on_LEDs_forward(int position) {
  // check if the current position is less than the total number of
  // red LEDs + 1 to make sure the current position is not out of bounds
  if (position < number_of_leds + 1) {
    // if the current position is at the first red LED then just only turn one LED on
    if (position == 0) {
      //Serial.println("1");
      analogWrite(led_pin_number[position], bright);
    }
    // if the current position is at the second red LED then just only turn two LEDs on
    else if (position == 1) {
      //Serial.println("2");
      analogWrite(led_pin_number[position], bright);
      analogWrite(led_pin_number[position - 1], intermediate);
    }
    // if the current position is not at the first or second LED then turn on three LEDs
    else if (position > 1 && position < number_of_leds){
      //Serial.println("3");
      analogWrite(led_pin_number[position], bright);
      analogWrite(led_pin_number[position - 1], intermediate);
      analogWrite(led_pin_number[position - 2], dim);
    }
    // if the current position is out of bounds then just only turn two LEDs on
    else {
      //Serial.println("4");
      analogWrite(led_pin_number[position - 1], intermediate);
      analogWrite(led_pin_number[position - 2], dim);
    } 
  }
}

// this function will have the LED chase effect run backward
void turn_on_LEDs_backward(int position) {
  // check if the current position is greater than -1 to
  // make sure the current position is not out of bounds
  if (position > -2) {
    // if the current position is at the last red LED then just only turn one LED on
    if (position == number_of_leds - 1) {
      //Serial.println("1");
      analogWrite(led_pin_number[position], bright);
    }
    // if the current position is at the second to last red LED then just only turn two LEDs on
    else if (position == number_of_leds - 2) {
      //Serial.println("2");
      analogWrite(led_pin_number[position], bright);
      analogWrite(led_pin_number[position + 1], intermediate);
    }
    // if the current position is not at the last or second to last LED then turn on three LEDs
    else if (position < number_of_leds - 2 && position > -1){
      //Serial.println("3");
      analogWrite(led_pin_number[position], bright);
      analogWrite(led_pin_number[position + 1], intermediate);
      analogWrite(led_pin_number[position + 2], dim);
    }
    // if the current position is out of bounds then just only turn two LEDs on
    else {
      //Serial.println("4");
      analogWrite(led_pin_number[position + 1], intermediate);
      analogWrite(led_pin_number[position + 2], dim);
    } 
  }
}

// this function will turn off all LEDs
void turn_off_LEDs() {

  // since we are turning off an LED, it will be easier to code
  // this function in a way that turns off all LEDs at once
  for (int i = 0; i < number_of_leds; i++) {
    analogWrite(led_pin_number[i], off);
  }
}

// this function updates the time and brightness based on the potentiometers
void update_time_and_brightness() {
  // the 1st potentiometer will get a read value
    // which will be used to calculate the delay time
    time_value = analogRead(time_pin_number);
    time = time_value;

    // the 2nd potentiometer will get a read value
    // which will be used to calculate the brightness
    brightness_value = analogRead(brightness_pin_number);
    bright = (255. / 1023.) * brightness_value;
    intermediate = bright - 75;
    dim = intermediate - 75;
}

void setup() {
  // put your setup code here, to run once:

  // create serial monitor
  Serial.begin(9600);

  // set red LED pins as output signals
  for (int i = 0; i < number_of_leds; i++) {
    pinMode(led_pin_number[i], OUTPUT);
  }

  // set potentiometer pins as input signals
  pinMode(time_pin_number, INPUT);
  pinMode(brightness_pin_number, INPUT);

  // set button pin as an input signal
  pinMode(button_pin, INPUT_PULLUP);

  // get the center LED pin
  if (number_of_leds % 2 == 1) {
    green_led_pin = number_of_leds / 2;
  }
  else {
    green_led_pin = number_of_leds / 2 - 1;
  }

  Serial.println(green_led_pin);

  last_button_state = digitalRead(button_pin);

  // start the program by having all LEDs off and wait for 2 seconds
  turn_off_LEDs();
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - last_time_button_state_changed >= debounce_duration) {
    // update the button state
    button_state = digitalRead(button_pin);

    if (button_state != last_button_state) {
      last_button_state = button_state;

      if (button_state == LOW) {
        if (led_state == HIGH) {
          led_state = LOW;
        }
        else {
          led_state = HIGH;
        }
        digitalWrite(led_pin_number[green_led_pin], led_state);
      }
    }
  }

  /*
  // move LEDs forward
  for (int i = 1; i < number_of_leds + 1; i++) {
    // update led position
    if (i <= number_of_leds) {
      led_position = i;
    }
    
    // update the time and brightness based on the potentiometers
    update_time_and_brightness();
    
    // this turns the LEDs on and off
    turn_on_LEDs_forward(i);
    delay(time);
    turn_off_LEDs();
  }
  
  
  
  // move LEDs backward
  for (int i = number_of_leds - 2; i > -2; i--) {
    // update led position
    if (i >= 0) {
      led_position = i;
    }

    // update the time and brightness based on the potentiometers
    update_time_and_brightness();
    
    // this turns the LEDs on and off
    turn_on_LEDs_backward(i);
    delay(time);
    turn_off_LEDs();
  }
  */
}