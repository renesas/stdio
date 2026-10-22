#include <stddef.h>
#include <string.h>

#include "rs_stdio_uart.h"

#if defined(BOARD_RA0L1_FPB)

#if !defined(R_UARTA_H)
#error "Please enable using the Smart Configurator configuration.xml > Components > Renesas > HAL Drivers > all > r_uarta."
#endif

uarta_instance_ctrl_t g_uart_jlob_vcom_ref_ctrl;

const uarta_baud_setting_t g_uart_jlob_vcom_ref_baud_setting =
{
#if (BSP_CFG_UARTA1_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC)

    /* Baud rate calculated with Actual_Error0.22%. */
    /* The permissible baud rate error range during reception: -5.15% ~ 5.14% */
    .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_MOSC,
    .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1,
    .brgca = 87,
    .delay_time = 1,

#elif (BSP_CFG_UARTA1_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_HOCO)

    /* Baud rate calculated with Actual_Error0.08%. */
    /* The permissible baud rate error range during reception: -5.19% ~ 5.18% */
    .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_HOCO,
    .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1,
    .brgca = 139,
    .delay_time = 1,

#elif (BSP_CFG_UARTA1_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_MOCO)

    /* Baud rate calculated with Actual_Error2.12%. */
    /* The permissible baud rate error range during reception: -4.67% ~ 4.62% */
    .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_MOCO,
    .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1,
    .brgca = 17,
    .delay_time = 1,

#elif ((BSP_CFG_UARTA1_CLOCK_SOURCE == BSP_CFG_FSXP_SOURCE) || \
       (BSP_CFG_UARTA1_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_LOCO))

    /* Baud rate calculated with Actual_Error100%. */
    /* The permissible baud rate error range during reception: Invalid Range Error */
    .utanck_clock_b.utasel = 0,
    .utanck_clock_b.utanck = 0,
    .brgca = 0,
    .delay_time = 31,

#elif (BSP_CFG_UARTA1_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK)

    /* Baud rate calculated with Actual_Error100%. */
    /* The permissible baud rate error range during reception: Invalid Range Error */
    .utanck_clock_b.utasel = 0,
    .utanck_clock_b.utanck = 0,
    .brgca = 0,
    .delay_time = 31,

#endif
};

/** UART extended configuration for UART on UARTA HAL driver */
const uarta_extended_cfg_t g_uart_jlob_vcom_ref_cfg_extend =
{
    .transfer_dir = UARTA_DIR_BIT_LSB_FIRST,
    .transfer_level = UARTA_ALV_BIT_POSITIVE_LOGIC,
    .clock_output = UARTA_CLOCK_OUTPUT_DISABLED,
    .p_baud_setting = (uarta_baud_setting_t *)&g_uart_jlob_vcom_ref_baud_setting,
};

/** UART interface configuration */
const uart_cfg_t g_uart_jlob_vcom_ref_cfg =
{
    .channel = 1,
    .data_bits = UART_DATA_BITS_8,
    .parity = UART_PARITY_OFF,
    .stop_bits = UART_STOP_BITS_1,
    .p_callback = NULL,
    .p_context = NULL,
    .p_extend = &g_uart_jlob_vcom_ref_cfg_extend,

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

    .rxi_ipl = (2),
    .txi_ipl = (2),

#if defined(VECTOR_NUMBER_UARTA1_RXI)
    .rxi_irq = VECTOR_NUMBER_UARTA1_RXI,
#else
    .rxi_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_UARTA1_TXI)
    .txi_irq = VECTOR_NUMBER_UARTA1_TXI,
#else
    .txi_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_UARTA1_ERRI)
    .eri_irq = VECTOR_NUMBER_UARTA1_ERRI,
#else
    .eri_irq = FSP_INVALID_VECTOR,
#endif
};

/* Instance structure to use this module. */
const uart_instance_t g_uart_jlob_vcom_ref =
{
    .p_ctrl = &g_uart_jlob_vcom_ref_ctrl,
    .p_cfg = &g_uart_jlob_vcom_ref_cfg,
    .p_api = &g_uart_on_uarta,
};

rs_stdio_uart_cfg_t const rs_stdio_uart_ref =
{
    .rxd = IOPORT_PORT_01_PIN_05,
    .txd = IOPORT_PORT_01_PIN_06,

    .uart_instance = g_uart_jlob_vcom_ref,
    .uart_cfg = g_uart_jlob_vcom_ref_cfg,

    .uarta_baud_setting = g_uart_jlob_vcom_ref_baud_setting,
    .uarta_extended_cfg = g_uart_jlob_vcom_ref_cfg_extend,
    .uarta_instance_ctrl = {0},
};

