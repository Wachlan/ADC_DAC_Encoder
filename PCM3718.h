//Lachlan Chow z5164192
//MTRN3500 Assignment 1

#ifndef PCM3718_MODULE_H
#define PCM3718_MODULE_H

#include <stdint.h>
#include <sys/io.h>
#include <iostream>
#include <iomanip>
#include "EmbeddedOperations.h"
#include <math.h>


/*
 * For MTRN3500 Students - These methods and their respective signatures must not be changed. If 
 * they are and human intervention is required then marks will be deducted. You are more than
 * welcome to add private member variables and private methods to the provided classes, or create
 * your own underlying classes to provide the requested functionality as long as all of the
 * functionality for the PCM3718 can be accessed using the methods in the provided classes.
 */

namespace EmbeddedDevice {
  class PCM3718 {
    public:
      PCM3718(EmbeddedOperations *eops, uint32_t base_addr);
      PCM3718(EmbeddedOperations *eops, uint32_t base_addr, uint32_t analog_range);
      ~PCM3718();

      uint16_t digitalInput();
      uint8_t digitalByteInput(bool high_byte);
      bool digitalBitInput(uint8_t bit);
      void digitalOutput(uint16_t value);
      void digitalByteOutput(bool high_byte, uint8_t value);
      void setRange(uint32_t new_analog_range);
      double analogInput(uint8_t channel) const;
      friend std::ostream& operator<<(std::ostream& output, const PCM3718& pcm);

    private:
      // NOTE: All sys/io function calls must be made through the EmbeddedOperations class
      EmbeddedOperations *eops;
      uint32_t BASE;          //The base address of the PCM3718 object
      uint32_t rangeCode;     //The anlaog range code
      double range;           //The range of conversion
      double lowRange;        //Minimum range of conversion
  };

  // For newer versions of g++, a friend function needs a matching declaration provided
  // in the namespace scope to make it a member of the namespace.
  // You can ignore this line.
  std::ostream& operator<<(std::ostream& output, const PCM3718& pcm);

}


#endif // PCM3718_MODULE_H
