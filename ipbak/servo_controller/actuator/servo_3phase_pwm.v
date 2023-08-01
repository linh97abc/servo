module servo_3phase_pwm 
 (
    mode,
    stop,
    direction,

    hall, // a, b, c
    pwm,
    pwm_n,

    // ha, la, hb, lb, hc, lc
    phase
);

input [1:0] mode; //0-6x, 1-3x, 2-1x
input stop;
input direction;
input [2:0] hall;
input pwm;
input pwm_n;
output reg [5:0] phase;


wire pulse_n;

assign pulse_n = (mode == 0)? pwm_n: 1'b1;

always @(*) begin
    if (stop) begin
        phase = 0;
    end else begin
        if (mode == 2) begin
            phase <= {pwm, hall[2], hall[1], hall[0], direction, 1'b1};
        end else begin
            case (direction? hall: ~hall)
                3'b001: // b->c
                    phase <= {2'b00, pwm, pulse_n, 2'b01};
                3'b011: // a->c
                    phase <= {pwm, pulse_n, 2'b00 , 2'b01};
                3'b010: // a->b
                    phase <= {pwm, pulse_n, 2'b01 , 2'b00};
                3'b110: // c->b
                    phase <= {2'b00 , 2'b01, pwm, pulse_n};
                3'b100: // c->a
                    phase <= {2'b01 , 2'b00, pwm, pulse_n};
                3'b101: // b->a
                    phase <= {2'b01 , pwm, pulse_n, 2'b00};
                default: 
                    phase <= 0;
            endcase
        end
    end
end


endmodule