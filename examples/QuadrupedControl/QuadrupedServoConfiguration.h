/*
 * QuadrupedServoConfiguration.h
 *
 *  Created on: 08.03.2019
 *      Author: Armin
 */

#ifndef SRC_QUADRUPEDSERVOCONFIGURATION_H_
#define SRC_QUADRUPEDSERVOCONFIGURATION_H_

/*
 * Servo stuff
 */
// some useful constants
#define LIFT_MAX_ANGLE 135  // upper mechanical limit
#define LIFT_MIN_ANGLE 40   // lower mechanical limit

#define MOVE_DIRECTION_FORWARD  0x00
#define MOVE_DIRECTION_LEFT     0x01
#define MOVE_DIRECTION_BACKWARD 0x02
#define MOVE_DIRECTION_RIGHT    0x03
#define MOVE_DIRECTION_SIDE_MASK    0x01

/*
 * Specification for basic creep
 * Angles are defined for front right leg
 */
#define Y_POSITION_OPEN_ANGLE  70 // the angle of both legs which are NOT together / upper part of Y
#define CREEP_BODY_MOVE_ANGLE  (Y_POSITION_OPEN_ANGLE - Y_POSITION_CLOSE_ANGLE)  // 50 the angle to move body
#define Y_POSITION_CLOSE_ANGLE 20 // the angle when both legs are together
#define Y_POSITION_FRONT_ANGLE (Y_POSITION_OPEN_ANGLE + CREEP_BODY_MOVE_ANGLE) // 120 the angle for the front leg after leg move forward

/*
 * Specification for basic turn
 */
#define TURN_BODY_ANGLE 40 // Angle per quarter turn
#define TURN_MOVE_ANGLE ((3 * TURN_BODY_ANGLE) / 2) // /2 since we start at 90 degree -> 150, and then with 3 turns-> 30
// For trot
#define TROT_BASE_ANGLE_FL_BR 135
#define TROT_BASE_ANGLE_BL_FR 45
#define TROT_MOVE_ANGLE 30
/*
 * Leg layout geometry
 */
#define NUMBER_OF_LEGS 4
#define NUMBER_OF_SERVOS 8
#define SERVOS_PER_LEG (NUMBER_OF_SERVOS / NUMBER_OF_LEGS)
#define DIAGONAL_SERVO_OFFSET (2 * SERVOS_PER_LEG)
#define LIFT_SERVO_OFFSET 1 // lift servo number is 1 more than base/pivot servo number

// Index into (external) servo array. Order must be the same as of definitions in main.
#define FRONT_LEFT_PIVOT 0
#define FRONT_LEFT_LEG (FRONT_LEFT_PIVOT / SERVOS_PER_LEG)
#define FRONT_LEFT_LIFT 1

#define BACK_LEFT_PIVOT 2
#define BACK_LEFT_LEG (BACK_LEFT_PIVOT / SERVOS_PER_LEG)
#define BACK_LEFT_LIFT 3

#define BACK_RIGHT_PIVOT 4
#define BACK_RIGHT_LEG (BACK_RIGHT_PIVOT / SERVOS_PER_LEG)
#define BACK_RIGHT_LIFT 5

#define FRONT_RIGHT_PIVOT 6
#define FRONT_RIGHT_LEG (FRONT_RIGHT_PIVOT / SERVOS_PER_LEG)
#define FRONT_RIGHT_LIFT 7

#endif /* SRC_QUADRUPEDSERVOCONFIGURATION_H_ */
