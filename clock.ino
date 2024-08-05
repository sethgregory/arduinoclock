#include "Arduino.h"
#include <Wire.h>
#include <I2C_RTC.h>

static DS3231 RTC;

// Set this value to true if you need to set your clock
bool set_time = false;

// If you need to set the initial time on your clock
// please change these values below
#define SET_HOURS   12    // set to current hours in 24-hour time (e.g., 15 is 3pm)
#define SET_MINUTES 33    // set to current minutes
#define SET_SECONDS 0     // set to current seconds
#define SET_DAY     28    // set to day of the month
#define SET_MONTH   7     // set to month of the year (e.g., 7 is July)
#define SET_YEAR    2024  // set to current year
#define SET_WEEK    1     // set to day of the week (sun=1, sat=7)

// Set this value to true if you would like debug output on the serial console
bool debug_mode = true;

int minutepin = 9;  // PWM pin for minutes
int hourpin   = 10; // PWM pin for hours

// Function to convert RTC time to analog values and write to output pins
void setDisplay(float seconds, float minutes, float hours){
  //Minutes is 0-59 hours is 0-12
  if(minutes < 0 || minutes > 60){
    Serial.println("Error minutes out of range, Minutes: " + String(minutes));
    return;
  }
  if(hours < 0 || hours > 12){
    Serial.println("Error hours out of range, Hours: " + String(hours));
    return;
  }

  // Adjust hours value down by one since meter begins at 1 o'clock
  hours = hours - 1;

  // Account for having subtracted one from "zero o'clock" case
  if(hours < 0) {
    hours = 11;
  }

  // Prepare values to write to output pins
  int minute_converted = ((minutes)/60.0) * 255;
  int hour_converted = ((hours)/11.0) * 255;

  // Write values to output pins
  analogWrite(minutepin, minute_converted);
  analogWrite(hourpin, hour_converted);

  if(debug_mode) {
    Serial.print("minute_converted: ");
    Serial.print(minute_converted);
    Serial.print("  hour_converted: ");
    Serial.println(hour_converted);
    Serial.println("");
  }
}

// Print the current RTC datetime value to serial output
void debugOutputTime() {
  switch (RTC.getWeek())
  {
    case 1:
      Serial.print("SUN");
      break;
    case 2:
      Serial.print("MON");
      break;
    case 3:
      Serial.print("TUE");
      break;
    case 4:
      Serial.print("WED");
      break;
    case 5:
      Serial.print("THU");
      break;
    case 6:
      Serial.print("FRI");
      break;
    case 7:
      Serial.print("SAT");
      break;
  }
  Serial.print(" ");
  Serial.print(RTC.getMonth());
  Serial.print("-");
  Serial.print(RTC.getDay());
  Serial.print("-");
  Serial.print(RTC.getYear());

  Serial.print(" ");

  Serial.print(RTC.getHours());
  Serial.print(":");
  Serial.print(RTC.getMinutes());
  Serial.print(":");
  Serial.print(RTC.getSeconds());
  Serial.print(" ");

  if (RTC.getHourMode() == CLOCK_H12)
  {
    if(RTC.getMeridiem() == HOUR_AM)
      Serial.println(" AM");
    if (RTC.getMeridiem() == HOUR_PM)
      Serial.println(" PM");     
  }
}

void setup()
{
  Serial.begin(9600);
  RTC.begin();

  // Set 12-hour clock mode
  RTC.setHourMode(CLOCK_H12);

  // If the boolean is set above, set the initial datetime
  if(set_time) {
    RTC.setTime(SET_HOURS, SET_MINUTES, SET_SECONDS);
    RTC.setDate(SET_DAY, SET_MONTH, SET_YEAR);
    RTC.setWeek(SET_WEEK);
  }

  // Set both pins to OUTPUT mode
  pinMode(minutepin, OUTPUT);
  pinMode(hourpin, OUTPUT);

  // Initialize the meters and write the full range of values
  for (int pos = 0; pos <= 255; pos += 1) { // PWM position goes from 0 degrees to 255 degrees
    // in steps of 1 degree
    analogWrite(minutepin, pos);
    analogWrite(hourpin, pos);
    delay(10);                       // waits 10 ms for the servo to reach the position
  }

  // Wait one second before displaying correct time
  delay(1000);
  
  analogWrite(minutepin, 0);
  analogWrite(hourpin, 0);
}

void loop()
{
  // Print the current datetime to the Serial output
  if(debug_mode) {
    debugOutputTime();
  }

  // Update the meters
  setDisplay(RTC.getSeconds(), RTC.getMinutes(), RTC.getHours());

  // Wait 1 second before looping
  delay(1000);
}
