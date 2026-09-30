#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdarg.h>
#include "rcc.h"
#include "gpio.h"

#ifndef PERIPH_BASE
#define PERIPH_BASE 0x40000000UL
#endif

#define APB1PERIPH_BASE       (PERIPH_BASE)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)

#define USART2_BASE           (APB1PERIPH_BASE + 0x4400UL) /* 0x40004400 */
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000UL) /* 0x40011000 */
#define USART6_BASE           (APB2PERIPH_BASE + 0x1400UL) /* 0x40011400 */

#define USART1                ((USART_TypeDef *)USART1_BASE)
#define USART2                ((USART_TypeDef *)USART2_BASE)
#define USART6                ((USART_TypeDef *)USART6_BASE)

/********************  Bit definition for USART_SR register  ********************/
#define USART_SR_PE_Pos             (0U)
#define USART_SR_PE_Msk             (0x1UL << USART_SR_PE_Pos)      /*!< 0x00000001 */
#define USART_SR_PE                 USART_SR_PE_Msk
#define USART_SR_FE_Pos             (1U)
#define USART_SR_FE_Msk             (0x1UL << USART_SR_FE_Pos)      /*!< 0x00000002 */
#define USART_SR_FE                 USART_SR_FE_Msk
#define USART_SR_NF_Pos             (2U)
#define USART_SR_NF_Msk             (0x1UL << USART_SR_NF_Pos)      /*!< 0x00000004 */
#define USART_SR_NF                 USART_SR_NF_Msk
#define USART_SR_ORE_Pos            (3U)
#define USART_SR_ORE_Msk            (0x1UL << USART_SR_ORE_Pos)     /*!< 0x00000008 */
#define USART_SR_ORE                USART_SR_ORE_Msk
#define USART_SR_IDLE_Pos           (4U)
#define USART_SR_IDLE_Msk           (0x1UL << USART_SR_IDLE_Pos)    /*!< 0x00000010 */
#define USART_SR_IDLE               USART_SR_IDLE_Msk
#define USART_SR_RXNE_Pos           (5U)
#define USART_SR_RXNE_Msk           (0x1UL << USART_SR_RXNE_Pos)    /*!< 0x00000020 */
#define USART_SR_RXNE               USART_SR_RXNE_Msk
#define USART_SR_TC_Pos             (6U)
#define USART_SR_TC_Msk             (0x1UL << USART_SR_TC_Pos)      /*!< 0x00000040 */
#define USART_SR_TC                 USART_SR_TC_Msk
#define USART_SR_TXE_Pos            (7U)
#define USART_SR_TXE_Msk            (0x1UL << USART_SR_TXE_Pos)     /*!< 0x00000080 */
#define USART_SR_TXE                USART_SR_TXE_Msk
#define USART_SR_LBD_Pos            (8U)
#define USART_SR_LBD_Msk            (0x1UL << USART_SR_LBD_Pos)     /*!< 0x00000100 */
#define USART_SR_LBD                USART_SR_LBD_Msk
#define USART_SR_CTS_Pos            (9U)
#define USART_SR_CTS_Msk            (0x1UL << USART_SR_CTS_Pos)     /*!< 0x00000200 */
#define USART_SR_CTS                USART_SR_CTS_Msk

/********************  Bit definition for USART_BRR register  ********************/
#define USART_BRR_DIV_FRACTION_Pos  (0U)
#define USART_BRR_DIV_FRACTION_Msk  (0xFUL << USART_BRR_DIV_FRACTION_Pos)   /*!< 0x0000000F */
#define USART_BRR_DIV_FRACTION      USART_BRR_DIV_FRACTION_Msk
#define USART_BRR_DIV_MANTISSA_Pos  (4U)
#define USART_BRR_DIV_MANTISSA_Msk  (0xFFFUL << USART_BRR_DIV_MANTISSA_Pos) /*!< 0x0000FFF0 */
#define USART_BRR_DIV_MANTISSA      USART_BRR_DIV_MANTISSA_Msk

