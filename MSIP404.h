//Lachlan Chow z5164192
//MTRN3500 Assignment 1

#ifndef MSIP404_MODULE_H
#define MSIP404_MODULE_H

#include <stdint.h>
#include <sys/io.h>
#include <iostream>

#include "EmbeddedOperations.h"

//#define BASE 0x200   //making an assumption for the base address

/*
 * For MTRN3500 Students - These methods and their respective signatures must not be changed. If 
 * they are and human intervention is required then marks will be deducted. You are more than
 * welcome to add private member variables and private methods to the provided classes, or create
 * your own underlying classes to provide the requested functionality as long as all of the
 * functionality for the MSIP404 can be accessed using the methods in the provided classes.
 */

namespace EmbeddedDevice {
  class MSIP404 {
    public:
      MSIP404(EmbeddedOperations *eops, uint32_t base_addr);
      ~MSIP404();

      void resetChannel(uint8_t channel);
      int32_t readChannel(uint8_t channel);
      bool readIndex(uint8_t channel);
      bool operator!();

    private:
      // NOTE: All sys/io function calls must be made through the EmbeddedOperations class
      EmbeddedOperations *eops;
      uint32_t BASE; //base address of MSIP404 object
  };
}

#endif // MSIP404_MODULE_H
