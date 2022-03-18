
#ifndef MAIN_h
#define MAIN_h

#define I2S_BUFFER_SIZE                     16U

#define DMA_PDM_TX                          DMAREQ_SSP0_TX
#define DMA_PDM_RX                          SSP0_RX_DMA

#define PDM_SPI                             LPC_SSP0

/*
 * Local Data
 */
static uint16_t incoming_buffer_a[I2S_BUFFER_SIZE];
static uint16_t incoming_buffer_b[I2S_BUFFER_SIZE];

// we're going to set them both up to ping-pong.
static DMA_CHDESC_T dmaTXDesc[2] __attribute__ ((aligned(16)));
static DMA_CHDESC_T dmaRXDesc[2] __attribute__ ((aligned(16)));


void Start_DMA(void);

#endif
