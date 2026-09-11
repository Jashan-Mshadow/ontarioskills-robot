# Wiring

| Arduino | Connects to |
|---|---|
| D5  | L298N ENA — left drive speed (PWM) |
| D6  | L298N IN1 |
| D7  | L298N IN2 |
| D11 | L298N ENB — right drive speed (PWM) |
| D8  | L298N IN3 |
| D12 | L298N IN4 |
| D9  | Claw servo signal |
| D10 | Lift servo signal |
| GND | Common ground across Arduino, L298N and servo supply |

## Notes

**Separate the supplies.** Drive motors, servos and the Arduino should not share a single regulator.
Under a stalled claw the voltage sag will reset the board mid-run, which in competition is the worst
possible time to find out.

**Retune every timing constant for your chassis.** Every `delay()` in the movement helpers encodes a
distance for our specific wheel size, motor speed and battery. They are not portable numbers.

**Check the claw range before powering the lift.** A claw servo commanded past its mechanical limit will
stall and heat up quickly.
