#include "MKL46Z4.h"

#include "Seg_LCD.h"
 // m?t n? c?u hình ng?t c?a ADC.
#define AIEN_ON ADC_SC1_AIEN_MASK
// m?t n? c?u hình d?u vào ADC là don c?c hay vi sai
#define DIFF_SINGLE 0x00
void ADC0_init(void);
int adc_read(uint8_t channel);
void delayMs(int n);
int main() {
  int result;
  uint16_t x = 1234;
  uint32_t i = 0;
  // kh?i t?o LCD và ADC0
  SystemClockConfiguration();
  SegLCD_Init();
  ADC0_init();
  //delayMs(1000);
  while (1) {
    result = adc_read(3); /* d?c k?t qu? chuy?n d?i ADC t? kênh 3 */
    x = (uint16_t)(result * (float)(330.0 / 65536));
    /* chuy?n d?i thành cu?ng d? ánh sáng */
    SegLCD_DisplayDecimal(x); // hi?n th? giá tr? trên LCD
    delayMs(1000);
  }
}
/* hàm kh?i t?o ADC0 */
void ADC0_init(void) {
  SIM -> SCGC5 |= SIM_SCGC5_PORTE(1) ; /* c?p clock t?i PORTE */
  PORTE -> PCR[22] = 0; /* c?u hình chân PTE22 làm d?u vào tuong t? */
  SIM -> SCGC6 |= SIM_SCGC6_ADC0(1); /* c?p clock t?i ADC0 */
  ADC0 -> SC2 &= ~(1 << ADC_SC2_ADTRG_SHIFT) ; /* ch?n kích ho?t b?ng ph?n m?m */
  ADC0 -> SC3 |= ADC_SC3_AVGE(1) | ADC_SC3_AVGS(3) ; /* cho phép tính trung bình 32 m?u */
  /* clock chia 4, th?i gian l?y m?u dài, 16-bit, bus clock */
  ADC0 -> CFG1 = ADC_CFG1_ADIV(2) | ADC_CFG1_ADLSMP(1) | ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0) ;
}
/* hàm d?c k?t qu? bi?n d?i AD */
int adc_read(uint8_t channel) {
  ADC0 -> SC1[0] = ADC_SC1_ADCH(channel) | ADC_SC1_DIFF(0) ; /* b?t d?u quá trình bi?n d?i AD trên kênh channel */
  while ( (ADC0 -> SC1[0] & ADC_SC1_COCO_MASK) == 0 ) {} /*d?i c? COCO */
	ADC0 -> SC1[0] &= ~( 1 << ADC_SC1_COCO_SHIFT );
  return ADC0->R[0]; /* d?c k?t qu? và xóa c? COCO */
}
/* t?o tr? n mili giây
 * Xung nh?p clock c?a lõi CPU du?c d?t t?i MCGFLLCLK ? t?n s? 48 MHz */
void delayMs(int n) {
  int i;
  int j;
  for (i = 0; i < n; i++)
    for (j = 0; j < 7000; j++) {}
}