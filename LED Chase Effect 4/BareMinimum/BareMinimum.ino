// initialize variables

// LEDs
int led_pin_number[] = {7, 8, 9, 10, 11, 12, 13};
int number_of_leds = sizeof(led_pin_number) / sizeof(led_pin_number[0]);
int green_led_pin;
int current_led_position = 0;   // position == array index

// time
int time = 125;
int time_value;
int time_pin_number = A1;
unsigned long last_time_button_state_changed = millis();
unsigned long debounce_duration = 50;
unsigned long unpause_time = 0;
unsigned long unpause_delay_duration = 500;

// brightness
int bright = 255;
int intermediate = bright - 75;
int dim = intermediate - 75;
int off = 0;
int brightness_value;
int brightness_pin_number = A0;

// button
int button_pin = 2;
int button_state = 0;
int last_button_state = 0;

// flags
bool game_is_paused = false;
bool game_unpaused_recently = false;
bool game_won = false;
bool game_won_1 = false;

// this function has the LED chase effect run in the forward direction
void turn_on_LEDs_forward(int position) {
  // this loop will turn on either 1, 2, or 3 LEDs based on the current led position
  for (int i = 0; i <= position; i++) {
    if (i == position) {
      analogWrite(led_pin_number[i], bright);
    }
    else if (i == position - 1) {
      analogWrite(led_pin_number[i], intermediate);
    }
    else if (i == position - 2) {
      analogWrite(led_pin_number[i], dim);
    }
  }
}

// this function has the LED chase effect run in the backward direction
void turn_on_LEDs_backward(int position) {
  // this loop will turn on either 1, 2, or 3 LEDs based on the current led position
  for (int i = number_of_leds - 1; i >= position; i--) {
    if (i == position) {
      analogWrite(led_pin_number[i], bright);
    }
    else if (i == position + 1) {
      analogWrite(led_pin_number[i], intermediate);
    }
    else if (i == position + 2) {
      analogWrite(led_pin_number[i], dim);
    }
  }
}

// this function turns all LEDs off (note: easier to code it to turn all of them off instead of finding the certain ones that are on)
void turn_off_LEDs() {
  for (int i = 0; i < number_of_leds; i++) {
    digitalWrite(led_pin_number[i], LOW);
  }
}

// this function updates the time and brightness based on the potentiometers
void update_time_and_brightness() {
  time_value = analogRead(time_pin_number);
  time = time_value;

  brightness_value = analogRead(brightness_pin_number);
  bright = (255. / 1023.) * brightness_value;
  intermediate = bright - 75;
  dim = intermediate - 75;
}

// this function blinks the green LED three times when user lands on it
void blink_green_led() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(led_pin_number[green_led_pin], HIGH);
    delay(200);
    digitalWrite(led_pin_number[green_led_pin], LOW);
    delay(200);
  }
}

// this function keeps the current position's LED on when the game is paused
void game_paused(int position) {
  turn_off_LEDs();

  if (game_won == true) {
    digitalWrite(led_pin_number[green_led_pin], HIGH);
  }
  else {
    digitalWrite(led_pin_number[position], HIGH);
  }
}

// this function lights up the current position's LED and its trailing LEDs
void light_current_LED(int position, int direction) {
  turn_off_LEDs();

  if (direction == 1) {
    turn_on_LEDs_forward(position);
  }
  else {
    turn_on_LEDs_backward(position);
  }
}

void setup() {
  // put your setup code here, to run once:

  // set up serial monitor at 9600 baud
  Serial.begin(9600);

  // set LED pins as output signals
  for (int i = 0; i < number_of_leds; i++) {
    pinMode(led_pin_number[i], OUTPUT);
  }

  // set potentiometer pins as input signals
  pinMode(time_pin_number, INPUT);
  pinMode(brightness_pin_number, INPUT);

  // set button pin as an input signal
  pinMode(button_pin, INPUT_PULLUP);

  // get the center LED position
  if (number_of_leds % 2 == 1) {
    green_led_pin = number_of_leds / 2;
  }
  else {
    green_led_pin = number_of_leds / 2 - 1;
  }

  // initially set the last button state as the initial state when starting up the program
  last_button_state = digitalRead(button_pin);

  // when starting the program, turn off all LEDs and wait a moment
  turn_off_LEDs();
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

  // set this to false to prevent the win state from looping infinitely
  game_won_1 = false;

  // this handles the button press with debounce
  if (millis() - last_time_button_state_changed >= debounce_duration) {
    button_state = digitalRead(button_pin);

    if (button_state != last_button_state) {
      last_button_state = button_state;
      last_time_button_state_changed = millis();

      if (button_state == LOW) {
        game_is_paused = !game_is_paused;
        game_unpaused_recently = !game_is_paused;

        if (game_is_paused == true) {
          digitalWrite(led_pin_number[green_led_pin], HIGH);
        }
        else {
          unpause_time = millis();
          digitalWrite(led_pin_number[green_led_pin], LOW);
        }
      }
    }
  }

  // update the time and brightness
  update_time_and_brightness();

  // this is so that the LED chase effect runs only when the game is not paused
  static unsigned long last_update_time = 0;
  static int led_direction = 1;          // 0 = backward, 1 = forward

  // if the game is paused maintain the paused state
  if (game_is_paused == true) {
    game_paused(current_led_position);

    // check if the user won the game
    if (current_led_position == green_led_pin && game_won == false && game_won_1 == false) {
      // set game flags to true
      game_won = true;
      game_won_1 = true;
      
      blink_green_led();

      // keep the green LED on afterwards
      digitalWrite(led_pin_number[green_led_pin], HIGH);
    }
  }
  else {
    // set first game flag to false
    game_won = false;

    // turn on the current position's LED and trailing LEDs during the unpause delay
    if (game_unpaused_recently == true) {
      light_current_LED(current_led_position, led_direction);

      if (millis() - unpause_time < unpause_delay_duration) {
        return;
      }
      else {
        // reset flag
        game_unpaused_recently = false;
      }
    }

    if (millis() - last_update_time >= time) {
      last_update_time = millis();

      turn_off_LEDs();

      current_led_position = current_led_position + led_direction;

      if (current_led_position >= number_of_leds) {
        led_direction = -1;
        current_led_position = number_of_leds - 1;
      }
      else if (current_led_position < 0) {
        led_direction = 1;
        current_led_position = 0;
      }

      if (led_direction == 1) {
        turn_on_LEDs_forward(current_led_position);
      }
      else {
        turn_on_LEDs_backward(current_led_position);
      }
    }
  }
}
