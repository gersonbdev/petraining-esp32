// PINOUTS
#define SCREW_MOTOR 27 // Digital pin output D27 -> endless screw
#define LINEAR_MOTOR_R 25 // Digital pin output D34 -> linear actuator right turn (H bridge)
#define LINEAR_MOTOR_L 33 // Digital pin output D35 -> linear actuator left turn (H bridge)
#define DOUT_INT 19 // Data output pin 19 HX711 internal
#define CLK_INT 18 // Clock output pin 18 HX711 internal
#define DOUT_EXT 13 // Data output pin 13 HX711 external
#define CLK_EXT 14 // Clock output pin 14 HX711 external
// SCL -> 22 & SDA -> 21 || RTC

// CONSTANTS
#define TIME_SUP 20 // Hours in 24h format
#define TIME_INF 6 // Hours in 24h format
#define CONFIG_EASY 1 // Easy configuratioon preset for quick starting for user
#define CONFIG_CUSTOM 2 // Custom configuratioon preset for quick starting for user
#define MAX_DISPENSE_PER_DAY 10 // Maximum times dispensation will happen per day
#define TIMEOUT_DISPENSE 20 // Maximun time screw motor will be turning, if exceeded, dispense will stop (seconds)
#define LINEAR_ACTUATOR_TIME_DELAY 14 // Time linear actuator will move forewards and backwards in seconds

// MACROS
#define SETUP_EASY (EEPROM.read(0) == CONFIG_EASY) // Preset easy configuration -> 1
#define SETUP_CUSTOM (EEPROM.read(0) == CONFIG_CUSTOM) // Preset customized configuration -> 2
#define TIME_WINDOW (TIME_SUP - TIME_INF) // Window time in hours in 24h format