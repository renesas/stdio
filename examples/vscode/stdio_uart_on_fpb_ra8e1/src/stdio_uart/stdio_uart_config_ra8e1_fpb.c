#include <stddef.h>
#include <string.h>

#include "rs_stdio_uart.h"

#if defined(BOARD_RA8E1_FPB)

#if !defined(R_SCI_B_UART_H)
#error "Please enable using the Smart Configurator configuration.xml > Components > Renesas > HAL Drivers > all > r_sci_b_uart."
#endif

sci_b_uart_instance_ctrl_t g_uart_jlob_vcom_ref_ctrl;

const sci_b_baud_setting_t g_uart_jlob_vcom_ref_baud_setting =
{
    /* Baud rate calculated with 3.340% error. */
    .baudrate_bits_b.abcse = 1,
    .baudrate_bits_b.abcs  = 0,
    .baudrate_bits_b.bgdm  = 0,
    .baudrate_bits_b.cks   = 0,
    .baudrate_bits_b.brr   = 6,
    .baudrate_bits_b.mddr  = (uint8_t)256,
    .baudrate_bits_b.brme  = false
};

/** UART extended configuration for UARTonSCI_B HAL driver */
const sci_b_uart_extended_cfg_t g_uart_jlob_vcom_ref_cfg_extend =
{
    .clock           = SCI_B_UART_CLOCK_INT,
    .rx_edge_start   = SCI_B_UART_START_BIT_FALLING_EDGE,
    .noise_cancel    = SCI_B_UART_NOISE_CANCELLATION_DISABLE,
    .rx_fifo_trigger = SCI_B_UART_RX_FIFO_TRIGGER_MAX,
    .p_baud_setting  = (sci_b_baud_setting_t*) &g_uart_jlob_vcom_ref_baud_setting,
    .flow_control    = SCI_B_UART_FLOW_CONTROL_RTS,

#if 0xFF != 0xFF
    .flow_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
#else
    .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif

    .rs485_setting =
    {
        .enable         = SCI_B_UART_RS485_DISABLE,
        .polarity       = SCI_B_UART_RS485_DE_POLARITY_HIGH,
        .assertion_time = 1,
        .negation_time  = 1,
    },

    .delay_cycles = 0,
};

/** UART interface configuration */
const uart_cfg_t g_uart_jlob_vcom_ref_cfg =
{
    .channel    = 9,
    .data_bits  = UART_DATA_BITS_8,
    .parity     = UART_PARITY_OFF,
    .stop_bits  = UART_STOP_BITS_1,
    .p_callback = NULL,
    .p_context  = NULL,
    .p_extend   = &g_uart_jlob_vcom_ref_cfg_extend,

#define RA_NOT_DEFINED (1)

#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
    .p_transfer_tx = NULL,
#else
    .p_transfer_tx = &RA_NOT_DEFINED,
#endif

#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
    .p_transfer_rx = NULL,
#else
    .p_transfer_rx = &RA_NOT_DEFINED,
#endif

#undef RA_NOT_DEFINED

    .rxi_ipl = (12),
    .txi_ipl = (12),
    .tei_ipl = (12),
    .eri_ipl = (12),

#if defined(VECTOR_NUMBER_SCI9_RXI)
    .rxi_irq = VECTOR_NUMBER_SCI9_RXI,
#else
    .rxi_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI9_TXI)
    .txi_irq = VECTOR_NUMBER_SCI9_TXI,
#else
    .txi_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI9_TEI)
    .tei_irq = VECTOR_NUMBER_SCI9_TEI,
#else
    .tei_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI9_ERI)
    .eri_irq = VECTOR_NUMBER_SCI9_ERI,
#else
    .eri_irq = FSP_INVALID_VECTOR,
#endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart_jlob_vcom_ref =
{
    .p_ctrl = &g_uart_jlob_vcom_ref_ctrl,
    .p_cfg  = &g_uart_jlob_vcom_ref_cfg,
    .p_api  = &g_uart_on_sci_b
};

rs_stdio_uart_cfg_t const rs_stdio_uart_ref =
{
    .rxd = IOPORT_PORT_01_PIN_01,
    .txd = IOPORT_PORT_01_PIN_02,

    .uart_instance = g_uart_jlob_vcom_ref,
    .uart_cfg = g_uart_jlob_vcom_ref_cfg,

    .sci_b_baud_setting = g_uart_jlob_vcom_ref_baud_setting,
    .sci_b_uart_extended_cfg = g_uart_jlob_vcom_ref_cfg_extend,
    .sci_b_uart_instance_ctrl = {0},
};

