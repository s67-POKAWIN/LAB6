// Define analog input pin connected to the capacitor voltage
const int AIN_PIN = A0;

// Digital pin used to charge the capacitor C through resistor R1
const int CHARGE_PIN = 9;

// Digital pin used to discharge the capacitor C through resistor R2
const int DISCHARGE_PIN = 10;

// Minimum ADC value to avoid noise near 0V
#define ADC_MIN (4)

// Maximum ADC value to avoid noise near VCC
#define ADC_MAX (1023 - ADC_MIN)

// Midpoint ADC value (~VCC/2), used to detect half-time
#define ADC_MID ((ADC_MIN + ADC_MAX) / 2)

// Resistance value in ohms (used for capacitance calculation)
#define R1 1000

// Natural logarithm of 2 (used in RC half-time formula)
#define LN2 0.6931472

// Variable to store the start time of capacitor charging
uint32_t t_start;

void setup() {
  // Initialize the serial port for PC communication, set baudrate to 115200
  Serial.begin(115200);

  // Configure charging pin as output
  pinMode(CHARGE_PIN, OUTPUT);

  // Configure discharge pin as high-impedance input initially
  pinMode(DISCHARGE_PIN, INPUT);

  // Discharge C initially
  digitalWrite(CHARGE_PIN, LOW);
  delay(1000);
}

void loop() {
  // --- CHARGING PHASE ---
  // Enable charging path
  pinMode(CHARGE_PIN, OUTPUT);
  pinMode(DISCHARGE_PIN, INPUT);

  // Start charging C
  digitalWrite(CHARGE_PIN, HIGH);

  // Save the start time
  t_start = micros();

  // Wait until capacitor voltage reaches ~VCC/2
  while (analogRead(AIN_PIN) < ADC_MID);
  
  // Calculate time elapsed
  uint32_t t_us = micros() - t_start;

  // Continue charging until near full voltage
  while (analogRead(AIN_PIN) < ADC_MAX);

  // --- DISCHARGING PHASE ---
  // Enable discharging path
  pinMode(CHARGE_PIN, INPUT);
  pinMode(DISCHARGE_PIN, OUTPUT);

  // Start discharging C
  digitalWrite(DISCHARGE_PIN, LOW);

  // Wait until capacitor is fully discharged
  while (analogRead(AIN_PIN) > ADC_MIN);

  // --- CALCULATION & OUTPUT ---
  String s;
  s = "half-time [us] = ";
  s += t_us;
  Serial.println(s);

  // Compute capacitance using C = t / (R * ln(2))
  float capacitance = t_us / (R1 * LN2);

  s = "R1 [Ohm] = ";
  s += R1;
  Serial.println(s);

  s = "capacitance [uF] = "; // Note: Output is likely in microfarads due to micros()
  s += capacitance;
  Serial.println(s);
  
  Serial.println("-----------------------");
}
