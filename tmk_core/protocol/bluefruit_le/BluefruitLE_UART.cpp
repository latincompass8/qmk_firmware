/**************************************************************************/
/*!
    Modified from the original Adafruit_BluefruitLE_UART.cpp which was
    released under the BSD License (below).

    Software License Agreement (BSD License)

    Copyright (c) 2020, Joshua Rubin
    Copyright (c) 2015, Adafruit Industries (adafruit.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include "BluefruitLE_UART.h"
#include "HardwareSerial.h"
#include "wait.h"
#include "print.h"

/******************************************************************************/
/*!
    @brief Instantiates a new instance of the BluefruitLE_UART class
*/
/******************************************************************************/
BluefruitLE_UART::BluefruitLE_UART() { _physical_transport = BLUEFRUIT_TRANSPORT_HWUART; }

/******************************************************************************/
/*!
    @brief Class's Destructor
*/
/******************************************************************************/
BluefruitLE_UART::~BluefruitLE_UART() { end(); }

/******************************************************************************/
/*!
    @brief Initialize the HW to enable communication with the BLE module

    @return Returns 'true' if everything initialised correctly, otherwise
            'false' if there was a problem during HW initialisation. If
            'irqPin' is not a HW interrupt pin false will be returned.
*/
/******************************************************************************/
bool BluefruitLE_UART::begin(uint32_t baud, bool debug, bool blocking) {
    _verbose = debug;

    Serial.begin(baud);
    Serial.setTimeout(_timeout);

    // reset Bluefruit module upon connect
    return reset(blocking);
}

/******************************************************************************/
/*!
    @brief  Uninitializes the SPI interface
*/
/******************************************************************************/
void BluefruitLE_UART::end(void) { Serial.end(); }

/******************************************************************************/
/*!
    @brief  Set the hardware MODE Pin if it is enabled, or performs a SW based
            mode switch if no MODE pin is available (SPI Friend, etc.)

    @param[in]  mode
                The mode to change to, either BLUEFRUIT_MODE_COMMAND or
                BLUEFRUIT_MODE_DATA

    @return     true if the mode switch was successful, otherwise false
*/
/******************************************************************************/
bool BluefruitLE_UART::setMode(uint8_t new_mode) {
    // invalid mode
    if (!(new_mode == BLUEFRUIT_MODE_COMMAND || new_mode == BLUEFRUIT_MODE_DATA)) return false;

    bool isOK;

    // Switch mode using +++ command, at worst switch 2 times
    int32_t updated_mode;

    isOK = atcommandIntReply(F("+++"), &updated_mode);

    if (isOK) {
        // Ahhh, we are already in the wanted mode before sending +++
        // Switch again. This is required to make sure it is always correct
        if (updated_mode != new_mode) {
            isOK = atcommandIntReply(F("+++"), &updated_mode);
            // Still does not match -> give up
            if (updated_mode != new_mode) return false;
        }
    }

    _mode = new_mode;

    return isOK;
}

/******************************************************************************/
/*!
    @brief  Print API. Either buffer the data internally or send it to bus
            if possible. \r and \n are command terminators and will force the
            packet to be sent to the Bluefruit LE module.

    @param[in]  c
                Character to send
*/
/******************************************************************************/
size_t BluefruitLE_UART::write(uint8_t c) {
    if (_verbose) xprintf("%c", c);
    wait_us(50);
    return Serial.write(c);
}

/******************************************************************************/
/*!
    @brief Check if the response from the previous command is ready

    @return 'true' if a response is ready, otherwise 'false'
*/
/******************************************************************************/
int BluefruitLE_UART::available(void) { return Serial.available(); }

/******************************************************************************/
/*!
    @brief Get a byte from response data, perform SPI transaction if needed

    @return -1 if no data is available
*/
/******************************************************************************/
int BluefruitLE_UART::read(void) { return Serial.read(); }

/******************************************************************************/
/*!
    @brief Get a byte from response without removing it, perform SPI transaction
           if needed

    @return -1 if no data is available
*/
/******************************************************************************/
int BluefruitLE_UART::peek(void) { return Serial.peek(); }

/******************************************************************************/
/*!
    @brief Flush current response data in the internal FIFO

    @return -1 if no data is available
*/
/******************************************************************************/
void BluefruitLE_UART::flush(void) { Serial.flush(); }