static fsp_err_t check_config_generics(
    rs_stdio_uart_cfg_t const * const p_cfg)
{
    int32_t diff = 0;

    FSP_ERROR_RETURN(NULL != p_cfg->uart_instance.p_api,
                     FSP_ERR_INVALID_POINTER);

    FSP_ERROR_RETURN(NULL != p_cfg->uart_instance.p_cfg,
                     FSP_ERR_INVALID_POINTER);

    FSP_ERROR_RETURN(NULL != p_cfg->uart_instance.p_ctrl,
                     FSP_ERR_INVALID_POINTER);

    FSP_ERROR_RETURN(&p_cfg->sci_b_uart_instance_ctrl == p_cfg->uart_instance.p_ctrl,
                     FSP_ERR_INVALID_POINTER);

    diff = memcmp(p_cfg->uart_instance.p_cfg, &p_cfg->uart_cfg, sizeof(p_cfg->uart_cfg));
    FSP_ERROR_RETURN( 0 == diff, FSP_ERR_INVALID_DATA);


    diff = memcmp(p_cfg->uart_instance.p_ctrl, &p_cfg->sci_b_uart_instance_ctrl, sizeof(p_cfg->sci_b_uart_instance_ctrl));
    FSP_ERROR_RETURN( 0 == diff, FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        &g_uart_on_sci_b ==
            p_cfg->uart_instance.p_api,
        FSP_ERR_INVALID_ARGUMENT);

    FSP_ERROR_RETURN(
        (ioport_port_pin_t)BSP_IO_PORT_FF_PIN_FF > p_cfg->txd,
        FSP_ERR_INVALID_ARGUMENT);

    FSP_ERROR_RETURN(
        (ioport_port_pin_t)BSP_IO_PORT_FF_PIN_FF > p_cfg->rxd,
        FSP_ERR_INVALID_ARGUMENT);

    FSP_ERROR_RETURN(
        BSP_IRQ_DISABLED !=
            p_cfg->uart_instance.p_cfg->rxi_ipl,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        BSP_IRQ_DISABLED !=
            p_cfg->uart_instance.p_cfg->txi_ipl,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        BSP_IRQ_DISABLED !=
            p_cfg->uart_instance.p_cfg->tei_ipl,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        FSP_INVALID_VECTOR !=
            p_cfg->uart_instance.p_cfg->rxi_irq,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        FSP_INVALID_VECTOR !=
            p_cfg->uart_instance.p_cfg->txi_irq,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        FSP_INVALID_VECTOR !=
            p_cfg->uart_instance.p_cfg->tei_irq,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        NULL !=
            p_cfg->uart_instance.p_cfg->p_extend,
        FSP_ERR_INVALID_POINTER);

    /*
     * Reference UART settings assume:
     *
     * HOCO  = 20 MHz
     * SCICLK = HOCO
     *
     * Different CGC configuration requires
     * regenerated baud settings.
     */

    FSP_ERROR_RETURN(
        UINT32_C(20000000) == R_BSP_SourceClockHzGet(FSP_PRIV_CLOCK_HOCO),
        FSP_ERR_INVALID_HW_CONDITION
    );

    FSP_ERROR_RETURN(
        (BSP_CFG_SCICLK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_HOCO),
        FSP_ERR_INVALID_HW_CONDITION
    );

    FSP_ERROR_RETURN(BSP_ICU_VECTOR_NUM_ENTRIES > p_cfg->uart_cfg.txi_irq, FSP_ERR_INVALID_DATA);
    FSP_ERROR_RETURN(BSP_ICU_VECTOR_NUM_ENTRIES > p_cfg->uart_cfg.rxi_irq, FSP_ERR_INVALID_DATA);
    FSP_ERROR_RETURN(BSP_ICU_VECTOR_NUM_ENTRIES > p_cfg->uart_cfg.tei_irq, FSP_ERR_INVALID_DATA);
    FSP_ERROR_RETURN(BSP_ICU_VECTOR_NUM_ENTRIES > p_cfg->uart_cfg.eri_irq, FSP_ERR_INVALID_DATA);

    {
        uint32_t lowest_prio = (1UL << __NVIC_PRIO_BITS) - 1UL;
        FSP_ERROR_RETURN(lowest_prio >= p_cfg->uart_cfg.txi_ipl, FSP_ERR_INVALID_DATA);
        FSP_ERROR_RETURN(lowest_prio >= p_cfg->uart_cfg.rxi_ipl, FSP_ERR_INVALID_DATA);
        FSP_ERROR_RETURN(lowest_prio >= p_cfg->uart_cfg.tei_ipl, FSP_ERR_INVALID_DATA);
        FSP_ERROR_RETURN(lowest_prio >= p_cfg->uart_cfg.eri_ipl, FSP_ERR_INVALID_DATA);
    }

    FSP_ERROR_RETURN(
        NULL !=
        ((sci_b_uart_extended_cfg_t const *)
            p_cfg->uart_instance.p_cfg->p_extend)->p_baud_setting,
        FSP_ERR_INVALID_POINTER);

    /* DMA/DTC transfer drivers are not supported. */
    FSP_ERROR_RETURN(
        NULL ==
            p_cfg->uart_instance.p_cfg->p_transfer_rx,
        FSP_ERR_INVALID_POINTER);

    /* DMA/DTC transfer drivers are not supported. */
    FSP_ERROR_RETURN(
        NULL ==
            p_cfg->uart_instance.p_cfg->p_transfer_tx,
        FSP_ERR_INVALID_POINTER);

    return FSP_SUCCESS;
}

