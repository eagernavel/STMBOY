#include <stddef.h>
#include <stdint.h>

#include "db_lut.h"
/*
 bit | GPIO
  0     PA9
  1     PC7
  2     PA10
  3     PB3
  4     PB5
  5     PB4
  6     PB10
  7     PA8
*/

/******************  Bits definition for GPIO_ODR register  *******************/
#define GPIO_ODR_OD3_Pos              (3U)
#define GPIO_ODR_OD3_Msk              (0x1UL << GPIO_ODR_OD3_Pos)            /*!< 0x00000008 */
#define GPIO_ODR_OD4_Pos              (4U)
#define GPIO_ODR_OD4_Msk              (0x1UL << GPIO_ODR_OD4_Pos)            /*!< 0x00000010 */
#define GPIO_ODR_OD5_Pos              (5U)
#define GPIO_ODR_OD5_Msk              (0x1UL << GPIO_ODR_OD5_Pos)            /*!< 0x00000020 */
#define GPIO_ODR_OD7_Pos              (7U)
#define GPIO_ODR_OD7_Msk              (0x1UL << GPIO_ODR_OD7_Pos)            /*!< 0x00000080 */
#define GPIO_ODR_OD8_Pos              (8U)
#define GPIO_ODR_OD8_Msk              (0x1UL << GPIO_ODR_OD8_Pos)            /*!< 0x00000100 */
#define GPIO_ODR_OD9_Pos              (9U)
#define GPIO_ODR_OD9_Msk              (0x1UL << GPIO_ODR_OD9_Pos)            /*!< 0x00000200 */
#define GPIO_ODR_OD10_Pos             (10U)
#define GPIO_ODR_OD10_Msk             (0x1UL << GPIO_ODR_OD10_Pos)           /*!< 0x00000400 */

/******************  Bits definition for GPIO_BSRR register  ******************/
#define GPIO_BSRR_BS3_Pos             (3U)
#define GPIO_BSRR_BS3_Msk             (0x1UL << GPIO_BSRR_BS3_Pos)           /*!< 0x00000008 */
#define GPIO_BSRR_BS4_Pos             (4U)
#define GPIO_BSRR_BS4_Msk             (0x1UL << GPIO_BSRR_BS4_Pos)           /*!< 0x00000010 */
#define GPIO_BSRR_BS5_Pos             (5U)
#define GPIO_BSRR_BS5_Msk             (0x1UL << GPIO_BSRR_BS5_Pos)           /*!< 0x00000020 */
#define GPIO_BSRR_BS7_Pos             (7U)
#define GPIO_BSRR_BS7_Msk             (0x1UL << GPIO_BSRR_BS7_Pos)           /*!< 0x00000080 */
#define GPIO_BSRR_BS8_Pos             (8U)
#define GPIO_BSRR_BS8_Msk             (0x1UL << GPIO_BSRR_BS8_Pos)           /*!< 0x00000100 */
#define GPIO_BSRR_BS9_Pos             (9U)
#define GPIO_BSRR_BS9_Msk             (0x1UL << GPIO_BSRR_BS9_Pos)           /*!< 0x00000200 */
#define GPIO_BSRR_BS10_Pos            (10U)
#define GPIO_BSRR_BS10_Msk            (0x1UL << GPIO_BSRR_BS10_Pos)          /*!< 0x00000400 */
#define GPIO_BSRR_BR3_Pos             (19U)
#define GPIO_BSRR_BR3_Msk             (0x1UL << GPIO_BSRR_BR3_Pos)           /*!< 0x00080000 */
#define GPIO_BSRR_BR4_Pos             (20U)
#define GPIO_BSRR_BR4_Msk             (0x1UL << GPIO_BSRR_BR4_Pos)           /*!< 0x00100000 */
#define GPIO_BSRR_BR5_Pos             (21U)
#define GPIO_BSRR_BR5_Msk             (0x1UL << GPIO_BSRR_BR5_Pos)           /*!< 0x00200000 */
#define GPIO_BSRR_BR7_Pos             (23U)
#define GPIO_BSRR_BR7_Msk             (0x1UL << GPIO_BSRR_BR7_Pos)           /*!< 0x00800000 */
#define GPIO_BSRR_BR8_Pos             (24U)
#define GPIO_BSRR_BR8_Msk             (0x1UL << GPIO_BSRR_BR8_Pos)           /*!< 0x01000000 */
#define GPIO_BSRR_BR9_Pos             (25U)
#define GPIO_BSRR_BR9_Msk             (0x1UL << GPIO_BSRR_BR9_Pos)           /*!< 0x02000000 */
#define GPIO_BSRR_BR10_Pos            (26U)
#define GPIO_BSRR_BR10_Msk            (0x1UL << GPIO_BSRR_BR10_Pos)          /*!< 0x04000000 */

