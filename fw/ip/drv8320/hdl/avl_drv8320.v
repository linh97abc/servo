module avl_drv8320
(
    //common
    clk,
    reset_n,

    address,
    writedata,
    write_n,
    read_n,    
    readdata,  

    irq,

    // In
    hall_0, // a, b, c
    hall_1, // a, b, c
    hall_2, // a, b, c
    hall_3, // a, b, c

    nFault,
    stop,
    drv8320_en,

    // Out
    phase_0,
    phase_1,
    phase_2,
    phase_3
);

parameter FREQ_CLK = 100_000_000;
parameter PWM_BASE_FREQ = 10_000_000;
parameter PWM_FREQ = 1000;
parameter [1:0] C_MODE0 = 0;
parameter [1:0] C_MODE1 = 0;
parameter [1:0] C_MODE2 = 0;
parameter [1:0] C_MODE3 = 0;

localparam DUTY_WIDTH = 16;
localparam PWM_PRES = FREQ_CLK/PWM_BASE_FREQ-1;
localparam PWM_HALF_PERIOD = PWM_BASE_FREQ/(2*PWM_FREQ) - 1;

localparam SERVO_NUM = 4;

input  wire         clk;
input  wire         reset_n;

    //avalon mm interface
input wire [5:0] address;
input wire [31:0] writedata;
input wire write_n;
input wire read_n;    
output reg [31:0] readdata;  
output irq;


input [2:0] hall_0;
input [2:0] hall_1;
input [2:0] hall_2;
input [2:0] hall_3;

input [SERVO_NUM-1:0] nFault;
input [SERVO_NUM-1:0] stop;
output reg [SERVO_NUM-1:0] drv8320_en;

output [5:0]    phase_0;
output [5:0]    phase_1;
output [5:0]    phase_2;
output [5:0]    phase_3;

reg         core_en;

wire [SERVO_NUM-1:0] start_servo;
reg [DUTY_WIDTH-2:0] pwm_prescaler;
reg [DUTY_WIDTH-2:0] half_period;
reg [5:0] trig_rate;
reg [SERVO_NUM-1:0] protected_en;
reg [DUTY_WIDTH-1:0] u0;
reg [DUTY_WIDTH-1:0] u1;
reg [DUTY_WIDTH-1:0] u2;
reg [DUTY_WIDTH-1:0] u3;



reg [1:0] mode_0;
reg [1:0] mode_1;
reg [1:0] mode_2;
reg [1:0] mode_3;

wire measurement_trigger;


reg measurement_trigger_pendding;
reg realtime_err;

reg [10: 0] ie;
wire [14: 0] flag; 

reg core_reset;

reg [SERVO_NUM-1:0] Fault_tmp;
reg [SERVO_NUM-1:0] Fault_sync0;
reg [SERVO_NUM-1:0] Fault_sync1;
reg [SERVO_NUM-1:0] drv8320_fault;

reg [2*SERVO_NUM-1:0] hall_0_sync;
reg [2*SERVO_NUM-1:0] hall_1_sync;
reg [2*SERVO_NUM-1:0] hall_2_sync;
reg [2*SERVO_NUM-1:0] hall_3_sync;

