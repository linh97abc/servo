	component DE0_CV_QSYS is
		port (
			clk_clk                                     : in    std_logic                     := 'X';             -- clk
			clk_5m_clk                                  : out   std_logic;                                        -- clk
			clk_sdram_clk                               : out   std_logic;                                        -- clk
			ltc2992_i2c_sda_t                           : out   std_logic;                                        -- sda_t
			ltc2992_i2c_scl_t                           : out   std_logic;                                        -- scl_t
			ltc2992_i2c_sda_i                           : in    std_logic                     := 'X';             -- sda_i
			ltc2992_i2c_scl_i                           : in    std_logic                     := 'X';             -- scl_i
			pll_locked_export                           : out   std_logic;                                        -- export
			reset_reset_n                               : in    std_logic                     := 'X';             -- reset_n
			sdram_wire_addr                             : out   std_logic_vector(12 downto 0);                    -- addr
			sdram_wire_ba                               : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_wire_cas_n                            : out   std_logic;                                        -- cas_n
			sdram_wire_cke                              : out   std_logic;                                        -- cke
			sdram_wire_cs_n                             : out   std_logic;                                        -- cs_n
			sdram_wire_dq                               : inout std_logic_vector(15 downto 0) := (others => 'X'); -- dq
			sdram_wire_dqm                              : out   std_logic_vector(1 downto 0);                     -- dqm
			sdram_wire_ras_n                            : out   std_logic;                                        -- ras_n
			sdram_wire_we_n                             : out   std_logic;                                        -- we_n
			servo_controllerv1_0_conduit_end_spi_sclk   : out   std_logic;                                        -- spi_sclk
			servo_controllerv1_0_conduit_end_spi_cs     : out   std_logic;                                        -- spi_cs
			servo_controllerv1_0_conduit_end_spi_miso   : in    std_logic                     := 'X';             -- spi_miso
			servo_controllerv1_0_conduit_end_spi_mosi   : out   std_logic;                                        -- spi_mosi
			servo_controllerv1_0_conduit_end_hall_0     : in    std_logic_vector(2 downto 0)  := (others => 'X'); -- hall_0
			servo_controllerv1_0_conduit_end_hall_1     : in    std_logic_vector(2 downto 0)  := (others => 'X'); -- hall_1
			servo_controllerv1_0_conduit_end_hall_2     : in    std_logic_vector(2 downto 0)  := (others => 'X'); -- hall_2
			servo_controllerv1_0_conduit_end_hall_3     : in    std_logic_vector(2 downto 0)  := (others => 'X'); -- hall_3
			servo_controllerv1_0_conduit_end_phase_0    : out   std_logic_vector(5 downto 0);                     -- phase_0
			servo_controllerv1_0_conduit_end_phase_1    : out   std_logic_vector(5 downto 0);                     -- phase_1
			servo_controllerv1_0_conduit_end_phase_2    : out   std_logic_vector(5 downto 0);                     -- phase_2
			servo_controllerv1_0_conduit_end_phase_3    : out   std_logic_vector(5 downto 0);                     -- phase_3
			servo_controllerv1_0_conduit_end_nFault     : in    std_logic_vector(3 downto 0)  := (others => 'X'); -- nFault
			servo_controllerv1_0_conduit_end_drv8320_en : out   std_logic_vector(3 downto 0);                     -- drv8320_en
			spi_ad7928_external_MISO                    : in    std_logic                     := 'X';             -- MISO
			spi_ad7928_external_MOSI                    : out   std_logic;                                        -- MOSI
			spi_ad7928_external_SCLK                    : out   std_logic;                                        -- SCLK
			spi_ad7928_external_SS_n                    : out   std_logic;                                        -- SS_n
			tmp101_i2c_sda_t                            : out   std_logic;                                        -- sda_t
			tmp101_i2c_scl_t                            : out   std_logic;                                        -- scl_t
			tmp101_i2c_sda_i                            : in    std_logic                     := 'X';             -- sda_i
			tmp101_i2c_scl_i                            : in    std_logic                     := 'X';             -- scl_i
			uart_rs485_conduit_end_rxd                  : in    std_logic                     := 'X';             -- rxd
			uart_rs485_conduit_end_txd                  : out   std_logic;                                        -- txd
			uart_rs485_conduit_end_dbg_os_pulse         : out   std_logic                                         -- dbg_os_pulse
		);
	end component DE0_CV_QSYS;

	u0 : component DE0_CV_QSYS
		port map (
			clk_clk                                     => CONNECTED_TO_clk_clk,                                     --                              clk.clk
			clk_5m_clk                                  => CONNECTED_TO_clk_5m_clk,                                  --                           clk_5m.clk
			clk_sdram_clk                               => CONNECTED_TO_clk_sdram_clk,                               --                        clk_sdram.clk
			ltc2992_i2c_sda_t                           => CONNECTED_TO_ltc2992_i2c_sda_t,                           --                      ltc2992_i2c.sda_t
			ltc2992_i2c_scl_t                           => CONNECTED_TO_ltc2992_i2c_scl_t,                           --                                 .scl_t
			ltc2992_i2c_sda_i                           => CONNECTED_TO_ltc2992_i2c_sda_i,                           --                                 .sda_i
			ltc2992_i2c_scl_i                           => CONNECTED_TO_ltc2992_i2c_scl_i,                           --                                 .scl_i
			pll_locked_export                           => CONNECTED_TO_pll_locked_export,                           --                       pll_locked.export
			reset_reset_n                               => CONNECTED_TO_reset_reset_n,                               --                            reset.reset_n
			sdram_wire_addr                             => CONNECTED_TO_sdram_wire_addr,                             --                       sdram_wire.addr
			sdram_wire_ba                               => CONNECTED_TO_sdram_wire_ba,                               --                                 .ba
			sdram_wire_cas_n                            => CONNECTED_TO_sdram_wire_cas_n,                            --                                 .cas_n
			sdram_wire_cke                              => CONNECTED_TO_sdram_wire_cke,                              --                                 .cke
			sdram_wire_cs_n                             => CONNECTED_TO_sdram_wire_cs_n,                             --                                 .cs_n
			sdram_wire_dq                               => CONNECTED_TO_sdram_wire_dq,                               --                                 .dq
			sdram_wire_dqm                              => CONNECTED_TO_sdram_wire_dqm,                              --                                 .dqm
			sdram_wire_ras_n                            => CONNECTED_TO_sdram_wire_ras_n,                            --                                 .ras_n
			sdram_wire_we_n                             => CONNECTED_TO_sdram_wire_we_n,                             --                                 .we_n
			servo_controllerv1_0_conduit_end_spi_sclk   => CONNECTED_TO_servo_controllerv1_0_conduit_end_spi_sclk,   -- servo_controllerv1_0_conduit_end.spi_sclk
			servo_controllerv1_0_conduit_end_spi_cs     => CONNECTED_TO_servo_controllerv1_0_conduit_end_spi_cs,     --                                 .spi_cs
			servo_controllerv1_0_conduit_end_spi_miso   => CONNECTED_TO_servo_controllerv1_0_conduit_end_spi_miso,   --                                 .spi_miso
			servo_controllerv1_0_conduit_end_spi_mosi   => CONNECTED_TO_servo_controllerv1_0_conduit_end_spi_mosi,   --                                 .spi_mosi
			servo_controllerv1_0_conduit_end_hall_0     => CONNECTED_TO_servo_controllerv1_0_conduit_end_hall_0,     --                                 .hall_0
			servo_controllerv1_0_conduit_end_hall_1     => CONNECTED_TO_servo_controllerv1_0_conduit_end_hall_1,     --                                 .hall_1
			servo_controllerv1_0_conduit_end_hall_2     => CONNECTED_TO_servo_controllerv1_0_conduit_end_hall_2,     --                                 .hall_2
			servo_controllerv1_0_conduit_end_hall_3     => CONNECTED_TO_servo_controllerv1_0_conduit_end_hall_3,     --                                 .hall_3
			servo_controllerv1_0_conduit_end_phase_0    => CONNECTED_TO_servo_controllerv1_0_conduit_end_phase_0,    --                                 .phase_0
			servo_controllerv1_0_conduit_end_phase_1    => CONNECTED_TO_servo_controllerv1_0_conduit_end_phase_1,    --                                 .phase_1
			servo_controllerv1_0_conduit_end_phase_2    => CONNECTED_TO_servo_controllerv1_0_conduit_end_phase_2,    --                                 .phase_2
			servo_controllerv1_0_conduit_end_phase_3    => CONNECTED_TO_servo_controllerv1_0_conduit_end_phase_3,    --                                 .phase_3
			servo_controllerv1_0_conduit_end_nFault     => CONNECTED_TO_servo_controllerv1_0_conduit_end_nFault,     --                                 .nFault
			servo_controllerv1_0_conduit_end_drv8320_en => CONNECTED_TO_servo_controllerv1_0_conduit_end_drv8320_en, --                                 .drv8320_en
			spi_ad7928_external_MISO                    => CONNECTED_TO_spi_ad7928_external_MISO,                    --              spi_ad7928_external.MISO
			spi_ad7928_external_MOSI                    => CONNECTED_TO_spi_ad7928_external_MOSI,                    --                                 .MOSI
			spi_ad7928_external_SCLK                    => CONNECTED_TO_spi_ad7928_external_SCLK,                    --                                 .SCLK
			spi_ad7928_external_SS_n                    => CONNECTED_TO_spi_ad7928_external_SS_n,                    --                                 .SS_n
			tmp101_i2c_sda_t                            => CONNECTED_TO_tmp101_i2c_sda_t,                            --                       tmp101_i2c.sda_t
			tmp101_i2c_scl_t                            => CONNECTED_TO_tmp101_i2c_scl_t,                            --                                 .scl_t
			tmp101_i2c_sda_i                            => CONNECTED_TO_tmp101_i2c_sda_i,                            --                                 .sda_i
			tmp101_i2c_scl_i                            => CONNECTED_TO_tmp101_i2c_scl_i,                            --                                 .scl_i
			uart_rs485_conduit_end_rxd                  => CONNECTED_TO_uart_rs485_conduit_end_rxd,                  --           uart_rs485_conduit_end.rxd
			uart_rs485_conduit_end_txd                  => CONNECTED_TO_uart_rs485_conduit_end_txd,                  --                                 .txd
			uart_rs485_conduit_end_dbg_os_pulse         => CONNECTED_TO_uart_rs485_conduit_end_dbg_os_pulse          --                                 .dbg_os_pulse
		);