static fsp_err_t check_config_generics(rs_stdio_uart_cfg_t const * const p_cfg)
{
    int32_t diff = 0;
    
    FSP_ERROR_RETURN((NULL != p_cfg->uart_instance.p_api), FSP_ERR_INVALID_POINTER);
    FSP_ERROR_RETURN((NULL != p_cfg->uart_instance.p_cfg), FSP_ERR_INVALID_POINTER);
    FSP_ERROR_RETURN((NULL != p_cfg->uart_instance.p_ctrl), FSP_ERR_INVALID_POINTER);
    FSP_ERROR_RETURN((&g_uart_on_uarta == p_cfg->uart_instance.p_api), FSP_ERR_INVALID_ARGUMENT);

    FSP_ERROR_RETURN(&p_cfg->uarta_instance_ctrl == p_cfg->uart_instance.p_ctrl,
                     FSP_ERR_INVALID_POINTER);

    diff = memcmp(p_cfg->uart_instance.p_cfg, &p_cfg->uart_cfg, sizeof(p_cfg->uart_cfg));
    FSP_ERROR_RETURN( 0 == diff, FSP_ERR_INVALID_DATA);


    diff = memcmp(p_cfg->uart_instance.p_ctrl, &p_cfg->uarta_instance_ctrl, sizeof(p_cfg->uarta_instance_ctrl));
    FSP_ERROR_RETURN( 0 == diff, FSP_ERR_INVALID_DATA);

    /* Minimal checks */
    FSP_ERROR_RETURN(((ioport_port_pin_t)BSP_IO_PORT_FF_PIN_FF > p_cfg->txd), FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN(((ioport_port_pin_t)BSP_IO_PORT_FF_PIN_FF > p_cfg->rxd), FSP_ERR_INVALID_ARGUMENT);

    FSP_ERROR_RETURN((BSP_IRQ_DISABLED != p_cfg->uart_instance.p_cfg->rxi_ipl), FSP_ERR_INVALID_DATA);
    FSP_ERROR_RETURN((BSP_IRQ_DISABLED != p_cfg->uart_instance.p_cfg->txi_ipl), FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN((FSP_INVALID_VECTOR != p_cfg->uart_instance.p_cfg->rxi_irq), FSP_ERR_INVALID_DATA);
    FSP_ERROR_RETURN((FSP_INVALID_VECTOR != p_cfg->uart_instance.p_cfg->txi_irq), FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN((NULL != p_cfg->uart_instance.p_cfg->p_extend), FSP_ERR_INVALID_POINTER);
    FSP_ERROR_RETURN(
        (NULL != ((uarta_extended_cfg_t const *) p_cfg->uart_instance.p_cfg->p_extend)->p_baud_setting),
        FSP_ERR_INVALID_POINTER);

    /*
     * DMA/DTC transfer drivers are not supported.
     */
    FSP_ERROR_RETURN((NULL == p_cfg->uart_instance.p_cfg->p_transfer_rx), FSP_ERR_INVALID_POINTER);

    /*
     * DMA/DTC transfer drivers are not supported.
     */
    FSP_ERROR_RETURN((NULL == p_cfg->uart_instance.p_cfg->p_transfer_tx), FSP_ERR_INVALID_POINTER);

    return FSP_SUCCESS;
}

static fsp_err_t check_config_exact(rs_stdio_uart_cfg_t const * const p_cfg)
{
    rs_stdio_uart_cfg_t const * const p_ref = &rs_stdio_uart_ref;

    /*
     * RA0L1 FPB reference VCOM routing.
     */
    FSP_ERROR_RETURN((IOPORT_PORT_01_PIN_05 == p_cfg->rxd), FSP_ERR_INVALID_ARGUMENT);

    /*
     * RA0L1 FPB reference VCOM routing.
     */
    FSP_ERROR_RETURN((IOPORT_PORT_01_PIN_06 == p_cfg->txd), FSP_ERR_INVALID_ARGUMENT);

    /* Check extended settings */
    uarta_extended_cfg_t const * const p_extend_ref =
        (uarta_extended_cfg_t const *) p_ref->uart_instance.p_cfg->p_extend;

    uarta_extended_cfg_t const * const p_extend_actual =
        (uarta_extended_cfg_t const *) p_cfg->uart_instance.p_cfg->p_extend;

    FSP_ERROR_RETURN((p_extend_ref->transfer_dir == p_extend_actual->transfer_dir), FSP_ERR_INVALID_DATA);
    FSP_ERROR_RETURN((p_extend_ref->transfer_level == p_extend_actual->transfer_level), FSP_ERR_INVALID_DATA);
    FSP_ERROR_RETURN((p_extend_ref->clock_output == p_extend_actual->clock_output), FSP_ERR_INVALID_DATA);

    /*
     * Reference UART settings assume:
     *
     * HOCO  = 32 MHz
     * PCLKA = 100 MHz
     *
     * Different CGC configuration requires
     * regenerated baud settings.
     */

    FSP_ERROR_RETURN(
            UINT32_C(32000000) == R_BSP_SourceClockHzGet(FSP_PRIV_CLOCK_HOCO),
            FSP_ERR_INVALID_HW_CONDITION
    );
    FSP_ERROR_RETURN(
        (BSP_CFG_UTASEL_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_HOCO),
        FSP_ERR_INVALID_HW_CONDITION
    );

    FSP_ERROR_RETURN(BSP_ICU_VECTOR_NUM_ENTRIES > p_cfg->uart_cfg.txi_irq, FSP_ERR_INVALID_DATA);
    FSP_ERROR_RETURN(BSP_ICU_VECTOR_NUM_ENTRIES > p_cfg->uart_cfg.rxi_irq, FSP_ERR_INVALID_DATA);

    {
        uint32_t lowest_prio = (1UL << __NVIC_PRIO_BITS) - 1UL;
        FSP_ERROR_RETURN(lowest_prio >= p_cfg->uart_cfg.txi_ipl, FSP_ERR_INVALID_DATA);
        FSP_ERROR_RETURN(lowest_prio >= p_cfg->uart_cfg.rxi_ipl, FSP_ERR_INVALID_DATA);
    }

    /* Check baud settings */
    uarta_baud_setting_t const * const p_baud_ref =
        p_extend_ref->p_baud_setting;

    uarta_baud_setting_t const * const p_baud_actual =
        p_extend_actual->p_baud_setting;

    FSP_ERROR_RETURN(
        (p_baud_ref->utanck_clock_b.utasel == p_baud_actual->utanck_clock_b.utasel),
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        (p_baud_ref->utanck_clock_b.utanck == p_baud_actual->utanck_clock_b.utanck),
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        (p_baud_ref->brgca == p_baud_actual->brgca),
        FSP_ERR_INVALID_DATA);

    FSP_ERROR_RETURN(
        (p_baud_ref->delay_time == p_baud_actual->delay_time),
        FSP_ERR_INVALID_DATA);

    return FSP_SUCCESS;
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

fsp_err_t rs_stdio_uart_check_config(rs_stdio_uart_cfg_t const * const p_cfg)
{
    FSP_ERROR_RETURN(NULL != p_cfg, FSP_ERR_INVALID_ARGUMENT);

    fsp_err_t err = FSP_SUCCESS;

    err = check_config_generics(p_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    err = check_config_exact(p_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return err;
}

fsp_err_t RS_STDIO_UART_CheckApplicationConfiguration(void)
{
    extern uarta_baud_setting_t g_uart_jlob_vcom_baud_setting;
    extern uart_instance_t const g_uart_jlob_vcom;
    extern const uart_cfg_t g_uart_jlob_vcom_cfg;
    extern const uarta_extended_cfg_t g_uart_jlob_vcom_cfg_extend;
    extern uarta_instance_ctrl_t g_uart_jlob_vcom_ctrl;

    rs_stdio_uart_cfg_t check =
    {
        .rxd = IOPORT_PORT_01_PIN_05,
        .txd = IOPORT_PORT_01_PIN_06,

        .uart_instance = g_uart_jlob_vcom,
        .uart_cfg = g_uart_jlob_vcom_cfg,

        .uarta_baud_setting = g_uart_jlob_vcom_baud_setting,
        .uarta_extended_cfg = g_uart_jlob_vcom_cfg_extend,
        .uarta_instance_ctrl = g_uart_jlob_vcom_ctrl,
    };

    check.uart_instance.p_cfg = &check.uart_cfg;
    check.uart_instance.p_ctrl = &check.uarta_instance_ctrl;

    return RS_STDIO_UART_CheckConfig(&check);
}

#endif /* defined(BOARD_RA0L1_FPB) */