assign irq = (|(ie & flag[10:0])) & core_en;
assign flag = {start_servo, drv8320_fault, stop, 1'b0, measurement_trigger_pendding, realtime_err};


servo_pwm_control #
(
    .CORE_FREQUENCY(FREQ_CLK),
    .DUTY_WIDTH(DUTY_WIDTH)
)
servo_pwm_inst
(
    .clk(clk),
    .reset_n(reset_n),
    .core_en(core_en),
    .prescaler(pwm_prescaler),
    .half_period(half_period),

    // start
    .start_servo(start_servo),

    // slave 
    .u0(u0), // -1 .. 1
    .u1(u1),
    .u2(u2),
    .u3(u3),

    .measurement_trigger(measurement_trigger),

    .mode_0(mode_0),
    .mode_1(mode_1),
    .mode_2(mode_2),
    .mode_3(mode_3),


    // In
    .hall_0(hall_0_sync[2*SERVO_NUM-1:0]), // a, b, c
    .hall_1(hall_1_sync[2*SERVO_NUM-1:0]), // a, b, c
    .hall_2(hall_2_sync[2*SERVO_NUM-1:0]), // a, b, c
    .hall_3(hall_3_sync[2*SERVO_NUM-1:0]), // a, b, c
    // Out
    .phase_0(phase_0),
    .phase_1(phase_1),
    .phase_2(phase_2),
    .phase_3(phase_3)
);

localparam CR_EN_BIT = 0,
    CR_START_BIT = 1,
    CR_DRV_EN_BIT = 9;


localparam TR_U_VALID_BIT = 0,
    TR_RESET_BIT = 6;

localparam 
    FLAG_ACTIVE_BIT = 11,
    FLAG_DRV8320_FAULT_BIT = 7,
    FLAG_STOP_BIT = 3,    
    FLAG_MEA_TRIG_BIT = 1,
    FLAG_REAL_TIME_BIT = 0;

localparam CR_OFFSET = 0,
        TR_OFFSET = 1,
        IE_OFFSET = 2,
        FLAG_OFFSET = 3,
        TRIGGER_RATE_OFFSET = 4,
        PWM_PRES_OFFSET = 5,
        PWM_HPERIOD_OFFSET = 6,

        PULSE_MODE0_OFFSET = 7,
        PULSE_MODE1_OFFSET = PULSE_MODE0_OFFSET+1,
        PULSE_MODE2_OFFSET = PULSE_MODE0_OFFSET+2,
        PULSE_MODE3_OFFSET = PULSE_MODE0_OFFSET+3,
        
        U0_OFFSET = 11,
        U1_OFFSET = U0_OFFSET+1,
        U2_OFFSET = U0_OFFSET+2,
        U3_OFFSET = U0_OFFSET+3,

        HALL0_OFFSET = 15,
        HALL1_OFFSET = HALL0_OFFSET+1,
        HALL2_OFFSET = HALL0_OFFSET+2,
        HALL3_OFFSET = HALL0_OFFSET+3;


 //avalon write data 
    always @(posedge clk or negedge reset_n)             //write down to reg
    begin
        if(~reset_n) begin
            core_reset <= 1'b0;
            drv8320_en <= 0;
            core_en <= 1'b0;
            pwm_prescaler <= PWM_PRES;
            half_period <= PWM_HALF_PERIOD;
            
            u0 <= 0;
            u1 <= 0;
            u2 <= 0;
            u3 <= 0;

            ie <= 0;
            mode_0 <= C_MODE0;
            mode_1 <= C_MODE1;
            mode_2 <= C_MODE2;
            mode_3 <= C_MODE3;
        end
        else begin 
            core_reset <= ((address == TR_OFFSET) && ~write_n)? writedata[TR_RESET_BIT]: 1'b0;           

            measurement_trigger_pendding <= 
                (measurement_trigger_pendding | measurement_trigger ) 
                & 
                ((~write_n && (
                    ((address == FLAG_OFFSET) && writedata[FLAG_MEA_TRIG_BIT]) ||
                    ((address == TR_OFFSET) && writedata[TR_U_VALID_BIT])
                ))? 1'b0: 1'b1);


            
            realtime_err <= 
                (realtime_err | (measurement_trigger_pendding & measurement_trigger))
                & ( (address == FLAG_OFFSET)? (write_n | ~writedata[FLAG_REAL_TIME_BIT]): 1'b1);



            if(~write_n) begin
                case (address)
                CR_OFFSET: begin
                    core_en <= writedata[CR_EN_BIT];
                    drv8320_en <= writedata[CR_DRV_EN_BIT+SERVO_NUM-1:CR_DRV_EN_BIT];
                end
                IE_OFFSET: ie <= writedata;

                PWM_PRES_OFFSET: 
                   if (~core_en) begin
                        pwm_prescaler <= writedata;
                   end 
                 
                PWM_HPERIOD_OFFSET: 
                   if (~core_en) begin
                        half_period <= writedata;
                   end 
                 
                PULSE_MODE0_OFFSET: 
                   if (~core_en) begin
                        mode_0 <= writedata;
                   end 
                 
                PULSE_MODE1_OFFSET: 
                   if (~core_en) begin
                        mode_1 <= writedata;
                   end 
                 
                PULSE_MODE2_OFFSET: 
                   if (~core_en) begin
                        mode_2 <= writedata;
                   end 
                 
                PULSE_MODE3_OFFSET: 
                   if (~core_en) begin
                        mode_3 <= writedata;
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

                TRIGGER_RATE_OFFSET: if (~core_en) begin
                    trig_rate <= writedata;
                end 

                endcase 
            end
            
        end
    end
    
    //avalon read data
    always @(*)            //read reg value      
    begin

        if(~read_n) begin
            case (address)
                CR_OFFSET: readdata <= {drv8320_en, 4'b0, 4'b0, core_en};
                IE_OFFSET: readdata <= ie;
                FLAG_OFFSET: readdata <= flag;
                PWM_PRES_OFFSET: readdata <= pwm_prescaler;
                PWM_HPERIOD_OFFSET: readdata <= half_period;
                TRIGGER_RATE_OFFSET: readdata <= trig_rate;
                PULSE_MODE0_OFFSET: readdata <= mode_0;
                PULSE_MODE1_OFFSET: readdata <= mode_1;
                PULSE_MODE2_OFFSET: readdata <= mode_2;
                PULSE_MODE3_OFFSET: readdata <= mode_3;
                U0_OFFSET: readdata <= u0;
                U1_OFFSET: readdata <= u1;
                U2_OFFSET: readdata <= u2;
                U3_OFFSET: readdata <= u3;
                HALL0_OFFSET: readdata <= hall_0_sync[2*SERVO_NUM-1:0];
                HALL1_OFFSET: readdata <= hall_1_sync[2*SERVO_NUM-1:0];
                HALL2_OFFSET: readdata <= hall_2_sync[2*SERVO_NUM-1:0];
                HALL3_OFFSET: readdata <= hall_3_sync[2*SERVO_NUM-1:0];
                default: readdata <= 0;
            endcase
        end

    end   

    // nFault_detect: 
    always @(posedge clk) begin
        if (~reset_n) begin
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

            hall_0_sync <= {hall_0_sync[SERVO_NUM-1:0], hall_0};
            hall_1_sync <= {hall_1_sync[SERVO_NUM-1:0], hall_1};
            hall_2_sync <= {hall_2_sync[SERVO_NUM-1:0], hall_2};
            hall_3_sync <= {hall_3_sync[SERVO_NUM-1:0], hall_3};

            drv8320_fault <= Fault_sync0 & Fault_sync1;
        end
    end

    assign start_servo = drv8320_en & ~drv8320_fault &~stop;
endmodule