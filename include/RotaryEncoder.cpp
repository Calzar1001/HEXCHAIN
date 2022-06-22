#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder()
{
    m_lastEventTime = 0; // Only need to initialize this variable (other parameters
    // will be set with setParameters() method).
}

void RotaryEncoder::init(int pinA, int pinB, int debounce_milliseconds,
                         int start_position, int min_position, int max_position)
{
    m_pinA = pinA;
    m_pinB = pinB;
    m_debounceMS = debounce_milliseconds;
    pinMode(m_pinA, INPUT_PULLUP);
    pinMode(m_pinA, HIGH); // turn on internal pullup
    pinMode(m_pinB, INPUT_PULLUP);
    pinMode(m_pinB, HIGH); // turn on internal pullup
    m_lastEventTime = 0;
    m_encoderPosition = start_position;
    m_encoderMin = min_position;
    m_encoderMax = max_position;
}

int RotaryEncoder::trackShaftPosition()
{
    boolean pinAValue = digitalRead(m_pinA);
    unsigned long current_time = millis();
    unsigned long time_between_events = current_time - m_lastEventTime;

    // Based on an example from Arduino Cookbook p. 191
    if ((m_encoderALast == HIGH) && (pinAValue == LOW) &&
        time_between_events > m_debounceMS)
    {
        if (digitalRead(m_pinB) == LOW)
        {
            if (m_encoderPosition > m_encoderMin)
                m_encoderPosition--;
        }
        else
        {
            if (m_encoderPosition < m_encoderMax)
                m_encoderPosition++;
        }
        m_lastEventTime = current_time;
    }

    m_encoderALast = pinAValue;
    return m_encoderPosition;
}

int RotaryEncoder::trackShaftPosition(unsigned long ticks)
{
    boolean pinAValue = digitalRead(m_pinA);
    unsigned long current_time = ticks;
    unsigned long time_between_events = current_time - m_lastEventTime;

    // Based on an example from Arduino Cookbook p. 191
    if ((m_encoderALast == HIGH) && (pinAValue == LOW) &&
        time_between_events > m_debounceMS)
    {
        if (digitalRead(m_pinB) == LOW)
        {
            if (m_encoderPosition > m_encoderMin)
                m_encoderPosition--;
        }
        else
        {
            if (m_encoderPosition < m_encoderMax)
                m_encoderPosition++;
        }
        m_lastEventTime = current_time;
    }

    m_encoderALast = pinAValue;
    return m_encoderPosition;
}