/********************  Bit definition for USART_CR1 register  ********************/
#define USART_CR1_SBK_Pos           (0U)
#define USART_CR1_SBK_Msk           (0x1UL << USART_CR1_SBK_Pos)    /*!< 0x00000001 */
#define USART_CR1_SBK               USART_CR1_SBK_Msk
#define USART_CR1_RWU_Pos           (1U)
#define USART_CR1_RWU_Msk           (0x1UL << USART_CR1_RWU_Pos)    /*!< 0x00000002 */
#define USART_CR1_RWU               USART_CR1_RWU_Msk
#define USART_CR1_RE_Pos            (2U)
#define USART_CR1_RE_Msk            (0x1UL << USART_CR1_RE_Pos)     /*!< 0x00000004 */
#define USART_CR1_RE                USART_CR1_RE_Msk
#define USART_CR1_TE_Pos            (3U)
#define USART_CR1_TE_Msk            (0x1UL << USART_CR1_TE_Pos)     /*!< 0x00000008 */
#define USART_CR1_TE                USART_CR1_TE_Msk
#define USART_CR1_IDLEIE_Pos        (4U)
#define USART_CR1_IDLEIE_Msk        (0x1UL << USART_CR1_IDLEIE_Pos) /*!< 0x00000010 */
#define USART_CR1_IDLEIE            USART_CR1_IDLEIE_Msk
#define USART_CR1_RXNEIE_Pos        (5U)
#define USART_CR1_RXNEIE_Msk        (0x1UL << USART_CR1_RXNEIE_Pos) /*!< 0x00000020 */
#define USART_CR1_RXNEIE            USART_CR1_RXNEIE_Msk
#define USART_CR1_TCIE_Pos          (6U)
#define USART_CR1_TCIE_Msk          (0x1UL << USART_CR1_TCIE_Pos)   /*!< 0x00000040 */
#define USART_CR1_TCIE              USART_CR1_TCIE_Msk
#define USART_CR1_TXEIE_Pos         (7U)
#define USART_CR1_TXEIE_Msk         (0x1UL << USART_CR1_TXEIE_Pos)  /*!< 0x00000080 */
#define USART_CR1_TXEIE             USART_CR1_TXEIE_Msk
#define USART_CR1_PEIE_Pos          (8U)
#define USART_CR1_PEIE_Msk          (0x1UL << USART_CR1_PEIE_Pos)   /*!< 0x00000100 */
#define USART_CR1_PEIE              USART_CR1_PEIE_Msk
#define USART_CR1_PS_Pos            (9U)
#define USART_CR1_PS_Msk            (0x1UL << USART_CR1_PS_Pos)     /*!< 0x00000200 */
#define USART_CR1_PS                USART_CR1_PS_Msk
#define USART_CR1_PCE_Pos           (10U)
#define USART_CR1_PCE_Msk           (0x1UL << USART_CR1_PCE_Pos)    /*!< 0x00000400 */
#define USART_CR1_PCE               USART_CR1_PCE_Msk
#define USART_CR1_WAKE_Pos          (11U)
#define USART_CR1_WAKE_Msk          (0x1UL << USART_CR1_WAKE_Pos)   /*!< 0x00000800 */
#define USART_CR1_WAKE              USART_CR1_WAKE_Msk
#define USART_CR1_M_Pos             (12U)
#define USART_CR1_M_Msk             (0x1UL << USART_CR1_M_Pos)      /*!< 0x00001000 */
#define USART_CR1_M                 USART_CR1_M_Msk
#define USART_CR1_UE_Pos            (13U)
#define USART_CR1_UE_Msk            (0x1UL << USART_CR1_UE_Pos)     /*!< 0x00002000 */
#define USART_CR1_UE                USART_CR1_UE_Msk
#define USART_CR1_OVER8_Pos         (15U)
#define USART_CR1_OVER8_Msk         (0x1UL << USART_CR1_OVER8_Pos)  /*!< 0x00008000 */
#define USART_CR1_OVER8             USART_CR1_OVER8_Msk

/********************  Bit definition for USART_CR2 register  ********************/
#define USART_CR2_ADD_Pos           (0U)
#define USART_CR2_ADD_Msk           (0xFUL << USART_CR2_ADD_Pos)    /*!< 0x0000000F */
#define USART_CR2_ADD               USART_CR2_ADD_Msk
#define USART_CR2_LBDL_Pos          (5U)
#define USART_CR2_LBDL_Msk          (0x1UL << USART_CR2_LBDL_Pos)   /*!< 0x00000020 */
#define USART_CR2_LBDL              USART_CR2_LBDL_Msk
#define USART_CR2_LBDIE_Pos         (6U)
#define USART_CR2_LBDIE_Msk         (0x1UL << USART_CR2_LBDIE_Pos)  /*!< 0x00000040 */
#define USART_CR2_LBDIE             USART_CR2_LBDIE_Msk
#define USART_CR2_LBCL_Pos          (8U)
#define USART_CR2_LBCL_Msk          (0x1UL << USART_CR2_LBCL_Pos)   /*!< 0x00000100 */
#define USART_CR2_LBCL              USART_CR2_LBCL_Msk
#define USART_CR2_CPHA_Pos          (9U)
#define USART_CR2_CPHA_Msk          (0x1UL << USART_CR2_CPHA_Pos)   /*!< 0x00000200 */
#define USART_CR2_CPHA              USART_CR2_CPHA_Msk
#define USART_CR2_CPOL_Pos          (10U)
#define USART_CR2_CPOL_Msk          (0x1UL << USART_CR2_CPOL_Pos)   /*!< 0x00000400 */
#define USART_CR2_CPOL              USART_CR2_CPOL_Msk
#define USART_CR2_CLKEN_Pos         (11U)
#define USART_CR2_CLKEN_Msk         (0x1UL << USART_CR2_CLKEN_Pos)  /*!< 0x00000800 */
#define USART_CR2_CLKEN             USART_CR2_CLKEN_Msk
#define USART_CR2_STOP_Pos          (12U)
#define USART_CR2_STOP_Msk          (0x3UL << USART_CR2_STOP_Pos)   /*!< 0x00003000 */
#define USART_CR2_STOP              USART_CR2_STOP_Msk
#define USART_CR2_STOP_0            (0x1UL << USART_CR2_STOP_Pos)   /*!< 0x00001000 */
#define USART_CR2_STOP_1            (0x2UL << USART_CR2_STOP_Pos)   /*!< 0x00002000 */
#define USART_CR2_LINEN_Pos         (14U)
#define USART_CR2_LINEN_Msk         (0x1UL << USART_CR2_LINEN_Pos)  /*!< 0x00004000 */
#define USART_CR2_LINEN             USART_CR2_LINEN_Msk

