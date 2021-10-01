/*
 * Unpublished Copyright (c) 2009-2021 AutonomouStuff, LLC, All Rights Reserved.
 *
 * This file is part of the PACMod ROS 1.0 driver which is released under the MIT license.
 * See file LICENSE included with this software or go to https://opensource.org/licenses/MIT for full license details.
 */

#include "pacmod_game_control/controllers.h"

#include <pacmod_msgs/SystemCmdInt.h>

// --- Generic gamepad controller (Logitech F310, XBOX)
Controller::Controller()
{
  axes_[JoyAxis::LEFT_STICK_LR] = 0;
  axes_[JoyAxis::LEFT_STICK_UD] = 1;

  axes_[JoyAxis::RIGHT_STICK_LR] = 3;
  axes_[JoyAxis::RIGHT_STICK_UD] = 4;

  axes_[JoyAxis::LEFT_TRIGGER_AXIS] = 2;
  axes_[JoyAxis::RIGHT_TRIGGER_AXIS] = 5;

  axes_[JoyAxis::DPAD_LR] = 6;
  axes_[JoyAxis::DPAD_UD] = 7;

  btns_[JoyButton::BOTTOM_BTN] = 0;
  btns_[JoyButton::RIGHT_BTN] = 1;
  btns_[JoyButton::LEFT_BTN] = 2;
  btns_[JoyButton::TOP_BTN] = 3;

  btns_[JoyButton::LEFT_BUMPER] = 4;
  btns_[JoyButton::RIGHT_BUMPER] = 5;

  btns_[JoyButton::BACK_SELECT_MINUS] = 6;
  btns_[JoyButton::START_PLUS] = 7;

  // Currently unused
  btns_[JoyButton::LEFT_STICK_PUSH] = 9;
  btns_[JoyButton::RIGHT_STICK_PUSH] = 10;
}

Controller::~Controller()
{
}

void Controller::set_controller_input(const sensor_msgs::Joy& joy_msg)
{
  input_msg_ = joy_msg;
}

float Controller::accelerator_value()
{
  // The triggers report 1.0 when untouched, and change negatively all the way to -1.0 when fully pressed
  return -(input_msg_.axes[axes_[JoyAxis::RIGHT_TRIGGER_AXIS]] - 1.0) / 2;
}

float Controller::brake_value()
{
  // The triggers report 1.0 when untouched, and change negatively all the way to -1.0 when fully pressed
  return -(input_msg_.axes[axes_[JoyAxis::LEFT_TRIGGER_AXIS]] - 1.0) / 2.0;
}

float Controller::steering_value(JoyAxis steering_axis)
{
  return input_msg_.axes[axes_[steering_axis]];
}

int Controller::turn_signal_cmd()
{
  // Left on directional pad
  if (input_msg_.axes[axes_[JoyAxis::DPAD_LR]] == AXES_MAX)
  {
    return pacmod_msgs::SystemCmdInt::TURN_LEFT;
  }
  // Right on directional pad
  else if (input_msg_.axes[axes_[JoyAxis::DPAD_LR]] == AXES_MIN)
  {
    return pacmod_msgs::SystemCmdInt::TURN_RIGHT;
  }
  // Down on directional pad
  else if (input_msg_.axes[axes_[JoyAxis::DPAD_UD]] == AXES_MIN)
  {
    return pacmod_msgs::SystemCmdInt::TURN_HAZARDS;
  }
  else
  {
    return pacmod_msgs::SystemCmdInt::TURN_NONE;
  }
}

int Controller::shift_cmd()
{
  uint8_t desired_gear = 0x0;
  desired_gear |=
      (input_msg_.buttons[btns_[JoyButton::RIGHT_BTN]] == BUTTON_DOWN) << pacmod_msgs::SystemCmdInt::SHIFT_REVERSE |
      (input_msg_.buttons[btns_[JoyButton::BOTTOM_BTN]] == BUTTON_DOWN) << pacmod_msgs::SystemCmdInt::SHIFT_HIGH |
      (input_msg_.buttons[btns_[JoyButton::TOP_BTN]] == BUTTON_DOWN) << pacmod_msgs::SystemCmdInt::SHIFT_PARK |
      (input_msg_.buttons[btns_[JoyButton::LEFT_BTN]] == BUTTON_DOWN) << pacmod_msgs::SystemCmdInt::SHIFT_NEUTRAL;

  switch (desired_gear)
  {
    case 1 << pacmod_msgs::SystemCmdInt::SHIFT_REVERSE:
      return pacmod_msgs::SystemCmdInt::SHIFT_REVERSE;
    case 1 << pacmod_msgs::SystemCmdInt::SHIFT_HIGH:
      return pacmod_msgs::SystemCmdInt::SHIFT_HIGH;
    case 1 << pacmod_msgs::SystemCmdInt::SHIFT_PARK:
      return pacmod_msgs::SystemCmdInt::SHIFT_PARK;
    case 1 << pacmod_msgs::SystemCmdInt::SHIFT_NEUTRAL:
      return pacmod_msgs::SystemCmdInt::SHIFT_NEUTRAL;
    // If we've got an invalid command (or multiple buttons pressed) return invalid
    default:
      return -1;
  }
}

bool Controller::horn_cmd()
{
  return (input_msg_.buttons[btns_[JoyButton::RIGHT_BUMPER]] == BUTTON_DOWN);
}

bool Controller::headlight_change()
{
  // Up on directional pad
  return (input_msg_.axes[axes_[JoyAxis::DPAD_UD]] == AXES_MAX);
}

