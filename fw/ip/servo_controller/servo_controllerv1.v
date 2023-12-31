module servo_controllerv1 (
    //common
    clk,
    reset_n,

    address,
    writedata,
    write_n,
    read_n,
    readdata,

    irq,

    // measure
    spi_cs,
    spi_sclk,
    spi_mosi,
    spi_miso,

    // In
    hall_0,  // a, b, c
    hall_1,  // a, b, c
    hall_2,  // a, b, c
    hall_3,  // a, b, c

    nFault,
    drv8320_en,

    // Out
    phase_0,
    phase_1,
    phase_2,
    phase_3
);

  parameter FREQ_CLK = 100_000_000;
  parameter SPI_SPEED = 1_000_000;
  parameter PWM_BASE_FREQ = 10_000_000;
  parameter PWM_FREQ = 1000;
  parameter [1:0] C_MODE0 = 0;
  parameter [1:0] C_MODE1 = 0;
  parameter [1:0] C_MODE2 = 0;
  parameter [1:0] C_MODE3 = 0;


  localparam DUTY_WIDTH = 16;
  localparam ADC_WIDTH = 12;
  localparam SPI_PRESCALER = FREQ_CLK / SPI_SPEED - 1;
  localparam PWM_PRES = FREQ_CLK / PWM_BASE_FREQ - 1;
  localparam PWM_HALF_PERIOD = PWM_BASE_FREQ / (2 * PWM_FREQ) - 1;

  localparam SERVO_NUM = 4;

  input wire clk;
  input wire reset_n;

  //avalon mm interface
  input wire [5:0] address;
  input wire [31:0] writedata;
  input wire write_n;
  input wire read_n;
  output reg [31:0] readdata;
  output irq;


  // spi
  output wire spi_cs;
  output wire spi_sclk;
  output wire spi_mosi;
  input wire spi_miso;

  input [2:0] hall_0;
  input [2:0] hall_1;
  input [2:0] hall_2;
  input [2:0] hall_3;

  input [SERVO_NUM-1:0] nFault;
  output reg [SERVO_NUM-1:0] drv8320_en;

  output [5:0] phase_0;
  output [5:0] phase_1;
  output [5:0] phase_2;
  output [5:0] phase_3;

  localparam CR_OFFSET = 0,  // protect_en , filter_level, en
  TR_OFFSET = 1,  // reset, start_servo, adc_init, u_valid
  IE_OFFSET = 2,
        FLAG_OFFSET = 3,

        SPI_PRES_OFFSET = 4,
        PWM_PRES_OFFSET = 5,
        PWM_TRIG_RATE = 6,

        PULSE_MODE0_OFFSET = 7,
        PULSE_MODE1_OFFSET = PULSE_MODE0_OFFSET+1,
        PULSE_MODE2_OFFSET = PULSE_MODE0_OFFSET+2,
        PULSE_MODE3_OFFSET = PULSE_MODE0_OFFSET+3,
        
        U0_OFFSET = 11,
        U1_OFFSET = U0_OFFSET+1,
        U2_OFFSET = U0_OFFSET+2,
        U3_OFFSET = U0_OFFSET+3,

        I0_MAX_OFFSET = 15,
        I1_MAX_OFFSET = I0_MAX_OFFSET+1,
        I2_MAX_OFFSET = I0_MAX_OFFSET+2,
        I3_MAX_OFFSET = I0_MAX_OFFSET+3,

        I0_OFFSET = 19,
        I1_OFFSET = I0_OFFSET+1,
        I2_OFFSET = I0_OFFSET+2,
        I3_OFFSET = I0_OFFSET+3,

        POS0_OFFSET = 23,
        POS1_OFFSET = POS0_OFFSET+1,
        POS2_OFFSET = POS0_OFFSET+2,
        POS3_OFFSET = POS0_OFFSET+3,

        DPOS_HALL0_OFFSET = 27,
        DPOS_HALL1_OFFSET = DPOS_HALL0_OFFSET+1,
        DPOS_HALL2_OFFSET = DPOS_HALL0_OFFSET+2,
        DPOS_HALL3_OFFSET = DPOS_HALL0_OFFSET+3,

        POS_HALL0_OFFSET = 31,
        POS_HALL1_OFFSET = POS_HALL0_OFFSET+1,
        POS_HALL2_OFFSET = POS_HALL0_OFFSET+2,
        POS_HALL3_OFFSET = POS_HALL0_OFFSET+3,

        HALL_OFFSET = 35;

  localparam 
    FLAG_CONTROL_PENDING_BIT = 15,
    FLAG_HALL_ERR_BIT = 11,
    FLAG_DRV8320_FAULT_BIT = 7,
    FLAG_STOP_BIT = 3,
    FLAG_ADC_VALID_BIT = 2,
    FLAG_MEA_TRIG_BIT = 1,
    FLAG_REAL_TIME_BIT = 0;

  reg                                 core_en;
  reg  [                         7:0] spi_divisor;
  reg                                 adc_init;
  reg  [                         3:0] filter_level;
  reg  [                         5:0] trig_rate;

  // m
  reg  [               ADC_WIDTH-1:0] i0_sample;
  reg  [               ADC_WIDTH-1:0] i1_sample;
  reg  [               ADC_WIDTH-1:0] i2_sample;
  reg  [               ADC_WIDTH-1:0] i3_sample;
  wire [               ADC_WIDTH-1:0] i0;
  wire [               ADC_WIDTH-1:0] i1;
  wire [               ADC_WIDTH-1:0] i2;
  wire [               ADC_WIDTH-1:0] i3;
  wire [               ADC_WIDTH-1:0] pos0;
  wire [               ADC_WIDTH-1:0] pos1;
  wire [               ADC_WIDTH-1:0] pos2;
  wire [               ADC_WIDTH-1:0] pos3;
  wire                                adc_data_valid_sig;
  reg                                 adc_data_valid;

  reg  [              DUTY_WIDTH-2:0] pwm_prescaler;
  reg  [              DUTY_WIDTH-2:0] half_period;
  reg  [               SERVO_NUM-1:0] start_servo;
  reg  [               SERVO_NUM-1:0] protected_en;
  reg  [              DUTY_WIDTH-1:0] u0;
  reg  [              DUTY_WIDTH-1:0] u1;
  reg  [              DUTY_WIDTH-1:0] u2;
  reg  [              DUTY_WIDTH-1:0] u3;
  reg                                 u_valid;

  reg  [               ADC_WIDTH-1:0] i0_max;
  reg  [               ADC_WIDTH-1:0] i1_max;
  reg  [               ADC_WIDTH-1:0] i2_max;
  reg  [               ADC_WIDTH-1:0] i3_max;

  reg  [                         1:0] mode_0;
  reg  [                         1:0] mode_1;
  reg  [                         1:0] mode_2;
  reg  [                         1:0] mode_3;

  wire                                measurement_trigger;
  wire [               SERVO_NUM-1:0] stop;


  reg                                 measurement_trigger_pendding;
  reg                                 realtime_err;
  reg                                 control_step_pending;

  reg  [FLAG_CONTROL_PENDING_BIT-1:0] ie;
  wire [  FLAG_CONTROL_PENDING_BIT:0] flag;

  reg                                 core_reset;

  reg  [               SERVO_NUM-1:0] Fault_tmp;
  reg  [               SERVO_NUM-1:0] Fault_sync0;
  reg  [               SERVO_NUM-1:0] Fault_sync1;
  reg  [               SERVO_NUM-1:0] drv8320_fault;

  reg  [                         5:0] hall_0_sync;
  reg  [                         5:0] hall_1_sync;
  reg  [                         5:0] hall_2_sync;
  reg  [                         5:0] hall_3_sync;

  wire [                        15:0] dpos_hall_0;
  wire [                        15:0] dpos_hall_1;
  wire [                        15:0] dpos_hall_2;
  wire [                        15:0] dpos_hall_3;


  wire [                        31:0] pos_hall_0;
  wire [                        31:0] pos_hall_1;
  wire [                        31:0] pos_hall_2;
  wire [                        31:0] pos_hall_3;

  wire [               SERVO_NUM-1:0] hall_err;


  assign irq = |(ie & flag[FLAG_CONTROL_PENDING_BIT-1:0]);
  assign flag = {
    control_step_pending,
    hall_err,
    drv8320_fault & drv8320_en,
    stop,
    adc_data_valid,
    measurement_trigger_pendding,
    realtime_err
  };

  detect_hall_pos detect_hall_pos_inst0 (
      .clk(clk),
      .reset_n(reset_n & ~core_reset),
      .hall(hall_0_sync[5:3]),
      .mea_trigger(measurement_trigger),
      .delta_pos(dpos_hall_0),
      .position(pos_hall_0),
      .pos_init(writedata),
      .pos_init_valid(((address == POS_HALL0_OFFSET) && ~write_n)),
      .hall_err(hall_err[0])
  );

  detect_hall_pos detect_hall_pos_inst1 (
      .clk(clk),
      .reset_n(reset_n & ~core_reset),
      .hall(hall_1_sync[5:3]),
      .mea_trigger(measurement_trigger),
      .delta_pos(dpos_hall_1),
      .position(pos_hall_1),
      .pos_init(writedata),
      .pos_init_valid(((address == POS_HALL1_OFFSET) && ~write_n)),
      .hall_err(hall_err[1])
  );

  detect_hall_pos detect_hall_pos_inst2 (
      .clk(clk),
      .reset_n(reset_n & ~core_reset),
      .hall(hall_2_sync[5:3]),
      .mea_trigger(measurement_trigger),
      .delta_pos(dpos_hall_2),
      .position(pos_hall_2),
      .pos_init(writedata),
      .pos_init_valid(((address == POS_HALL2_OFFSET) && ~write_n)),
      .hall_err(hall_err[2])
  );

  detect_hall_pos detect_hall_pos_inst3 (
      .clk(clk),
      .reset_n(reset_n & ~core_reset),
      .hall(hall_3_sync[5:3]),
      .mea_trigger(measurement_trigger),
      .delta_pos(dpos_hall_3),
      .position(pos_hall_3),
      .pos_init(writedata),
      .pos_init_valid(((address == POS_HALL3_OFFSET) && ~write_n)),
      .hall_err(hall_err[3])
  );

  ad7928_top #(
      .CORE_FREQUENCY(FREQ_CLK)
  ) measure_inst (
      .clk(clk),
      .reset_n(reset_n & ~core_reset),
      .core_en(core_en),
      .spi_divisor(spi_divisor),
      .adc_init(adc_init),

      .spi_cs  (spi_cs),
      .spi_sclk(spi_sclk),
      .spi_mosi(spi_mosi),
      .spi_miso(spi_miso),

      .filter_level(filter_level),

      .m_data_0(pos2),
      .m_data_1(pos3),
      .m_data_2(pos0),
      .m_data_3(pos1),
      .m_data_4(i3),
      .m_data_5(i2),
      .m_data_6(i0),
      .m_data_7(i1),

      .m_valid(adc_data_valid_sig)

  );

  servo_pwm_control #(
      .CORE_FREQUENCY(FREQ_CLK),
      .DUTY_WIDTH(DUTY_WIDTH),
      .ADC_WIDTH(ADC_WIDTH)
  ) servo_pwm_inst (
      .clk(clk),
      .reset_n(reset_n & ~core_reset),
      .core_en(core_en),
      .prescaler(pwm_prescaler),
      .half_period(half_period),

      // start
      .start_servo(start_servo),
      .protected_en(protected_en),
      .trig_rate(trig_rate),

      // slave 
      .u0(u0),  // -1 .. 1
      .u1(u1),
      .u2(u2),
      .u3(u3),
      .s_valid(u_valid),

      .measurement_trigger(measurement_trigger),

      //
      .i0(i0_sample),
      .i1(i1_sample),
      .i2(i2_sample),
      .i3(i3_sample),

      .i0_max(i0_max),
      .i1_max(i1_max),
      .i2_max(i2_max),
      .i3_max(i3_max),

      .mode_0(mode_0),
      .mode_1(mode_1),
      .mode_2(mode_2),
      .mode_3(mode_3),


      // In
      .hall_0 (hall_0_sync[5:3]),  // a, b, c
      .hall_1 (hall_1_sync[5:3]),  // a, b, c
      .hall_2 (hall_2_sync[5:3]),  // a, b, c
      .hall_3 (hall_3_sync[5:3]),  // a, b, c
      // Out
      .phase_0(phase_0),
      .phase_1(phase_1),
      .phase_2(phase_2),
      .phase_3(phase_3),

      .stop(stop)
  );

  localparam CR_EN_BIT = 0, CR_FILTER_LEVEL_BIT = 1, CR_PROTECT_EN_BIT = 5, CR_DRV_EN_BIT = 9;


  localparam TR_U_VALID_BIT = 0, TR_ADC_INIT_BIT = 1, TR_START_SERVO_BIT = 2, TR_RESET_BIT = 6;


  reg adc_data_valid_tmp;
  always @(posedge clk) begin
    if (~reset_n) begin
      adc_data_valid_tmp <= 1'b0;
      adc_data_valid <= 1'b0;
    end else begin

      if (adc_data_valid_sig) begin
        adc_data_valid <= 1'b1;
        adc_data_valid_tmp <= 1'b1;
      end else if (measurement_trigger) begin
        adc_data_valid <= adc_data_valid_tmp;
        adc_data_valid_tmp <= 1'b0;
      end
    end
  end




  //avalon write data 
  always @(posedge clk or negedge reset_n)             //write down to reg
    begin
    if (~reset_n) begin
      core_reset <= 1'b0;
      drv8320_en <= 0;
      core_en <= 1'b0;
      spi_divisor <= SPI_PRESCALER;
      adc_init <= 1'b0;
      filter_level <= 1'b0;
      pwm_prescaler <= PWM_PRES;
      half_period <= PWM_HALF_PERIOD;
      start_servo <= 0;
      protected_en <= {SERVO_NUM{1'b1}};
      trig_rate <= 1;
      u0 <= 0;
      u1 <= 0;
      u2 <= 0;
      u3 <= 0;
      u_valid <= 1'b0;
      ie <= 0;
      mode_0 <= C_MODE0;
      mode_1 <= C_MODE1;
      mode_2 <= C_MODE2;
      mode_3 <= C_MODE3;
      measurement_trigger_pendding <= 1'b0;
      control_step_pending <= 1'b0;
      realtime_err <= 1'b0;
    end else begin
      core_reset <= ((address == TR_OFFSET) && ~write_n) ? writedata[TR_RESET_BIT] : 1'b0;
      start_servo <= 
                ((address == TR_OFFSET) && ~write_n)? 
                writedata[TR_START_SERVO_BIT+SERVO_NUM-1:TR_START_SERVO_BIT]: 
                {SERVO_NUM{1'b0}};

      adc_init <= ((address == TR_OFFSET) && ~write_n) ? writedata[TR_ADC_INIT_BIT] : 1'b0;
      u_valid <= ((address == TR_OFFSET) && ~write_n) ? writedata[TR_U_VALID_BIT] : 1'b0;

      measurement_trigger_pendding <= 
                (measurement_trigger_pendding | measurement_trigger ) 
                & 
                ((~write_n && (
                    ((address == FLAG_OFFSET) && writedata[FLAG_MEA_TRIG_BIT])
                ))? 1'b0: 1'b1);

      control_step_pending <=
                ie[FLAG_MEA_TRIG_BIT]
                &             
                (control_step_pending | measurement_trigger ) 
                & 
                ((~write_n && (
                    ((address == FLAG_OFFSET) && writedata[FLAG_CONTROL_PENDING_BIT])
                ))? 1'b0: 1'b1);

      realtime_err <= 
                ((control_step_pending & measurement_trigger) | realtime_err)
                & ( (address == FLAG_OFFSET)? (write_n | ~writedata[FLAG_REAL_TIME_BIT]): 1'b1);



      if (~write_n) begin
        case (address)
          CR_OFFSET: begin
            core_en <= writedata[CR_EN_BIT];
            drv8320_en <= writedata[CR_DRV_EN_BIT+SERVO_NUM-1:CR_DRV_EN_BIT];
            if (~writedata[0] | ~core_en) begin
              protected_en <= writedata[CR_PROTECT_EN_BIT+SERVO_NUM-1:CR_PROTECT_EN_BIT];
              filter_level <= writedata[CR_FILTER_LEVEL_BIT+3:CR_FILTER_LEVEL_BIT];
            end
          end
          IE_OFFSET: ie <= writedata;
          SPI_PRES_OFFSET: begin
            if (~core_en) begin
              spi_divisor <= writedata;
            end
          end
          PWM_PRES_OFFSET: begin
            if (~core_en) begin
              pwm_prescaler <= writedata[14:0];
              half_period   <= writedata[30:16];
            end
          end
          PWM_TRIG_RATE: begin
            if (~core_en) begin
              trig_rate <= writedata;
            end
          end
          PULSE_MODE0_OFFSET: begin
            if (~core_en) begin
              mode_0 <= writedata;
            end
          end
          PULSE_MODE1_OFFSET: begin
            if (~core_en) begin
              mode_1 <= writedata;
            end
          end
          PULSE_MODE2_OFFSET: begin
            if (~core_en) begin
              mode_2 <= writedata;
            end
          end
          PULSE_MODE3_OFFSET: begin
            if (~core_en) begin
              mode_3 <= writedata;
            end
          end
          I0_MAX_OFFSET: begin
            if (~core_en) begin
              i0_max <= writedata[ADC_WIDTH-1:0];
            end
          end
          I1_MAX_OFFSET: begin
            if (~core_en) begin
              i1_max <= writedata[ADC_WIDTH-1:0];
            end
          end
          I2_MAX_OFFSET: begin
            if (~core_en) begin
              i2_max <= writedata[ADC_WIDTH-1:0];
            end
          end
          I3_MAX_OFFSET: begin
            if (~core_en) begin
              i3_max <= writedata[ADC_WIDTH-1:0];
            end
          end
          U0_OFFSET: begin
            u0 <= writedata;
          end
          U1_OFFSET: begin
            u1 <= writedata;
          end
          U2_OFFSET: begin
            u2 <= writedata;
          end
          U3_OFFSET: begin
            u3 <= writedata;
          end


        endcase
      end

    end
  end

  //avalon read data
  always @(*)            //read reg value      
    begin

    if (~read_n) begin
      case (address)
        CR_OFFSET: readdata <= {drv8320_en, protected_en, filter_level, core_en};
        IE_OFFSET: readdata <= ie;
        FLAG_OFFSET: readdata <= flag;
        SPI_PRES_OFFSET: readdata <= spi_divisor;
        PWM_PRES_OFFSET: readdata <= {1'b0, half_period, 1'b0, pwm_prescaler};
        PWM_TRIG_RATE: readdata <= trig_rate;
        PULSE_MODE0_OFFSET: readdata <= mode_0;
        PULSE_MODE1_OFFSET: readdata <= mode_1;
        PULSE_MODE2_OFFSET: readdata <= mode_2;
        PULSE_MODE3_OFFSET: readdata <= mode_3;
        U0_OFFSET: readdata <= u0;
        U1_OFFSET: readdata <= u1;
        U2_OFFSET: readdata <= u2;
        U3_OFFSET: readdata <= u3;
        // I0_MAX_OFFSET: readdata <= {i0_max, {16-ADC_WIDTH{1'b0}}};
        // I1_MAX_OFFSET: readdata <= {i1_max, {16-ADC_WIDTH{1'b0}}};
        // I2_MAX_OFFSET: readdata <= {i2_max, {16-ADC_WIDTH{1'b0}}};
        // I3_MAX_OFFSET: readdata <= {i3_max, {16-ADC_WIDTH{1'b0}}};
        // I0_OFFSET: readdata <= {i0_sample, {16-ADC_WIDTH{1'b0}}};
        // I1_OFFSET: readdata <= {i1_sample, {16-ADC_WIDTH{1'b0}}};
        // I2_OFFSET: readdata <= {i2_sample, {16-ADC_WIDTH{1'b0}}};
        // I3_OFFSET: readdata <= {i3_sample, {16-ADC_WIDTH{1'b0}}};
        // POS0_OFFSET: readdata <= {pos0, {16-ADC_WIDTH{1'b0}}};
        // POS1_OFFSET: readdata <= {pos1, {16-ADC_WIDTH{1'b0}}};
        // POS2_OFFSET: readdata <= {pos2, {16-ADC_WIDTH{1'b0}}};
        // POS3_OFFSET: readdata <= {pos3, {16-ADC_WIDTH{1'b0}}};

        I0_MAX_OFFSET: readdata <= i0_max;
        I1_MAX_OFFSET: readdata <= i1_max;
        I2_MAX_OFFSET: readdata <= i2_max;
        I3_MAX_OFFSET: readdata <= i3_max;
        I0_OFFSET: readdata <= i0_sample;
        I1_OFFSET: readdata <= i1_sample;
        I2_OFFSET: readdata <= i2_sample;
        I3_OFFSET: readdata <= i3_sample;
        POS0_OFFSET: readdata <= pos0;
        POS1_OFFSET: readdata <= pos1;
        POS2_OFFSET: readdata <= pos2;
        POS3_OFFSET: readdata <= pos3;

        DPOS_HALL0_OFFSET: readdata <= dpos_hall_0;
        DPOS_HALL1_OFFSET: readdata <= dpos_hall_1;
        DPOS_HALL2_OFFSET: readdata <= dpos_hall_2;
        DPOS_HALL3_OFFSET: readdata <= dpos_hall_3;

        POS_HALL0_OFFSET: readdata <= pos_hall_0;
        POS_HALL1_OFFSET: readdata <= pos_hall_1;
        POS_HALL2_OFFSET: readdata <= pos_hall_2;
        POS_HALL3_OFFSET: readdata <= pos_hall_3;

        HALL_OFFSET:
        readdata <= {hall_3_sync[5:3], hall_2_sync[5:3], hall_1_sync[5:3], hall_0_sync[5:3]};


        default: readdata <= 0;
      endcase
    end

  end

  // nFault_detect: 
  always @(posedge clk) begin
    if (~reset_n | core_reset) begin
      Fault_tmp <= 0;
      Fault_sync1 <= 0;
      Fault_sync0 <= 0;
      drv8320_fault <= 0;
      hall_0_sync <= 0;
      hall_1_sync <= 0;
      hall_2_sync <= 0;
      hall_3_sync <= 0;
    end else begin
      Fault_sync1 <= Fault_sync0;
      Fault_sync0 <= Fault_tmp;
      Fault_tmp <= ~nFault;

      drv8320_fault <= Fault_sync0 & Fault_sync1;


      hall_0_sync <= {hall_0_sync[2:0], hall_0};
      hall_1_sync <= {hall_1_sync[2:0], hall_1};
      hall_2_sync <= {hall_2_sync[2:0], hall_2};
      hall_3_sync <= {hall_3_sync[2:0], hall_3};
    end
  end

  always @(posedge clk) begin
    if (~reset_n | core_reset) begin
      i0_sample <= 0;
      i1_sample <= 0;
      i2_sample <= 0;
      i3_sample <= 0;
    end else begin
      if (measurement_trigger) begin
        i0_sample <= i0;
        i1_sample <= i1;
        i2_sample <= i2;
        i3_sample <= i3;

      end
    end
  end
endmodule