/********************  Bit definition for USART_CR3 register  ********************/
#define USART_CR3_EIE_Pos           (0U)
#define USART_CR3_EIE_Msk           (0x1UL << USART_CR3_EIE_Pos)    /*!< 0x00000001 */
#define USART_CR3_EIE               USART_CR3_EIE_Msk
#define USART_CR3_IREN_Pos          (1U)
#define USART_CR3_IREN_Msk          (0x1UL << USART_CR3_IREN_Pos)   /*!< 0x00000002 */
#define USART_CR3_IREN              USART_CR3_IREN_Msk
#define USART_CR3_IRLP_Pos          (2U)
#define USART_CR3_IRLP_Msk          (0x1UL << USART_CR3_IRLP_Pos)   /*!< 0x00000004 */
#define USART_CR3_IRLP              USART_CR3_IRLP_Msk
#define USART_CR3_HDSEL_Pos         (3U)
#define USART_CR3_HDSEL_Msk         (0x1UL << USART_CR3_HDSEL_Pos)  /*!< 0x00000008 */
#define USART_CR3_HDSEL             USART_CR3_HDSEL_Msk
#define USART_CR3_NACK_Pos          (4U)
#define USART_CR3_NACK_Msk          (0x1UL << USART_CR3_NACK_Pos)   /*!< 0x00000010 */
#define USART_CR3_NACK              USART_CR3_NACK_Msk
#define USART_CR3_SCEN_Pos          (5U)
#define USART_CR3_SCEN_Msk          (0x1UL << USART_CR3_SCEN_Pos)   /*!< 0x00000020 */
#define USART_CR3_SCEN              USART_CR3_SCEN_Msk
#define USART_CR3_DMAR_Pos          (6U)
#define USART_CR3_DMAR_Msk          (0x1UL << USART_CR3_DMAR_Pos)   /*!< 0x00000040 */
#define USART_CR3_DMAR              USART_CR3_DMAR_Msk
#define USART_CR3_DMAT_Pos          (7U)
#define USART_CR3_DMAT_Msk          (0x1UL << USART_CR3_DMAT_Pos)   /*!< 0x00000080 */
#define USART_CR3_DMAT              USART_CR3_DMAT_Msk
#define USART_CR3_RTSE_Pos          (8U)
#define USART_CR3_RTSE_Msk          (0x1UL << USART_CR3_RTSE_Pos)   /*!< 0x00000100 */
#define USART_CR3_RTSE              USART_CR3_RTSE_Msk
#define USART_CR3_CTSE_Pos          (9U)
#define USART_CR3_CTSE_Msk          (0x1UL << USART_CR3_CTSE_Pos)   /*!< 0x00000200 */
#define USART_CR3_CTSE              USART_CR3_CTSE_Msk
#define USART_CR3_CTSIE_Pos         (10U)
#define USART_CR3_CTSIE_Msk         (0x1UL << USART_CR3_CTSIE_Pos)  /*!< 0x00000400 */
#define USART_CR3_CTSIE             USART_CR3_CTSIE_Msk
#define USART_CR3_ONEBIT_Pos        (11U)
#define USART_CR3_ONEBIT_Msk        (0x1UL << USART_CR3_ONEBIT_Pos) /*!< 0x00000800 */
#define USART_CR3_ONEBIT            USART_CR3_ONEBIT_Msk

typedef struct{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
}USART_TypeDef;

uint32_t USART_GetBaudRate(uint32_t baudrate);
void USART_Init(USART_TypeDef *USART ,uint32_t baudrate);
void USART_WriteByte(USART_TypeDef *USART, uint8_t data);
uint8_t USART_ReadByte(USART_TypeDef *USART);
void USART_Write(USART_TypeDef *USART, const char *str, uint32_t size);
void USART_Printf(USART_TypeDef *USART,const char *fmt,...);


#endif
