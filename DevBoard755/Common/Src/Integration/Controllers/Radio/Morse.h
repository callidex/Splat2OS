/*
 * Morse.h
 *
 *  Created on: Feb 20, 2022
 *      Author: paul
 */

#ifndef SRC_INTEGRATION_CONTROLLERS_RADIO_MORSE_H_
#define SRC_INTEGRATION_CONTROLLERS_RADIO_MORSE_H_

#include <string.h>

namespace Integration {

#define MORSE_MAX_TEXT_LENGTH (80)


class TimeRange
{
public:
	TimeRange()
	{
	    resetToNow();
	}
	void resetToNow()
	{
	    _stamp = HAL_GetTick();
	}


	//--------------------------------------------------------
	long millisecondsElapsed()
	{
	    unsigned long current = HAL_GetTick();
	    return current - _stamp;
	}

	//--------------------------------------------------------
	long secondsElapsed()
	{
	    return millisecondsElapsed() / 1000;
	}
private:
    unsigned long _stamp;
};

class Morse
{
#define DEFAULT_UNIT_DURATION_IN_MS (150)
#define WARMUP_PAUSE_IN_MS (3000)
#define ENDOFTEXT_PAUSE_IN_MS (3000)
#define LONG_RETRY_PAUSE_IN_MS (30000)
#define LONG_CARRIER_DURATION_IN_MS (4000)

#define MCODE(LEN, PATTERN) ( (PATTERN << 3) | ( LEN & 0x7 ) )
#define M_LEN(X) ( X & 0x7 )
#define M_PATTERN(X) ( X >> 3 )
	const unsigned char _morseTable[] = { '0', MCODE(5, 0b11111), '1', MCODE(
			5, 0b01111), '2', MCODE(5, 0b00111), '3', MCODE(5, 0b00011), '4', MCODE(
			5, 0b00001), '5', MCODE(5, 0b00000), '6', MCODE(5, 0b10000), '7', MCODE(
			5, 0b11000), '8', MCODE(5, 0b11100), '9', MCODE(5, 0b11110), '/', MCODE(
			5, 0b10010), 'A', MCODE(2, 0b01), 'B', MCODE(4, 0b1000), 'C', MCODE(4,
			0b1010), 'D', MCODE(3, 0b100), 'E', MCODE(1, 0b0), 'F', MCODE(4,
			0b0010), 'G', MCODE(3, 0b110), 'H', MCODE(4, 0b0000), 'I', MCODE(2,
			0b00), 'J', MCODE(4, 0b0111), 'K', MCODE(3, 0b101), 'L', MCODE(4,
			0b0100), 'M', MCODE(2, 0b11), 'N', MCODE(2, 0b10), 'O', MCODE(3, 0b111),
			'P', MCODE(4, 0b0110), 'Q', MCODE(4, 0b1101), 'R', MCODE(3, 0b010), 'S',
			MCODE(3, 0b000), 'T', MCODE(1, 0b1), 'U', MCODE(3, 0b001), 'V', MCODE(4,
					0b0001), 'W', MCODE(3, 0b011), 'X', MCODE(4, 0b1001), 'Y',
			MCODE(4, 0b1011), 'Z', MCODE(4, 0b1100), 0, 0 };
	// configurable params


    enum MorseState
    {
        msStopped = 0,
        msWarmUpPause,
        msSendingDit,
        msSendingDah,
        msSendingPauseBetweenDitDah,
        msSendingPauseBetweenLetters,
        msSendingSpace,
        msSendingLongCarrier,
        msEndOfText,
        msLongPauseBetweenRepeat
    };

public:
    Morse() {
    	_text[0] = 0;
    	_state = msStopped;
    	_currentPosInText = 0;
    	_unitDurationInMs = DEFAULT_UNIT_DURATION_IN_MS;
    }
    void setText(const char *text) {
    	size_t len = strlen(text);
    	if (len > MORSE_MAX_TEXT_LENGTH - 1) {
    		len = MORSE_MAX_TEXT_LENGTH - 1;
    	}
    	strncpy(_text, text, len + 1);
    }

    void setUnitDurationInMs(long unitDurationInMs) {
    	_unitDurationInMs = unitDurationInMs;
    }
    void handleTimeout() {
    	if (_state == msStopped) {
    		return;
    	}

    	if (_time.millisecondsElapsed() > _currentTimeoutInMS) {
    		_time.resetToNow();
    		changeStateByTimeout();
    	}
    }
    bool isTransmittingActive() const {
    	bool res;
    	res = (_state == msWarmUpPause) || (_state == msSendingDit)
    			|| (_state == msSendingDah) || (_state == msSendingLongCarrier)
    			|| (_state == msSendingPauseBetweenDitDah)
    			|| (_state == msSendingPauseBetweenLetters)
    			|| (_state == msSendingSpace) || (_state == msEndOfText);
    	return res;
    }
    bool isToneActive() const {
    	return (_state == msSendingDit) || (_state == msSendingDah)
    			|| (_state == msSendingLongCarrier);
    }


    void start() {
    	MorseState state = msWarmUpPause;
    	_currentTimeoutInMS = getTimeoutForState(state);
    	_state = state;
    }
    void stop() {
    	_state = msStopped;
    }

private:
    static const unsigned char _morseTable[42*2];