static fsp_err_t check_config_exact(
    rs_stdio_uart_cfg_t const * const p_cfg)
{
    rs_stdio_uart_cfg_t const * const p_ref =
        &rs_stdio_uart_ref;

    /* RA8E1 FPB reference VCOM routing */
    FSP_ERROR_RETURN(
        IOPORT_PORT_01_PIN_01 == p_cfg->rxd,
        FSP_ERR_INVALID_ARGUMENT);

    /* RA8E1 FPB reference VCOM routing */
    FSP_ERROR_RETURN(
        IOPORT_PORT_01_PIN_02 == p_cfg->txd,
        FSP_ERR_INVALID_ARGUMENT);

    sci_b_uart_extended_cfg_t const * const p_extend_ref =
        (sci_b_uart_extended_cfg_t const *)
            p_ref->uart_instance.p_cfg->p_extend;

    sci_b_uart_extended_cfg_t const * const p_extend_actual =
        (sci_b_uart_extended_cfg_t const *)
            p_cfg->uart_instance.p_cfg->p_extend;

    FSP_ERROR_RETURN(
        p_extend_ref->clock ==
            p_extend_actual->clock,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        p_extend_ref->rx_edge_start ==
            p_extend_actual->rx_edge_start,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        p_extend_ref->noise_cancel ==
            p_extend_actual->noise_cancel,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        p_extend_ref->rx_fifo_trigger ==
            p_extend_actual->rx_fifo_trigger,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        p_extend_ref->flow_control ==
            p_extend_actual->flow_control,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        p_extend_ref->flow_control_pin ==
            p_extend_actual->flow_control_pin,
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        0 == memcmp(
                &p_extend_ref->rs485_setting,
                &p_extend_actual->rs485_setting,
                sizeof(p_extend_actual->rs485_setting)),
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        p_extend_ref->delay_cycles ==
            p_extend_actual->delay_cycles,
        FSP_ERR_INVALID_DATA);

    sci_b_baud_setting_t const * const p_baud_ref =
        p_extend_ref->p_baud_setting;

    sci_b_baud_setting_t const * const p_baud_actual =
        p_extend_actual->p_baud_setting;

    FSP_ERROR_RETURN(
        0 == memcmp(p_baud_ref,
                    p_baud_actual,
                    sizeof(sci_b_baud_setting_t)),
        FSP_ERR_INVALID_DATA);

    return FSP_SUCCESS;
}

fsp_err_t rs_stdio_uart_check_config(
    rs_stdio_uart_cfg_t const * const p_cfg)
{
    FSP_ERROR_RETURN(NULL != p_cfg,
                     FSP_ERR_INVALID_ARGUMENT);

    fsp_err_t err = check_config_generics(p_cfg);

    FSP_ERROR_RETURN(FSP_SUCCESS == err,
                     err);

    err = check_config_exact(p_cfg);

    FSP_ERROR_RETURN(FSP_SUCCESS == err,
                     err);

    return err;
}

fsp_err_t rs_stdio_config_gen(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t config_size = 0;
    rs_stdio_uart_cfg_t generated_config;

    err = RS_STDIO_UART_GetConfigSize(&config_size);

    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    err = RS_STDIO_UART_GenConfig(&generated_config, sizeof(generated_config));

    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return err;
}

fsp_err_t RS_STDIO_UART_CheckApplicationConfiguration(void)
{
    extern sci_b_baud_setting_t g_uart_jlob_vcom_baud_setting;
    extern sci_b_uart_instance_ctrl_t g_uart_jlob_vcom_ctrl;
    extern uart_instance_t const g_uart_jlob_vcom;
    extern const uart_cfg_t g_uart_jlob_vcom_cfg;
    extern const sci_b_uart_extended_cfg_t g_uart_jlob_vcom_cfg_extend;

    rs_stdio_uart_cfg_t check =
    {
        .rxd = IOPORT_PORT_01_PIN_01,
        .txd = IOPORT_PORT_01_PIN_02,

        .uart_instance = g_uart_jlob_vcom,
        .uart_cfg = g_uart_jlob_vcom_cfg,

        .sci_b_baud_setting =
            g_uart_jlob_vcom_baud_setting,

        .sci_b_uart_extended_cfg =
            g_uart_jlob_vcom_cfg_extend,

        .sci_b_uart_instance_ctrl =
            g_uart_jlob_vcom_ctrl,
    };

    check.uart_instance.p_cfg = &check.uart_cfg;
    check.uart_instance.p_ctrl = &check.sci_b_uart_instance_ctrl;

    return RS_STDIO_UART_CheckConfig(&check);
}

#endif /* defined(BOARD_RA8E1_FPB) */
