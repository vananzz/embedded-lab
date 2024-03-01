void LED_Initialize ( void ); // Initialize GPIO
void LED_On ( void ); // Switch LED on
void LED_Off ( void ); // Switch LED off
void blink_LED ( void const *argument ); // Blink LEDs in a thread
void Init_BlinkyThread ( void ); // Initialize thread