bool Controller::wiper_change()
{
  return (input_msg_.buttons[btns_[JoyButton::LEFT_BUMPER]] == BUTTON_DOWN);
}

bool Controller::enable()
{
  return (input_msg_.buttons[btns_[JoyButton::START_PLUS]] == BUTTON_DOWN &&
          input_msg_.buttons[btns_[JoyButton::BACK_SELECT_MINUS]] == BUTTON_DOWN);
}

bool Controller::disable()
{
  return (input_msg_.buttons[btns_[JoyButton::BACK_SELECT_MINUS]] == BUTTON_DOWN &&
          input_msg_.buttons[btns_[JoyButton::START_PLUS]] != BUTTON_DOWN);
}

// --- Logitech G29, racing wheel with pedals
LogitechG29Controller::LogitechG29Controller()
{
  // steering wheel, not right stick
  axes_[JoyAxis::RIGHT_STICK_LR] = 0;
  // throttle pedal, not right trigger
  axes_[JoyAxis::RIGHT_TRIGGER_AXIS] = 2;
  // brake pedal, not left trigger
  axes_[JoyAxis::LEFT_TRIGGER_AXIS] = 3;
  axes_[JoyAxis::DPAD_LR] = 4;
  axes_[JoyAxis::DPAD_UD] = 5;

  btns_[JoyButton::BOTTOM_BTN] = 0;
  btns_[JoyButton::RIGHT_BTN] = 2;
  btns_[JoyButton::LEFT_BTN] = 1;
  btns_[JoyButton::TOP_BTN] = 3;

  // Following two are two blue buttons on the left
  btns_[JoyButton::LEFT_BUMPER] = 7;
  btns_[JoyButton::BACK_SELECT_MINUS] = 11;
  // Following two are two blue buttons on the right
  btns_[JoyButton::RIGHT_BUMPER] = 6;
  btns_[JoyButton::START_PLUS] = 10;
}

float LogitechG29Controller::accelerator_value()
{
  // The pedals report -1.0 when untouched, and change positively to 1.0 when full pressed
  return (input_msg_.axes[axes_[JoyAxis::RIGHT_TRIGGER_AXIS]] + 1.0) / 2.0;
}

float LogitechG29Controller::brake_value()
{
  // The pedals report -1.0 when untouched, and change positively to 1.0 when full pressed
  return (input_msg_.axes[axes_[JoyAxis::LEFT_TRIGGER_AXIS]] + 1.0) / 2.0;
}

// --- HRI Safe Remote Controller
HriSafeController::HriSafeController()
{
  axes_[JoyAxis::LEFT_STICK_LR] = 0;
  axes_[JoyAxis::LEFT_STICK_UD] = 1;

  axes_[JoyAxis::RIGHT_STICK_LR] = 3;
  axes_[JoyAxis::RIGHT_STICK_UD] = 4;

  axes_[JoyAxis::LEFT_TRIGGER_AXIS] = 2;
  axes_[JoyAxis::RIGHT_TRIGGER_AXIS] = 5;

  axes_[JoyAxis::DPAD_LR] = 6;
  axes_[JoyAxis::DPAD_UD] = 7;

  // TODO(jwhitleyastuff): Complete missing buttons
  btns_[JoyButton::BOTTOM_BTN] = 0;
  btns_[JoyButton::RIGHT_BTN] = 1;
  btns_[JoyButton::TOP_BTN] = 2;
  btns_[JoyButton::LEFT_BTN] = 3;

  // HRI Safe Remote has no bumper buttons!
}

float HriSafeController::accelerator_value()
{
  // Only consider center-to-up range as accelerator value
  if (input_msg_.axes[axes_[JoyAxis::RIGHT_TRIGGER_AXIS]] <= 0.0)
  {
    return -(input_msg_.axes[axes_[JoyAxis::RIGHT_TRIGGER_AXIS]]);
  }
  return 0.0;
}

float HriSafeController::brake_value()
{
  // Only consider center-to-up range as brake value
  if (input_msg_.axes[axes_[JoyAxis::LEFT_TRIGGER_AXIS]] <= 0.0)
  {
    return -(input_msg_.axes[axes_[JoyAxis::LEFT_TRIGGER_AXIS]]);
  }
  return 0.0;
}

int HriSafeController::turn_signal_cmd()
{
  if (input_msg_.axes[axes_[JoyAxis::RIGHT_STICK_UD]] < -0.5)
  {
    return pacmod_msgs::SystemCmdInt::TURN_HAZARDS;
  }
  else if (input_msg_.axes[axes_[JoyAxis::RIGHT_STICK_LR]] > 0.5)
  {
    return pacmod_msgs::SystemCmdInt::TURN_LEFT;
  }
  else if (input_msg_.axes[axes_[JoyAxis::RIGHT_STICK_LR]] < -0.5)
  {
    return pacmod_msgs::SystemCmdInt::TURN_RIGHT;
  }
  else
  {
    return pacmod_msgs::SystemCmdInt::TURN_NONE;
  }
}

bool HriSafeController::horn_cmd()
{
  // Currently no button assigned to horn commands
  return false;
}

bool HriSafeController::wiper_change()
{
  // Currently no button assigned to wiper commands
  return false;
}

bool HriSafeController::enable()
{
  return (input_msg_.axes[axes_[JoyAxis::DPAD_LR]] <= -0.9);
}

bool HriSafeController::disable()
{
  return (input_msg_.axes[axes_[JoyAxis::DPAD_LR]] >= 0.9);
}