/**
  * @brief General Purpose I/O
  */
typedef struct
{
  volatile uint32_t MODER;
  volatile uint32_t OTYPER;
  volatile uint32_t OSPEEDR;
  volatile uint32_t PUPDR;
  volatile uint32_t IDR;
  volatile uint32_t ODR;
  volatile uint32_t BSRR;
  volatile uint32_t LCKR;
  volatile uint32_t AFR[2];
} GPIO_TypeDef;

#define PERIPH_BASE           0x40000000UL /*!< Peripheral base address in the alias region */
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)

#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800UL)

#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)

void prv_db70_write_with_branches(uint8_t data)
{
    uint32_t bsrrA = GPIO_BSRR_BR9_Msk | GPIO_BSRR_BR10_Msk | GPIO_BSRR_BR8_Msk;
    uint32_t bsrrB = GPIO_BSRR_BR3_Msk | GPIO_BSRR_BR5_Msk | GPIO_BSRR_BR4_Msk | GPIO_BSRR_BR10_Msk;
    uint32_t bsrrC = GPIO_BSRR_BR7_Msk;

    if (data & (1 << 0))
        bsrrA |= GPIO_BSRR_BS9_Msk;
    if (data & (1 << 1))
        bsrrC |= GPIO_BSRR_BS7_Msk;
    if (data & (1 << 2))
        bsrrA |= GPIO_BSRR_BS10_Msk;
    if (data & (1 << 3))
        bsrrB |= GPIO_BSRR_BS3_Msk;
    if (data & (1 << 4))
        bsrrB |= GPIO_BSRR_BS5_Msk;
    if (data & (1 << 5))
        bsrrB |= GPIO_BSRR_BS4_Msk;
    if (data & (1 << 6))
        bsrrB |= GPIO_BSRR_BS10_Msk;
    if (data & (1 << 7))
        bsrrA |= GPIO_BSRR_BS8_Msk;

    GPIOA->BSRR = bsrrA;
    GPIOB->BSRR = bsrrB;
    GPIOC->BSRR = bsrrC;
}

void prv_db70_write_optimized(uint8_t data)
{
    // Preserve register state
    uint32_t odrA = GPIOA->ODR;
    uint32_t odrB = GPIOB->ODR;
    uint32_t odrC = GPIOC->ODR;

    // Clear all pin states
    odrA &= ~(GPIO_ODR_OD9_Msk | GPIO_ODR_OD10_Msk | GPIO_ODR_OD8_Msk);
    odrB &= ~(GPIO_ODR_OD3_Msk | GPIO_ODR_OD5_Msk | GPIO_ODR_OD4_Msk | GPIO_ODR_OD10_Msk);
    odrC &= ~(GPIO_ODR_OD7_Msk);

    // Set new values
    // bit | GPIO  | Optmimized expression
    //  0    PA9     ((data & 0x01U) << 9U)
    //  1    PC7     ((data & 0x02U) << 6U)
    //  2    PA10    ((data & 0x04U) << 8U)
    //  3    PB3     (data & 0x08U)
    //  4    PB5     ((data & 0x10U) << 1U)
    //  5    PB4     ((data & 0x20U) >> 1U)
    //  6    PB10    ((data & 0x40U) << 4U)
    //  7    PA8     ((data & 0x80U) << 1U)

    odrA |= ((data & 0x01U) << 9U) | ((data & 0x04U) << 8U) | ((data & 0x80U) << 1U);
    odrB |= (data & 0x08U) | ((data & 0x10U) << 1U) | ((data & 0x20U) >> 1U) | ((data & 0x40U) << 4U);
    odrC |=  ((data & 0x02U) << 6U);

    // Write back to ODR
    GPIOA->ODR = odrA;
    GPIOB->ODR = odrB;
    GPIOC->ODR = odrC;
}

void prv_db70_write_with_lut(uint8_t data)
{
    const DBLutEntry entry = g_dbLut[data];
    GPIOA->BSRR = entry.bsrrA;
    GPIOB->BSRR = entry.bsrrB;
    GPIOC->BSRR = entry.bsrrC;
}