    MorseState _state;

    char _text[MORSE_MAX_TEXT_LENGTH];
    int _currentPosInText;

    TimeRange _time;
    long _currentTimeoutInMS;

    unsigned char _currentUtitsPattern;
    int _numUnitsRest;

    long _unitDurationInMs;

    MorseState MorseState Morse::prepareNextChar() {
    	char ch = _text[_currentPosInText];
    	_currentPosInText++;

    	if (ch == 0) {
    		_currentPosInText = 0;
    		return msEndOfText;
    	} else if (ch == ' ') {
    		return msSendingSpace;
    	} else if (ch == '_') {
    		return msSendingLongCarrier;
    	} else {
    		int length = 0;
    		unsigned char pattern = 0;
    		if (findCharInTable(ch, length, pattern)) {
    			// align pattern
    			pattern <<= (8 - length);

    			_currentUtitsPattern = pattern;
    			_numUnitsRest = length;
    			return prepareNextDitDah();
    		} else {
    			// send space
    			return msSendingSpace;
    		}

    	}

    	// never happen
    	return msSendingSpace;
    }
     MorseState Morse::prepareNextDitDah() {
    	if (_numUnitsRest == 0) {
    		return msSendingPauseBetweenLetters;
    	}

    	MorseState st = (_currentUtitsPattern & 0x80) ? msSendingDah : msSendingDit;
    	_currentUtitsPattern <<= 1;
    	_numUnitsRest -= 1;
    	return st;
    }
    bool findCharInTable(char ch, int &outLength,
    		unsigned char &outPattern) const {
    	size_t idx = 0;
    	while (_morseTable[idx]) {
    		if (_morseTable[idx] == ch) {
    			unsigned char encodedPatter = _morseTable[idx + 1];
    			int len = M_LEN(encodedPatter);
    			unsigned char pattern = M_PATTERN(encodedPatter);

    			outLength = len;
    			outPattern = pattern;
    			return true;
    		}

    		idx += 2;
    	}

    	// TODO: implement missed chars here

    	return false;
    }

    void changeStateByTimeout() {
    	if (_state == msStopped) {
    		// nothing to do. (should never happen)
    	} else if (_state == msWarmUpPause) {
    		// start sending
    		_currentPosInText = 0;
    		MorseState state = prepareNextChar(); // possible: dit, dah, space, longCarrier, endoftext
    		_currentTimeoutInMS = getTimeoutForState(state);
    		_state = state;
    	} else if (_state == msSendingDit || _state == msSendingDah) {
    		MorseState state = msSendingPauseBetweenDitDah;
    		_currentTimeoutInMS = getTimeoutForState(state);
    		_state = state;
    	} else if (_state == msSendingPauseBetweenDitDah) {
    		MorseState state = prepareNextDitDah(); // possible: dit, dah, pause_letters
    		_currentTimeoutInMS = getTimeoutForState(state);
    		_state = state;
    	} else if (_state == msSendingPauseBetweenLetters) {
    		MorseState state = prepareNextChar(); // possible: dit, dah, space, longCarrier, endoftext
    		_currentTimeoutInMS = getTimeoutForState(state);
    		_state = state;
    	} else if (_state == msSendingSpace || _state == msSendingLongCarrier) {
    		MorseState state = prepareNextChar(); // possible: dit, dah, space, longCarrier, endoftext
    		_currentTimeoutInMS = getTimeoutForState(state);
    		_state = state;
    	} else if (_state == msEndOfText) {
    		MorseState state = msLongPauseBetweenRepeat;
    		_currentTimeoutInMS = getTimeoutForState(state);
    		_state = state;
    	} else if (_state == msLongPauseBetweenRepeat) {
    		MorseState state = msWarmUpPause;
    		_currentTimeoutInMS = getTimeoutForState(state);
    		_state = state;
    	} else {
    		// never happen
    		MorseState state = msWarmUpPause;
    		_currentTimeoutInMS = getTimeoutForState(state);
    		_state = state;
    	}

    }
    long getTimeoutForState(MorseState state) const {
    	long res = 0;

    	long unitSizeX1 = _unitDurationInMs; // dit duration

    	if (state == msWarmUpPause) {
    		res = WARMUP_PAUSE_IN_MS;
    	} else if (state == msSendingDit) {
    		res = unitSizeX1;
    	} else if (state == msSendingDah) {
    		res = 3 * unitSizeX1;
    	} else if (state == msSendingPauseBetweenDitDah) {
    		res = unitSizeX1;
    	} else if (state == msSendingPauseBetweenLetters) {
    		res = 3 * unitSizeX1;
    	} else if (state == msSendingSpace) {
    		res = 7 * unitSizeX1;
    	} else if (state == msSendingLongCarrier) {
    		res = LONG_CARRIER_DURATION_IN_MS;
    	} else if (state == msEndOfText) {
    		res = ENDOFTEXT_PAUSE_IN_MS;
    	} else if (state == msLongPauseBetweenRepeat) {
    		res = LONG_RETRY_PAUSE_IN_MS;
    	} else {
    		// never happen
    		res = unitSizeX1;
    	}


} /* namespace Integration */

#endif /* SRC_INTEGRATION_CONTROLLERS_RADIO_MORSE_H_ */
